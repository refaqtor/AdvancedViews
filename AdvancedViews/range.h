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

class Range
{
public:
    constexpr Range(int numElements, int elementVisualLength)
        : m_numElements(numElements)
        , m_elementVisualLength(elementVisualLength)
    {}

    Range(const Range &other) = default;
    Range(Range &&other) noexcept = default;
    Range& operator=(const Range &other) = default;
    Range& operator=(Range &&other) noexcept = default;

    constexpr bool operator==(const Range &other) const
    {
        return m_numElements == other.m_numElements
                && m_elementVisualLength == other.m_elementVisualLength;
    }

    constexpr bool empty() const
    {
        return m_numElements <= 0;
    }

    constexpr int length() const
    {
        return m_numElements;
    }

    constexpr int elementVisualLength() const
    {
        return m_elementVisualLength;
    }

    constexpr int visualLength() const
    {
        return m_numElements * m_elementVisualLength;
    }

    constexpr void resize(int size)
    {
        m_numElements = size;
    }

    constexpr void enlarge(int amount)
    {
        m_numElements += amount;
    }

    constexpr void reduce(int amount)
    {
        amount = amount > m_numElements ? m_numElements : amount;
        m_numElements -= amount;
    }

    constexpr void setLength(int numElements)
    {
        m_numElements = numElements;
    }

private:
    int m_numElements = 0;
    int m_elementVisualLength = 0;
};
