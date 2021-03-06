/*
    This file is part of AdvancedViews.

    AdvancedViews is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AdvancedViews is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with AdvancedViews.  If not, see <http://www.gnu.org/licenses/>.
*/

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
}

TableViewPrivateElement::~TableViewPrivateElement()
{
    clearItem();
}

void TableViewPrivateElement::setCell(Cell c)
{
    m_cell = std::move(c);
    if (m_context) {
        m_context->setContextProperty("row", c.row());
        m_context->setContextProperty("column", c.column());
    }
    if (m_item) {
        m_item->setPosition(QPoint(m_cell.x(), m_cell.y()));
        m_item->setSize(QSize(m_cell.width(), m_cell.height()));
    }
}

bool TableViewPrivateElement::visible() const
{
    return m_visible;
}

void TableViewPrivateElement::setVisible(bool visible)
{
    if (m_visible == visible)
        return;
    m_visible = visible;
    if (m_item)
        m_item->setVisible(m_visible);
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
    /* do nothing */
}

void TableViewPrivateElement::onIncubatorSetInitialState(QObject *object)
{
    m_item.reset(qobject_cast<QQuickItem*>(object));
    m_item->setParentItem(&m_table);
    m_item->setX(m_cell.x());
    m_item->setY(m_cell.y());
    m_item->setWidth(m_cell.width());
    m_item->setHeight(m_cell.height());
    m_item->setZ(0);
    m_item->setVisible(m_visible);
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

std::unique_ptr<TableViewPrivateElement> TableViewPrivate::getOrCreateElement(Cell cell)
{
    std::unique_ptr<TableViewPrivateElement> result;
    if (m_cache.empty()) {
        result = std::make_unique<TableViewPrivateElement>(*this, std::move(cell));
        result->createItem();
    } else {
        result = std::move(m_cache.back());
        m_cache.pop_back();
        result->setCell(std::move(cell));
        result->setVisible(true);
    }
    return result;
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
    auto isVisible = [&visibleCells] (const auto& e){ return visibleCells.find(e->cell()) != visibleCells.end(); };
    const auto it = std::partition(m_elements.begin(), m_elements.end(), isVisible);
    std::for_each(it, m_elements.end(), [](const auto& element) { element->setVisible(false); });
    std::move(it, m_elements.end(), std::back_inserter(m_cache));
    m_elements.erase(it, m_elements.end());

    // Add new elements that become visible
    for (const Cell& v : visibleCells)
        if (currentCells.find(v) == currentCells.end())
            m_elements.push_back(getOrCreateElement(v));
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
