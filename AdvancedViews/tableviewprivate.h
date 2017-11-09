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
#include <queue>

#include <QAbstractItemModel>
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

    bool visible() const { return m_visible; }
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

class TableViewTaskQueue
{
public:
    using Task = std::function<void()>;

    TableViewTaskQueue() = default;
    TableViewTaskQueue(const TableViewTaskQueue&) = delete;
    TableViewTaskQueue& operator=(const TableViewTaskQueue&) = delete;
    TableViewTaskQueue(TableViewTaskQueue&&) = delete;
    TableViewTaskQueue& operator=(TableViewTaskQueue&&) = delete;

    void push(Task task);

private:
    bool m_executing = false;
    std::queue<Task> m_tasks;
};

class TableViewPrivate : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(TableViewPrivate)

    Q_PROPERTY(QQmlComponent* cellDelegate READ cellDelegate WRITE setCellDelegate NOTIFY cellDelegateChanged)
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QRect visibleArea READ visibleArea WRITE setVisibleArea NOTIFY visibleAreaChanged)

public:
    TableViewPrivate(QQuickItem *parent = nullptr);
    ~TableViewPrivate();

    QQmlComponent* cellDelegate() const;
    QAbstractItemModel* model() const;
    QRect visibleArea() const;

public slots:
    void setCellDelegate(QQmlComponent *cellDelegate);
    void setModel(QAbstractItemModel* model);
    void setVisibleArea(QRect visibleArea);

signals:
    void cellDelegateChanged(QQmlComponent *cellDelegate);
    void modelChanged(QAbstractItemModel* model);
    void visibleAreaChanged(QRect visibleArea);

private:
    std::unique_ptr<TableViewPrivateElement> getOrCreateElement(Cell c);

    void onVisibleAreaChanged();
    void onCellDelegateChanged();

    void onModelRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onModelRowsInserted(const QModelIndex &parent, int first, int last);
    void onModelRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onModelRowsRemoved(const QModelIndex &parent, int first, int last);
    void onModelRowsAboutToBeMoved( const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void onModelRowsMoved( const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void onModelColumnsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onModelColumnsInserted(const QModelIndex &parent, int first, int last);
    void onModelColumnsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onModelColumnsRemoved(const QModelIndex &parent, int first, int last);
    void onModelColumnsAboutToBeMoved( const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn);
    void onModelColumnsMoved( const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn);
    void onModelAboutToBeReset();
    void onModelReset();
    void onModelLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);
    void onModelLayoutChanged(const QList<QPersistentModelIndex> &parents, QAbstractItemModel::LayoutChangeHint hint);

    void updateGeometry();

    Table m_table;
    QRect m_visibleArea;
    QPointer<QQmlComponent> m_cellDelegate;
    QPointer<QAbstractItemModel> m_model;
    std::vector<std::unique_ptr<TableViewPrivateElement>> m_cache;
    std::vector<std::unique_ptr<TableViewPrivateElement>> m_elements;
    std::unique_ptr<TableViewTaskQueue> m_tasks;
};

