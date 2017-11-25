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

#include <algorithm>
#include <vector>
#include <cmath>
#include <numeric>

#include <range.h>

#include <optional>

#include "stdutils.h"

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
    void append(int visualLength)
    {
        m_ranges.emplace_back(1, visualLength);
        fixRanges();
    }

    bool move(int fromStart, int fromEnd, int to) {
        if (to >= fromStart && to <= (fromEnd + 1))
            return false;
        int length = (fromEnd + 1) - fromStart;
        while (length > 0) {
            const int visualLength = get(fromStart)->visualLength;
            removeAt(fromStart);
            insertAt(fromStart > to ? to : (to -1), visualLength);
            --length;
            if (to < fromStart) {
                ++to;
                ++fromStart;
            }
        }
        return true;
    }

    bool insertAt(int pos, int visualLength, int count = 1)
    {
        if (pos < 0 || pos > length())
            return false;

        int i = 0;
        for (auto it = m_ranges.begin(); it != m_ranges.end(); ++it) {
            const int start = i;
            const int end = start + it->length();
            if (pos == start) {
                // prepend
                m_ranges.insert(it, Range(count, visualLength));
                fixRanges();
                return true;
            } else if (pos == end) {
                // append after this range
                m_ranges.insert(std::next(it), Range(count, visualLength));
                fixRanges();
                return true;
            } else if (pos > start && pos < end) {
                // Split this range in two and add the new one in the middle
                *it = Range(pos - start, it->elementVisualLength());
                m_ranges.insert(std::next(it), {Range(count, visualLength),
                                                Range(end - pos, it->elementVisualLength())});
                fixRanges();
                return true;
            } else {
                // continue
                i = end;
            }
        }

        m_ranges.emplace_back(count, visualLength);
        fixRanges();
        return true;
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
        return stdutils::reduce(m_ranges, &Range::length, 0);
    }

    int visualLength() const
    {
        return stdutils::reduce(m_ranges, &Range::visualLength, 0);
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
        /*
        The current implementation is for efficient since fix the ranges
        in place. On the other hand is far more complex. This code
        snippet shows the same code in a simple but less efficient way.

        std::vector<Range> ranges;
        ranges.reserve(m_ranges.size());
        for (const Range &range : m_ranges) {
            if (range.empty())
                continue;
            if (ranges.empty())
                ranges.push_back(range);
            else if (ranges.rbegin()->elementVisualLength() == range.elementVisualLength())
                ranges.rbegin()->resize(ranges.rbegin()->length() + range.length());
            else
                ranges.push_back(range);
        }
        std::swap(m_ranges, ranges);
        */

        auto first = m_ranges.begin();
        auto last = m_ranges.end();
        auto previous = m_ranges.end();
        auto pivot = m_ranges.end();

        // Find a pivot iterator for determing the first element
        // from which the following should be removed
        for (; first != last; ) {
            if (first->empty()) {
                pivot = first;
                first = std::next(first);
                break;
            }
            if (previous != m_ranges.end() && previous->elementVisualLength() == first->elementVisualLength()) {
                previous->resize(previous->length() + first->length());
                first->resize(0);
            } else {
                previous = first;
                first = std::next(first);
            }
        }

        for (; first != last; first = std::next(first)) {
            if (first->empty()) {
                // Do nothing
            } else if (previous != pivot && previous != m_ranges.end() && previous->elementVisualLength() == first->elementVisualLength()) {
                previous->resize(previous->length() + first->length());
                first->resize(0);
            } else {
                std::iter_swap(pivot, first);
                previous = previous == m_ranges.end() ? first : std::next(previous);
                pivot = std::next(pivot);
            }
        }

        // Remove all the element from pivot to end
        if (pivot != m_ranges.end())
            m_ranges.erase(pivot, last);
    }

    std::vector<Range> m_ranges;
};
