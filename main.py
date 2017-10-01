import unittest


class Range:
    """
    This class represents a range of element with the same size
    """
    def __init__(self, num_elements, size):
        self.num_elements = num_elements
        self.size = size

    def __eq__(self, other):
        return self.num_elements == other.num_elements and self.size == other.size


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
        Remove the element at the given position
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
        Return the visual size for the element at the given position
        :param pos: The element position
        :return: Return the element size or None
        """
        if pos < 0:
            return None
        i = 0
        for r in self.ranges:
            i += r.num_elements
            if pos < i:
                return r.size
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

    def test_append(self):
        axis = Axis()
        axis.append(100)
        self.assertEqual(axis.ranges, [Range(1, 100)])

        axis.append(100)
        self.assertEqual(axis.ranges, [Range(2, 100)])

        axis.append(50)
        self.assertEqual(axis.ranges, [Range(2, 100), Range(1, 50)])

    def test_get(self):
        axis = Axis()
        axis.append(100)
        axis.append(100)
        axis.append(50)
        self.assertEqual(axis.get(0), 100)
        self.assertEqual(axis.get(1), 100)
        self.assertEqual(axis.get(2), 50)
        self.assertEqual(axis.get(3), None)
        self.assertEqual(axis.get(-1), None)

    def test_length(self):
        axis = Axis()
        self.assertEqual(axis.length(), 0)
        axis.append(100)
        self.assertEqual(axis.length(), 1)
        axis.append(50)
        self.assertEqual(axis.length(), 2)
        axis.append(100)
        self.assertEqual(axis.length(), 3)
        axis.append(100)
        self.assertEqual(axis.length(), 4)

    def test_visual_length(self):
        axis = Axis()
        self.assertEqual(axis.visual_length(), 0)
        axis.append(100)
        self.assertEqual(axis.visual_length(), 100)
        axis.append(50)
        self.assertEqual(axis.visual_length(), 150)
        axis.append(100)
        self.assertEqual(axis.visual_length(), 250)
        axis.append(100)
        self.assertEqual(axis.visual_length(), 350)

    def test_remove(self):
        axis = Axis()
        axis.append(100)
        axis.append(50)
        axis.append(100)
        axis.append(100)
        self.assertEqual(axis.ranges, [Range(1, 100), Range(1, 50), Range(2, 100)])
        self.assertEqual(axis.remove_at(-1), False)
        self.assertEqual(axis.ranges, [Range(1, 100), Range(1, 50), Range(2, 100)])
        self.assertEqual(axis.remove_at(4), False)
        self.assertEqual(axis.ranges, [Range(1, 100), Range(1, 50), Range(2, 100)])
        self.assertEqual(axis.remove_at(3), True)
        self.assertEqual(axis.ranges, [Range(1, 100), Range(1, 50), Range(1, 100)])
        self.assertEqual(axis.remove_at(1), True)
        self.assertEqual(axis.ranges, [Range(2, 100)])
        self.assertEqual(axis.remove_at(1), True)
        self.assertEqual(axis.ranges, [Range(1, 100)])
        self.assertEqual(axis.remove_at(0), True)
        self.assertEqual(axis.ranges, [])


if __name__ == '__main__':
    unittest.main()
