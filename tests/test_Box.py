#!/usr/bin/env python
"""Test code for the Box class."""
from spanops import Interval, Box, Span
import unittest


class BoxTestCase(unittest.TestCase):

    def test_empty(self):
        a = Box()
        self.assertTrue(a.empty)
        b = Box(x=Interval(3, 1), y=Interval(1, 2))
        self.assertTrue(b.empty)
        c = Box(x=Interval(1, 4), y=Interval(4, 2))
        self.assertTrue(c.empty)

    def test_standard(self):
        a = Box(x=Interval(3, 5), y=Interval(7, 9))
        self.assertEqual(a.width, 3)
        self.assertEqual(a.x0, 3)
        self.assertEqual(a.x1, 5)
        self.assertEqual(a.x, Interval(3, 5))
        self.assertEqual(a.height, 3)
        self.assertEqual(a.y0, 7)
        self.assertEqual(a.y1, 9)
        self.assertEqual(a.y, Interval(7, 9))
        self.assertFalse(a.empty)
        self.assertEqual(a.area, 9)

    def test_intersect(self):
        a = Interval(1, 4)
        b = Interval(3, 6)
        c = Interval(3, 4)
        d = Interval(-2, 0)
        self.assertEqual(Box(a, a) & Box(b, a), Box(c, a))
        self.assertTrue((Box(a, a) & Box(d, a)).empty)
        box = Box(a, a)
        box &= Box(b, a)
        self.assertEqual(box, Box(c, a))
        box = Box(a, a)
        box &= Box(d, a)
        self.assertTrue(box.empty)

    def test_overlaps(self):
        a = Interval(1, 4)
        c = Interval(3, 4)
        d = Interval(-2, 0)
        self.assertFalse(Box(c, c).overlaps(Box(d, d)))
        self.assertFalse(Box(c, c).overlaps(Box(a, d)))
        self.assertFalse(Box(c, c).overlaps(Box(d, a)))
        self.assertTrue(Box(c, c).overlaps(Box(a, a)))

    def test_expand_and_overlaps(self):
        def check_in_place(box, *args):
            copy = Box(box.x, box.y)
            copy.expand_to(*args)
            self.assertEqual(box.expanded_to(*args), copy)
        a = Interval(1, 4)
        b = Interval(3, 6)
        c = Interval(3, 4)
        d = Interval(1, 3)
        e = Interval(4, 6)
        box1 = Box(x=c, y=c)
        # x, y overload
        self.assertEqual(box1.expanded_to(x=1, y=3), Box(x=a, y=c))
        check_in_place(box1, 1, 3)
        self.assertEqual(box1.expanded_to(x=3, y=1), Box(x=c, y=a))
        check_in_place(box1, 3, 1)
        self.assertEqual(box1.expanded_to(x=6, y=3), Box(x=b, y=c))
        check_in_place(box1, 6, 3)
        self.assertEqual(box1.expanded_to(x=3, y=6), Box(x=c, y=b))
        check_in_place(box1, 3, 6)
        # Span overload
        self.assertEqual(box1.expanded_to(Span(x=d, y=3)), Box(x=a, y=c))
        check_in_place(box1, Span(x=d, y=3))
        self.assertEqual(box1.expanded_to(Span(x=c, y=1)), Box(x=c, y=a))
        check_in_place(box1, Span(x=c, y=1))
        self.assertEqual(box1.expanded_to(Span(x=e, y=3)), Box(x=b, y=c))
        check_in_place(box1, Span(x=e, y=3))
        self.assertEqual(box1.expanded_to(Span(x=c, y=6)), Box(x=c, y=b))
        check_in_place(box1, Span(x=c, y=6))
        # Box overload
        self.assertEqual(box1.expanded_to(Box(x=d, y=c)), Box(x=a, y=c))
        check_in_place(box1, Box(x=d, y=c))
        self.assertEqual(box1.expanded_to(Box(x=c, y=d)), Box(x=c, y=a))
        check_in_place(box1, Box(x=c, y=e))
        self.assertEqual(box1.expanded_to(Box(x=e, y=c)), Box(x=b, y=c))
        check_in_place(box1, Box(x=e, y=c))
        self.assertEqual(box1.expanded_to(Box(x=c, y=e)), Box(x=c, y=b))
        check_in_place(box1, Box(x=c, y=d))


if __name__ == "__main__":
    unittest.main()
