#pragma once

#include "cell.h"
#include "table.h"

#include <memory>

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
    void onVisibleAreaChanged();
    void onCellDelegateChanged();

    void updateGeometry();

    Table m_table;
    QRect m_visibleArea;
    QPointer<QQmlComponent> m_cellDelegate;
    std::vector<std::unique_ptr<TableViewPrivateElement>> m_elements;
};

