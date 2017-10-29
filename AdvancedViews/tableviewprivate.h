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

#pragma once

#include "cell.h"
#include "table.h"

#include <memory>
#include <stack>

#include <QQmlComponent>
#include <QQmlIncubator>
#include <QQmlContext>
#include <QPointer>
#include <QQuickItem>

class TableViewPrivate;
class TableViewPrivateElement;

class TableViewIncubator : public QQmlIncubator
{
public:
    TableViewIncubator(TableViewPrivateElement& element);

protected:
    void statusChanged(Status) final;
    void setInitialState(QObject *) final;

private:
    TableViewPrivateElement &m_element;
};

class TableViewPrivateElement
{
public:
    TableViewPrivateElement(TableViewPrivate& table, Cell cell);
    ~TableViewPrivateElement();

    Cell cell() const { return m_cell; }
    void setCell(Cell c);

    bool visible() const;
    void setVisible(bool visible);

    void createItem();
    void clearItem();

    void onIncubatorStatusChanged(QQmlIncubator::Status status);
    void onIncubatorSetInitialState(QObject *object);

private:
    TableViewPrivate &m_table;
    Cell m_cell;
    std::unique_ptr<QQmlContext> m_context;
    std::unique_ptr<QQmlIncubator> m_incubator;
    std::unique_ptr<QQuickItem> m_item;
    bool m_visible = true;
};

class TableViewPrivate : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(TableViewPrivate)

    Q_PROPERTY(QQmlComponent* cellDelegate READ cellDelegate WRITE setCellDelegate NOTIFY cellDelegateChanged)
    Q_PROPERTY(QRect visibleArea READ visibleArea WRITE setVisibleArea NOTIFY visibleAreaChanged)

public:
    TableViewPrivate(QQuickItem *parent = nullptr);
    ~TableViewPrivate();

    QQmlComponent* cellDelegate() const;
    QRect visibleArea() const;

public slots:
    void setCellDelegate(QQmlComponent *cellDelegate);
    void setVisibleArea(QRect visibleArea);

signals:
    void cellDelegateChanged(QQmlComponent *cellDelegate);
    void visibleAreaChanged(QRect visibleArea);

private:
    std::unique_ptr<TableViewPrivateElement> getOrCreateElement(Cell c);

    void onVisibleAreaChanged();
    void onCellDelegateChanged();

    void updateGeometry();

    Table m_table;
    QRect m_visibleArea;
    QPointer<QQmlComponent> m_cellDelegate;
    std::vector<std::unique_ptr<TableViewPrivateElement>> m_cache;
    std::vector<std::unique_ptr<TableViewPrivateElement>> m_elements;
};

