#pragma once

#include <QRect>

class Cell
{
public:
    constexpr Cell()
        : m_row(0)
        , m_column(0)
        , m_rect(QRect())
    {}

    constexpr Cell(int row, int column, QRect rect)
        : m_row(row)
        , m_column(column)
        , m_rect(std::move(rect))
    {}

    Cell(const Cell& other)
        : m_row(other.m_row)
        , m_column(other.m_column)
        , m_rect(other.m_rect)
    {}

    constexpr int row() const { return m_row; }
    constexpr int column() const { return m_column; }
    constexpr QRect rect() const { return m_rect; }
    constexpr int x() const { return m_rect.x(); }
    constexpr int y() const { return m_rect.y(); }
    constexpr int width() const { return m_rect.width(); }
    constexpr int height() const { return m_rect.height(); }
    constexpr QSize size() const { return m_rect.size(); }
    constexpr QPoint point() const { return m_rect.topLeft(); }

    Cell& operator=(const Cell &other) {
        m_row = other.m_row;
        m_column = other.m_column;
        m_rect = other.m_rect;
        return *this;
    }

    bool operator==(const Cell &other) const
    {
        return m_row == other.m_row
                && m_column == other.m_column
                && m_rect == other.m_rect;
    }

private:
    int m_row = 0;
    int m_column = 0;
    QRect m_rect;
};
