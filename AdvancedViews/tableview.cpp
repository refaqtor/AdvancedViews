#include "tableview.h"

#include <QQmlEngine>
#include <set>

namespace
{

bool cellComparator(const Cell &c1, const Cell &c2)
{
    if (c1.row() < c2.row())
        return true;
    if (c1.column() < c2.column())
        return true;
    return false;
}

}

TableViewPrivateElement::TableViewPrivateElement(TableViewPrivate &table, Cell cell)
    : m_table(table)
    , m_cell(std::move(cell))
{}

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
    if (status == QQmlIncubator::Ready)
        m_item.reset(qobject_cast<QQuickItem*>(m_incubator->object()));
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
    setBoundingRect(m_table.boundingRect());
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
    using Set = std::set<Cell>;

    std::vector<Cell> cells = m_table.cellsInVisualRect(m_visibleArea);

    Set visibleCells(&cellComparator);
    visibleCells.insert(cells.begin(), cells.end());

    Set currentCells(&cellComparator);
    for (const auto &element : m_elements)
        currentCells.insert(element->cell());

    Set toRemove;
    std::set_difference(currentCells.begin(), currentCells.end(),
                        visibleCells.begin(), visibleCells.end(),
                        std::inserter(toRemove, toRemove.begin()));
    for (auto it = m_elements.begin(), end = m_elements.end(); it != end; ++it)
        if (toRemove.find((*it)->cell()) != toRemove.end())
            it = m_elements.erase(it);

    Set toAdd;
    std::set_difference(visibleCells.begin(), visibleCells.end(),
                        currentCells.begin(), currentCells.end(),
                        std::inserter(toAdd, toAdd.begin()));
    for (const Cell &cell : toAdd) {
        auto element = std::make_unique<TableViewPrivateElement>(*this, cell);
        element->createItem();
        m_elements.push_back(std::move(element));
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
