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
