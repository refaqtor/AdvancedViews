#pragma once

#include <QAbstractTableModel>

class TestModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TestModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent) const final;
    int columnCount(const QModelIndex &parent) const final;
    QVariant data(const QModelIndex &index, int role) const final;

    Q_INVOKABLE void reset(int rows, int columns);
    Q_INVOKABLE void insertColumnAt(int pos);
    Q_INVOKABLE void insertRowAt(int pos);
    Q_INVOKABLE void moveRow(int from, int to);
    Q_INVOKABLE void moveColumn(int from, int to);

private:
    int m_rows = 0;
    int m_columns = 0;
};
