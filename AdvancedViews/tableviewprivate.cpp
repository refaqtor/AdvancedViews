#include "tableviewprivate.h"

#include <QQmlEngine>
#include <iostream>
#include <set>
#include <unordered_set>

namespace
{

bool cellComparator(const Cell &c1, const Cell &c2)
{
    if (c1.row() < c2.row())
        return true;
    if (c1.row() > c2.row())
        return false;
    if (c1.column() < c2.column())
        return true;
    if (c1.column() > c2.column())
        return false;
    return false;
}

}

TableViewPrivateElement::TableViewPrivateElement(TableViewPrivate &table, Cell cell)
    : m_table(table)
    , m_cell(std::move(cell))
{
    createItem();
}

TableViewPrivateElement::~TableViewPrivateElement()
{
    clearItem();
}

void TableViewPrivateElement::createItem()
{
    Q_ASSERT(!m_incubator);
    Q_ASSERT(!m_item);
    Q_ASSERT(!m_context);

    QQmlContext *tableContext = QQmlEngine::contextForObject(&m_table);
    m_context = std::make_unique<QQmlContext>(tableContext, nullptr);
    m_context->setContextProperty("row", m_cell.row());
    m_context->setContextProperty("column", m_cell.column());

    m_incubator = std::make_unique<TableViewIncubator>(*this);

    m_table.cellDelegate()->create(*m_incubator, m_context.get(), tableContext);
}

void TableViewPrivateElement::clearItem()
{
    m_item.reset();
    m_context.reset();
    m_incubator.reset();
}

void TableViewPrivateElement::onIncubatorStatusChanged(QQmlIncubator::Status status)
{
    if (status == QQmlIncubator::Ready) {
        m_item.reset(qobject_cast<QQuickItem*>(m_incubator->object()));
    }
}

void TableViewPrivateElement::onIncubatorSetInitialState(QObject *object)
{
    auto item = qobject_cast<QQuickItem*>(object);
    item->setParentItem(&m_table);
    item->setX(m_cell.x());
    item->setY(m_cell.y());
    item->setWidth(m_cell.width());
    item->setHeight(m_cell.height());
}

TableViewPrivate::TableViewPrivate(QQuickItem *parent)
    : QQuickItem(parent)
{
    for (int i = 0; i < 1000; ++i) {
        m_table.xAxis().append(100);
        m_table.yAxis().append(100);
    }
    updateGeometry();
}

TableViewPrivate::~TableViewPrivate() = default;

QQmlComponent* TableViewPrivate::cellDelegate() const
{
    return m_cellDelegate;
}

QRect TableViewPrivate::visibleArea() const
{
    return m_visibleArea;
}

void TableViewPrivate::setCellDelegate(QQmlComponent *cellDelegate)
{
    if (m_cellDelegate == cellDelegate)
        return;

    m_cellDelegate = cellDelegate;
    emit cellDelegateChanged(m_cellDelegate);
    onCellDelegateChanged();
}

void TableViewPrivate::setVisibleArea(QRect visibleArea)
{
    if (m_visibleArea == visibleArea)
        return;

    m_visibleArea = visibleArea;
    emit visibleAreaChanged(m_visibleArea);
    onVisibleAreaChanged();
}

void TableViewPrivate::onVisibleAreaChanged()
{
    using CellSet = std::set<Cell, bool(*)(const Cell&, const Cell&)>;

    CellSet visibleCells(&cellComparator);
    const std::vector<Cell> visibleCellsVector = m_table.cellsInVisualRect(m_visibleArea);
    visibleCells.insert(visibleCellsVector.begin(), visibleCellsVector.end());

    CellSet currentCells(&cellComparator);
    for (const auto& e : m_elements)
        currentCells.insert(e->cell());

    // Remove elements that are not visibile anymore
    auto isVisible = [&visibleCells] (const auto& e){ return visibleCells.find(e->cell()) == visibleCells.end(); };
    m_elements.erase(std::remove_if(m_elements.begin(), m_elements.end(), isVisible), m_elements.end());

    // Add new elements that become visible
    for (const Cell& v : visibleCells) {
        if (currentCells.find(v) == currentCells.end()) {
            auto e = std::make_unique<TableViewPrivateElement>(*this, v);
            m_elements.push_back(std::move(e));
        }
    }
}

void TableViewPrivate::onCellDelegateChanged()
{
    for (const auto &element : m_elements)
        element->clearItem();
    if (m_cellDelegate.isNull())
        return;
    for (const auto &element : m_elements)
        element->createItem();
}

void TableViewPrivate::updateGeometry()
{
    auto rect = m_table.boundingRect();
    setSize(rect.size());
}

TableViewIncubator::TableViewIncubator(TableViewPrivateElement &element)
    : m_element(element)
{}

void TableViewIncubator::statusChanged(QQmlIncubator::Status status)
{
    m_element.onIncubatorStatusChanged(status);
}

void TableViewIncubator::setInitialState(QObject *object)
{
    m_element.onIncubatorSetInitialState(object);
}