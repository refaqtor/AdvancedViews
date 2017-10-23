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

    constexpr void setLength(int numElements)
    {
        m_numElements = numElements;
    }

private:
    int m_numElements = 0;
    int m_elementVisualLength = 0;
};
