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
#include <cmath>
#include <numeric>
#include <optional>
#include <vector>

#include "range.h"
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

    struct FindRangeResult
    {
        using Iterator = std::vector<Range>::iterator;

        FindRangeResult(int start, Iterator iterator)
                : start(start)
                , iterator(iterator)
        {}

        int start;
        Iterator iterator;
    };

public:
    /**
     * Append a new element with \p visualLength visual length
     * @param visualLength The new element visual length
     */
    void append(int visualLength)
    {
        m_ranges.emplace_back(1, visualLength);
        fixRanges();
    }

    /**
     * Move the element starting from \p fromStart to \p fromEnd to \to
     * @param fromStart The first element to be removed
     * @param fromEnd The last element to be removed
     * @param to The position to be moved
     * @return true on success, false otherwise
     */
    bool move(int fromStart, int fromEnd, int to)
    {
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

    /**
     * Insert \p count elements starting from \p pos with \p visualLength visual length
     * @param pos The insert position
     * @param visualLength The elements visual length
     * @param count The number of elements to insert
     * @return true on success, false otherwise
     */
    bool insertAt(int pos, int visualLength, int count = 1)
    {
        const int length = this->length();

        if (pos < 0 || pos > length)
            return false;

        int start = -1;
        int end = -1;
        std::vector<Range>::iterator pivot;

        if (pos == length) {
            start = length;
            end = start + count;
            pivot = m_ranges.end();
        } else if (auto result = find_range_that_contains_index(pos)) {
            start = result->start;
            end = start + result->iterator->length();
            pivot = result->iterator;
        } else {
            return false;
        }

        if (pos == start) {
            // Insert the range
            m_ranges.insert(pivot, Range(count, visualLength));
        } else if (pos > start && pos < end){
            // Split this range in two and add the new one in the middle
            *pivot = Range(pos - start, pivot->elementVisualLength());
            m_ranges.insert(std::next(pivot), { Range(count, visualLength), Range(end - pos, pivot->elementVisualLength()) });
        } else {
            return false;
        }

        fixRanges();
        return true;
    }

    /**
     * Remove \p count elements starting from \p pos
     * @param pos The start element to be removed
     * @param count The number of elements to be removed
     * @return true on success, false otherwise
     */
    bool removeAt(int pos, int count = 1)
    {
        if (pos < 0 || (pos + count) > length() || count <= 0)
            return false;

        // Search for the range that contains the element at `pos`
        std::optional<FindRangeResult> find_result = find_range_that_contains_index(pos);

        // If we didn't found it, return false
        if (!find_result)
            return false;

        std::vector<Range>::iterator& pivot = find_result->iterator;
        int& start = find_result->start;
        int offset = pos - start;

        // Remove `count` elements
        while (pivot != m_ranges.end() && count > 0) {
            const int num_elements = std::min(pivot->length() - offset, count);
            pivot->reduce(num_elements);
            count -= num_elements;
            pivot = std::next(pivot);
            offset = 0;
        }

        // Fix ranges, in the case we empties some range in the removal
        fixRanges();

        return count <= 0;
    }

    /**
     * The remove the element at the given \p visualPos visual position
     * @param visualPos The visual position of the element to be removed
     * @return true on success, false otherwise
     */
    bool visualRemoveAt(int visualPos)
    {
        std::optional<AxisGetResult> result = visualGet(visualPos);
        return result ? removeAt(result->pos) : false;
    }

    /**
     * Return the number of elements in the axis
     */
    int length() const
    {
        return stdutils::reduce(m_ranges, &Range::length, 0);
    }

    /**
     * Return the visual length of the axis
     */
    int visualLength() const
    {
        return stdutils::reduce(m_ranges, &Range::visualLength, 0);
    }

    /**
     * Return the i-th element at \p pos
     * @param pos The element pos
     * @return The optional element at \p pos
     */
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

    /**
     * Return the element at the given \p visualPos visual position
     * @param visualPos The element visual position
     * @return The optional the element at the given visual position
     */
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
    /**
     * This function fixes the whole vector of ranges by eventually
     * removing empty ranges or merging ranges with the same visual size
     */
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

    std::optional<FindRangeResult> find_range_that_contains_index(int pos) {
        int start = 0;
        auto it = m_ranges.begin();
        while (it != m_ranges.end()) {
            const int end = start + it->length();
            if (pos < end)
                return FindRangeResult(start, it);
            it = std::next(it);
            start = end;
        }
        return std::optional<FindRangeResult>();
    }

    std::vector<Range> m_ranges;
};
