#!/usr/bin/env python
"""Test code for the Interval class."""
from spanops import Interval
import unittest


class IntervalTestCase(unittest.TestCase):

    def test_empty(self):
        a = Interval()
        self.assertTrue(a.empty)
        b = Interval(3, 1)
        self.assertTrue(b.empty)
        self.assertEqual(a, b)

    def test_single(self):
        a = Interval(5)
        self.assertEqual(a.length, 1)
        self.assertEqual(a.min, 5)
        self.assertEqual(a.max, 5)
        self.assertFalse(a.empty)
        self.assertEqual(a, Interval(5))

    def test_standard(self):
        a = Interval(3, 5)
        self.assertEqual(a.length, 3)
        self.assertEqual(a.min, 3)
        self.assertEqual(a.max, 5)
        self.assertFalse(a.empty)
        self.assertEqual(a, Interval(max=5, min=3))

    def test_intersect(self):
        a = Interval(1, 4)
        b = Interval(3, 6)
        c = Interval(5, 8)
        self.assertEqual(a & b, Interval(3, 4))
        self.assertTrue((a & c).empty)
        a &= b
        self.assertEqual(a, Interval(3, 4))
        a &= c
        self.assertTrue(a.empty)

    def test_expand(self):
        a = Interval(-1, 7)
        self.assertEqual(a, Interval(-1, 7))
        c = a.expanded_to(9)
        self.assertEqual(c, Interval(-1, 9))
        d = a.expanded_to(Interval(-3, -2))
        self.assertEqual(d, Interval(-3, 7))
        e = a.expanded_to(Interval())
        self.assertEqual(a, e)

    def test_overlap(self):
        a = Interval(2, 6)
        self.assertFalse(a.overlaps(1))
        self.assertTrue(a.overlaps(2))
        self.assertTrue(a.overlaps(4))
        self.assertTrue(a.overlaps(6))
        self.assertFalse(a.overlaps(7))
        self.assertFalse(a.overlaps(Interval(-1, 1)))
        self.assertTrue(a.overlaps(Interval(-1, 2)))
        self.assertTrue(a.overlaps(Interval(3, 4)))
        self.assertTrue(a.overlaps(Interval(5, 7)))
        self.assertFalse(a.overlaps(Interval(7, 9)))

if __name__ == "__main__":
    unittest.main()
