#include "testmodel.h"


TestModel::TestModel(QObject *parent)
    : QAbstractTableModel(parent)
{}

int TestModel::rowCount(const QModelIndex &parent) const
{
    return m_rows;
}

int TestModel::columnCount(const QModelIndex &parent) const
{
    return m_columns;
}

QVariant TestModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

void TestModel::reset(int rows, int columns)
{
    beginResetModel();
    m_rows = rows;
    m_columns = columns;
    endResetModel();
}

void TestModel::insertColumnAt(int pos)
{
    beginInsertColumns(QModelIndex(), pos, pos);
    ++m_columns;
    endInsertColumns();
}

void TestModel::insertRowAt(int pos)
{
    beginInsertRows(QModelIndex(), pos, pos);
    ++m_rows;
    endInsertRows();
}

void TestModel::moveRow(int from, int to)
{
    beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
    endMoveRows();
}

void TestModel::moveColumn(int from, int to)
{
    beginMoveColumns(QModelIndex(), from, from, QModelIndex(), to);
    endMoveColumns();
}
