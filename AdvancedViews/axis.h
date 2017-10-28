#pragma once

#include <algorithm>
#include <vector>
#include <math.h>
#include <numeric>

#include <range.h>

#include <optional>

struct AxisGetResult
{
    constexpr AxisGetResult(int pos = -1, int visualPos = -1, int visualLength = -1)
        : pos(pos)
        , visualPos(visualPos)
        , visualLength(visualLength)
    {}

    constexpr bool operator==(const AxisGetResult& other) const {
        return pos == other.pos
                && visualPos == other.visualPos
                && visualLength == other.visualLength;
    }

    int pos;
    int visualPos;
    int visualLength;
};

class Axis
{
    friend class AdvancedViewsTest;

public:
    Axis()
    {}

    void append(int visualLength)
    {
        m_ranges.push_back(Range(1, visualLength));
        fixRanges();
    }

    bool removeAt(int pos)
    {
        if (pos < 0)
            return false;
        int i = 0;
        for (Range& r : m_ranges) {
            i += r.length();
            if (pos < i) {
                r.resize(r.length() - 1);
                fixRanges();
                return true;
            }
        }
        return false;
    }

    bool visualRemoveAt(int visualPos)
    {
        std::optional<AxisGetResult> result = visualGet(visualPos);
        return result ? removeAt(result->pos) : false;
    }

    int length() const
    {
        int result = 0;
        for (const Range& r : m_ranges)
            result += r.length();
        return result;
    }

    int visualLength() const
    {
        int result = 0;
        for (const Range& r : m_ranges)
            result += r.visualLength();
        return result;
    }

    std::optional<AxisGetResult> get(int pos) const
    {
        if (pos < 0)
            return std::optional<AxisGetResult>();
        int i = 0, v = 0;
        for (const Range &r : m_ranges) {
            if (pos < (i + r.length())) {
                AxisGetResult result;
                result.pos = pos;
                result.visualPos = v + (pos - i) * r.elementVisualLength();
                result.visualLength = r.elementVisualLength();
                return result;
            }
            i += r.length();
            v += r.visualLength();
        }
        return std::optional<AxisGetResult>();
    }

    std::optional<AxisGetResult> visualGet(int visualPos) const
    {
        std::optional<AxisGetResult> result;
        int minVisualPos = 0;
        int numElements = 0;
        int maxVisualPos = 0;
        for (const Range &range : m_ranges) {
            maxVisualPos = minVisualPos + range.visualLength();
            if (minVisualPos <= visualPos && visualPos < maxVisualPos) {
                const int visualOffset = visualPos - minVisualPos;
                const int pos = std::floor(visualOffset / range.elementVisualLength());
                result = AxisGetResult();
                result->pos = numElements + pos;
                result->visualPos = minVisualPos + pos * range.elementVisualLength();
                result->visualLength = range.elementVisualLength();
                return result;
            }
            minVisualPos = maxVisualPos;
            numElements += range.length();
        }
        return result;
    }

private:
    void fixRanges()
    {
        std::vector<Range> ranges;
        ranges.reserve(m_ranges.size());
        for (const Range &range : m_ranges) {
            if (range.empty())
                continue;
            if (ranges.empty())
                ranges.push_back(range);
            else if (ranges.rbegin()->elementVisualLength() == range.elementVisualLength())
                ranges.rbegin()->resize(ranges.rbegin()->length() + 1);
            else
                ranges.push_back(range);
        }
        std::swap(m_ranges, ranges);
    }

    std::vector<Range> m_ranges;
};
