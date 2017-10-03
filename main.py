import unittest
import math
import functools


class Range:
    """
    This class represents a range of element with the same size
    """

    def __init__(self, num_elements, size):
        """
        Initialize the range with the given number of elements and size
        :param num_elements: The number of elements
        :param size: The size of each element
        """
        self.num_elements = num_elements
        self.size = size

    def __eq__(self, other):
        return self.num_elements == other.num_elements and self.size == other.size

    def __ne__(self, other):
        return not self.__eq__(other)


class Axis:
    """
    This class manage a set of rows or columns in an axis.
    This class compact its elements by merging columns or rows
    with identical width or height
    """

    def __init__(self):
        self.ranges = []

    def append(self, size):
        """
        Append an element to the end of the axis
        :param size: the size of the element
        :return: None
        """
        self.ranges.append(Range(1, size))
        self._fix_ranges()

    def remove_at(self, pos):
        """
        Remove element at the given position
        :param pos: The element pos
        :return: True on success, False otherwise
        """
        if pos < 0:
            return False
        i = 0
        for r in self.ranges:
            i += r.num_elements
            if pos < i:
                r.num_elements -= 1
                self._fix_ranges()
                return True
        return False

    def visual_remove_at(self, visual_pos):
        """
        Remove the element at the given visual position
        :param visual_pos: The visual pos
        :return: True on success, False otherwise
        """
        element = self.visual_get(visual_pos)
        if element is None:
            return False
        (pos, size) = element
        return self.remove_at(pos)

    def length(self):
        """
        Return the number of elements contained in this Axis
        :return: Return the number of elements
        """
        result = 0
        for r in self.ranges:
            result += r.num_elements
        return result

    def visual_length(self):
        """
        Return the total visual length of the axis elements
        :return: The total visual length
        """
        result = 0
        for r in self.ranges:
            result += r.size * r.num_elements
        return result

    def get(self, pos):
        """
        Return the tuple (pos, size) at the given position
        :param pos: The element position
        :return: Return the element (pos, size) or None
        """
        if pos < 0:
            return None
        i = 0
        for r in self.ranges:
            i += r.num_elements
            if pos < i:
                return (pos, r.size)
        return None

    def visual_get(self, visual_pos):
        """
        Return the tuple (pos, size) at the given visual position
        :param visual_pos: The visual position
        :return: Return the element (pos, size) or None
        """
        if visual_pos < 0:
            return None
        pos = 0
        pip = 0
        for r in self.ranges:
            min = pos
            max = min + r.num_elements * r.size
            if min <= visual_pos and visual_pos < max:
                visual_pos -= min
                index = math.floor(visual_pos / r.size)
                return (pip + index, r.size)
            pos = max
            pip += r.num_elements
        return None

    def _fix_ranges(self):
        """
        Merge the ranges with same length and remove those empty
        :return: None
        """
        ranges = []
        for r in self.ranges:
            if r.num_elements == 0:
                pass
            elif len(ranges) == 0:
                ranges.append(r)
            elif ranges[-1].size == r.size:
                ranges[-1].num_elements += 1
            else:
                ranges.append(r)
        self.ranges = ranges


class AxisTest(unittest.TestCase):
    def setUp(self):
        self.axis = Axis()

    def test_append(self):
        self.axis.append(100)
        self.assertEqual(self.axis.ranges, [Range(1, 100)])
        self.axis.append(100)
        self.assertEqual(self.axis.ranges, [Range(2, 100)])
        self.axis.append(50)
        self.assertEqual(self.axis.ranges, [Range(2, 100), Range(1, 50)])

    def test_get(self):
        self.axis.append(100)
        self.axis.append(100)
        self.axis.append(50)
        self.assertEqual(self.axis.get(0), (0, 100))
        self.assertEqual(self.axis.get(1), (1, 100))
        self.assertEqual(self.axis.get(2), (2, 50))
        self.assertEqual(self.axis.get(3), None)
        self.assertEqual(self.axis.get(-1), None)

    def test_visual_get(self):
        self.axis.append(100)
        self.axis.append(100)
        self.axis.append(50)
        self.assertEqual(self.axis.visual_get(0), (0, 100))
        self.assertEqual(self.axis.visual_get(50), (0, 100))
        self.assertEqual(self.axis.visual_get(100), (1, 100))
        self.assertEqual(self.axis.visual_get(150), (1, 100))
        self.assertEqual(self.axis.visual_get(200), (2, 50))
        self.assertEqual(self.axis.visual_get(225), (2, 50))
        self.assertEqual(self.axis.visual_get(250), None)

    def test_length(self):
        self.assertEqual(self.axis.length(), 0)
        self.axis.append(100)
        self.assertEqual(self.axis.length(), 1)
        self.axis.append(50)
        self.assertEqual(self.axis.length(), 2)
        self.axis.append(100)
        self.assertEqual(self.axis.length(), 3)
        self.axis.append(100)
        self.assertEqual(self.axis.length(), 4)

    def test_visual_length(self):
        self.assertEqual(self.axis.visual_length(), 0)
        self.axis.append(100)
        self.assertEqual(self.axis.visual_length(), 100)
        self.axis.append(50)
        self.assertEqual(self.axis.visual_length(), 150)
        self.axis.append(100)
        self.assertEqual(self.axis.visual_length(), 250)
        self.axis.append(100)
        self.assertEqual(self.axis.visual_length(), 350)

    def test_remove_at(self):
        self.axis.append(100)
        self.axis.append(50)
        self.axis.append(100)
        self.axis.append(100)
        self.assertEqual(self.axis.ranges, [Range(1, 100), Range(1, 50), Range(2, 100)])
        self.assertEqual(self.axis.remove_at(-1), False)
        self.assertEqual(self.axis.ranges, [Range(1, 100), Range(1, 50), Range(2, 100)])
        self.assertEqual(self.axis.remove_at(4), False)
        self.assertEqual(self.axis.ranges, [Range(1, 100), Range(1, 50), Range(2, 100)])
        self.assertEqual(self.axis.remove_at(3), True)
        self.assertEqual(self.axis.ranges, [Range(1, 100), Range(1, 50), Range(1, 100)])
        self.assertEqual(self.axis.remove_at(1), True)
        self.assertEqual(self.axis.ranges, [Range(2, 100)])
        self.assertEqual(self.axis.remove_at(1), True)
        self.assertEqual(self.axis.ranges, [Range(1, 100)])
        self.assertEqual(self.axis.remove_at(0), True)
        self.assertEqual(self.axis.ranges, [])

    def test_visual_remove_at(self):
        self.axis.append(100)
        self.axis.append(50)
        self.axis.append(100)
        self.axis.append(100)
        self.assertEqual(self.axis.ranges, [Range(1, 100), Range(1, 50), Range(2, 100)])
        self.assertEqual(self.axis.visual_remove_at(-1), False)
        self.assertEqual(self.axis.ranges, [Range(1, 100), Range(1, 50), Range(2, 100)])
        self.assertEqual(self.axis.visual_remove_at(350), False)
        self.assertEqual(self.axis.ranges, [Range(1, 100), Range(1, 50), Range(2, 100)])
        self.assertEqual(self.axis.visual_remove_at(250), True)
        self.assertEqual(self.axis.ranges, [Range(1, 100), Range(1, 50), Range(1, 100)])
        self.assertEqual(self.axis.visual_remove_at(100), True)
        self.assertEqual(self.axis.ranges, [Range(2, 100)])
        self.assertEqual(self.axis.visual_remove_at(100), True)
        self.assertEqual(self.axis.ranges, [Range(1, 100)])
        self.assertEqual(self.axis.visual_remove_at(0), True)
        self.assertEqual(self.axis.ranges, [])


@functools.total_ordering
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __eq__(self, other):
        return self.x == other.x and \
               self.y == other.y

    def __ne__(self, other):
        return not self.__eq__(other)

    def __lt__(self, other):
        if self.x < other.x:
            return True
        if self.y < other.y:
            return True
        return False


class Rect:
    def __init__(self, top_left, bottom_right):
        self.top_left = top_left
        self.bottom_right = bottom_right

    @staticmethod
    def from_xy(x1, y1, x2, y2):
        return Rect(Point(x1, y1), Point(x2, y2))

    @staticmethod
    def from_points(top_left, bottom_right):
        return Rect(top_left, bottom_right)

    @staticmethod
    def from_point_and_size(top_left, width, height):
        return Rect(top_left, Point(top_left.x + width, top_left.y + height))

    def __eq__(self, other):
        return self.top_left == other.top_left and \
               self.bottom_right == other.bottom_right

    def __ne__(self, other):
        return not self.__eq__(other)


@functools.total_ordering
class Cell:
    def __init__(self, row, column):
        self.row = row
        self.column = column

    def __eq__(self, other):
        return self.row == other.row and self.column == other.column

    def __ne__(self, other):
        return not self.__eq__(other)

    def __lt__(self, other):
        if self.row < other.row:
            return True
        if self.column < other.column:
            return True
        return False


class Table:
    def __init__(self):
        self.xAxis = Axis()
        self.yAxis = Axis()

    def bounding_rect(self):
        """
        Return the visual rect for the given table
        :return:
        """
        return self.xAxis.visual_length(), self.yAxis.visual_length()

    def cells_in_visual_rect(self, rect):
        """
        Return the cells in the given visual rect
        """
        result = []
        return result


class TableTest(unittest.TestCase):
    def setUp(self):
        self.table = Table()

    def test_bounding_rect(self):
        self.assertEqual(self.table.bounding_rect(), (0, 0))
        self.table.xAxis.append(100)
        self.table.yAxis.append(50)
        self.assertEqual(self.table.bounding_rect(), (100, 50))

    def test_cells_in_rect(self):
        self.table.xAxis.append(100)
        self.table.xAxis.append(100)
        self.table.yAxis.append(50)
        self.table.yAxis.append(50)
        self.table.yAxis.append(50)
        self.assertEqual(self.table.bounding_rect(), (200, 150))
        cells = self.table.cells_in_visual_rect(Rect.from_xy(0, 0, 200, 150))
        self.assertEqual(cells, [Cell(0, 0), Cell(0, 1), Cell(0, 2),
                                 Cell(1, 0), Cell(1, 1), Cell(1, 2)])


if __name__ == '__main__':
    unittest.main()
