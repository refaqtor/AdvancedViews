import unittest
import math
from functools import reduce, total_ordering


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

    @property
    def visual_length(self):
        return self.num_elements * self.size

    @visual_length.setter
    def visual_length(self, value):
        raise Exception('Range.visual_length is a readonly property')

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

    @property
    def length(self):
        """
        Return the number of elements contained in this Axis
        :return: Return the number of elements
        """
        return reduce(lambda tot, rng: tot + rng.num_elements, self.ranges, 0)

    @property
    def visual_length(self):
        """
        Return the total visual length of the axis elements
        :return: The total visual length
        """
        return reduce(lambda tot, rng: tot + rng.size * rng.num_elements, self.ranges, 0)


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
        self.assertEqual(self.axis.length, 0)
        self.axis.append(100)
        self.assertEqual(self.axis.length, 1)
        self.axis.append(50)
        self.assertEqual(self.axis.length, 2)
        self.axis.append(100)
        self.assertEqual(self.axis.length, 3)
        self.axis.append(100)
        self.assertEqual(self.axis.length, 4)

    def test_visual_length(self):
        self.assertEqual(self.axis.visual_length, 0)
        self.axis.append(100)
        self.assertEqual(self.axis.visual_length, 100)
        self.axis.append(50)
        self.assertEqual(self.axis.visual_length, 150)
        self.axis.append(100)
        self.assertEqual(self.axis.visual_length, 250)
        self.axis.append(100)
        self.assertEqual(self.axis.visual_length, 350)

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


@total_ordering
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

    def __repr__(self):
        return '({}, {})'.format(self.x, self.y)

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

    def intersection(self, other):
        """
        Return the intersection of this rect with other
        """
        if self.top_left.x >= other.bottom_right.x \
            or self.bottom_right.x <= other.top_left.x \
            or self.top_left.y >= other.bottom_right.y \
            or self.bottom_right.y <= other.top_left.y:
            return None
        top_left = Point(max(self.top_left.x, other.top_left.x),
                         max(self.top_left.y, other.top_left.y))
        bottom_right = Point(min(self.bottom_right.x, other.bottom_right.x),
                             min(self.bottom_right.y, other.bottom_right.y))
        if top_left.x >= bottom_right.x \
           or top_left.y >= bottom_right.y:
            return None
        return Rect.from_points(top_left, bottom_right)

    @property
    def width(self):
        """
        Return the rectangle width
        """
        return self.right - self.left

    @width.setter
    def width(self, width):
        self.right = self.left + width

    @property
    def height(self):
        """
        Return the rectangle height
        """
        return self.bottom - self.top

    @height.setter
    def height(self, height):
        self.bottom = self.top + height

    @property
    def left(self):
        """
        Return the left edge coordinate
        """
        return self.top_left.x

    @left.setter
    def left(self, value):
        self.top_left.x = value

    @property
    def right(self):
        """
        Return the right edge coordinate
        """
        return self.bottom_right.x

    @right.setter
    def right(self, value):
        self.bottom_right.x = value    

    @property
    def top(self):
        """
        Return the top edge coordinate
        """
        return self.top_left.y

    @top.setter
    def top(self, value):
        self.top_left.y = value

    @property
    def bottom(self):
        """
        Return the bottom edge coordinate
        """
        return self.bottom_right.y

    @bottom.setter
    def bottom(self, value):
        self.bottom_right.y = value

    def __eq__(self, other):
        if other is None:
            return False
        return self.top_left == other.top_left and \
               self.bottom_right == other.bottom_right

    def __ne__(self, other):
        return not self.__eq__(other)

    def __repr__(self):
        return '({}, {})'.format(self.top_left, self.bottom_right)


class RectTest(unittest.TestCase):
    def test_intersection(self):
        rect = Rect.from_xy(0, 0, 100, 100)
        self.assertEqual(rect.intersection(Rect.from_xy(100, 100, 200, 200)), None)
        self.assertEqual(rect.intersection(Rect.from_xy(50, 50, 100, 100)), Rect.from_xy(50, 50, 100, 100))
        self.assertEqual(rect.intersection(Rect.from_xy(0, 0, 100, 100)), Rect.from_xy(0, 0, 100, 100))
        self.assertEqual(rect.intersection(Rect.from_xy(-20, -20, 20, 20)), Rect.from_xy(0, 0, 20, 20))
        self.assertEqual(rect.intersection(Rect.from_xy(80, 80, 120, 120)), Rect.from_xy(80, 80, 100, 100))
        self.assertEqual(rect.intersection(Rect.from_xy(120, 120, 140, 140)), None)
        self.assertEqual(rect.intersection(Rect.from_xy(-20, 20, 0, 0)), None)

    def test_width_and_height(self):
        rect = Rect.from_xy(0, 0, 100, 80)
        self.assertEqual(rect.width, 100)
        self.assertEqual(rect.height, 80)
        rect.width = 50
        self.assertEqual(rect.width, 50)
        self.assertEqual(rect.right, 50)
        rect.height = 40
        self.assertEqual(rect.bottom, 40)
        self.assertEqual(rect.height, 40)

    def test_edges(self):
        rect = Rect.from_xy(0, 20, 100, 100)
        self.assertEqual(rect.left, 0)
        self.assertEqual(rect.top, 20)
        self.assertEqual(rect.right, 100)
        self.assertEqual(rect.bottom, 100)


@total_ordering
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

    def __repr__(self):
        return '(' + str(self.row) + ',' + str(self.column) + ')'


class Table:
    def __init__(self):
        self.xAxis = Axis()
        self.yAxis = Axis()

    @property
    def bounding_rect(self):
        """
        Return the visual rect for the given table
        :return:
        """
        top_left = Point(0, 0)
        bottom_right = Point(self.xAxis.visual_length,
                             self.yAxis.visual_length)
        return Rect(top_left, bottom_right)

    @bounding_rect.setter
    def bounding_rect(self, value):
        raise Exception('Bounding rect of Table is readonly')

    def cells_in_visual_rect(self, rect):
        """
        Return the cells in the given visual rect
        """
        x_min = max(rect.left, 0)
        x_max = min(rect.right, self.xAxis.visual_length - 1)
        y_min = max(rect.top, 0)
        y_max = min(rect.bottom, self.yAxis.visual_length - 1)
        column_min, _ = self.xAxis.visual_get(x_min)
        column_max, _ = self.xAxis.visual_get(x_max)
        row_min, _ = self.yAxis.visual_get(y_min)
        row_max, _ = self.yAxis.visual_get(y_max)
        result = []
        for c in range(column_min, column_max + 1):
            for r in range(row_min, row_max + 1):
                result.append(Cell(r, c))
        result.sort()
        return result

class TableTest(unittest.TestCase):
    def setUp(self):
        self.table = Table()

    def test_bounding_rect(self):
        self.assertEqual(self.table.bounding_rect, Rect.from_xy(0, 0, 0, 0))
        self.table.xAxis.append(100)
        self.table.yAxis.append(50)
        self.assertEqual(self.table.bounding_rect, Rect.from_xy(0, 0, 100, 50))

    def test_cells_in_rect(self):
        self.table.xAxis.append(100)
        self.table.xAxis.append(100)
        self.table.yAxis.append(50)
        self.table.yAxis.append(50)
        self.table.yAxis.append(50)
        self.assertEqual(self.table.bounding_rect, Rect.from_xy(0, 0, 200, 150))
        cells = self.table.cells_in_visual_rect(Rect.from_xy(0, 0, 200, 150))
        self.assertEqual(cells, [Cell(0, 0), Cell(0, 1),
                                 Cell(1, 0), Cell(1, 1),
                                 Cell(2, 0), Cell(2, 1)])


if __name__ == '__main__':
    unittest.main()
