#pragma once

#include <QRect>

#include <axis.h>
#include <cell.h>

class Table
{
    friend class AdvancedViewsTest;

public:
    QRect boundingRect() const
    {
        return QRect(0, 0, m_xAxis.visualLength(), m_yAxis.visualLength());
    }

    std::vector<Cell> cellsInVisualRect(QRect rect) const
    {
        rect = rect.intersected(boundingRect());
        if (!rect.isValid())
            return std::vector<Cell>();
        const int columnMin = m_xAxis.visualGet(rect.left())->pos;
        const int columnMax = m_xAxis.visualGet(rect.right() - 1)->pos;
        const int rowMin = m_yAxis.visualGet(rect.top())->pos;
        const int rowMax = m_yAxis.visualGet(rect.bottom() - 1)->pos;

        std::vector<Cell> result;
        for (int c = columnMin; c <= columnMax; ++c) {
            for (int r = rowMin; r <= rowMax; ++r) {
                const auto row = m_yAxis.get(r);
                const auto column = m_xAxis.get(c);
                result.emplace_back(row->pos, column->pos,
                                    QRect(column->visualPos, row->visualPos,
                                          column->visualLength, row->visualLength));
            }
        }
        auto predicate = [](const Cell &l, const Cell &r){
            if (l.row() < r.row())
                return true;
            if (l.column() < r.column())
                return true;
            return false;
        };
        std::stable_sort(result.begin(), result.end(), predicate);
        return result;
    }

    Axis& xAxis() { return m_xAxis; }
    Axis& yAxis() { return m_yAxis; }

private:
    Axis m_xAxis;
    Axis m_yAxis;
};
