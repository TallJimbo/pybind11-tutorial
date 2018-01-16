#!/usr/bin/env python
"""Test code for the Span class."""
from spanops import Span, Interval
import unittest


class SpanTestCase(unittest.TestCase):

    def test_empty(self):
        a = Span()
        self.assertTrue(a.empty)
        b = Span(y=2, x=Interval(3, 1))
        self.assertTrue(b.empty)

    def test_standard(self):
        a = Span(Interval(3, 5), 2)
        self.assertEqual(a.width, 3)
        self.assertEqual(a.x0, 3)
        self.assertEqual(a.x1, 5)
        self.assertFalse(a.empty)

    def test_intersect(self):
        a = Span(y=1, x=Interval(1, 4))
        b = Span(y=1, x=Interval(3, 6))
        c = Span(y=1, x=Interval(5, 8))
        d = Span(y=0, x=Interval(2, 3))
        self.assertEqual(a & b, Span(y=1, x=Interval(3, 4)))
        self.assertTrue((a & c).empty)
        self.assertTrue((a & d).empty)
        a &= b
        self.assertEqual(a, Span(y=1, x=Interval(3, 4)))
        a &= c
        self.assertTrue(a.empty)
        b &= d
        self.assertTrue(b.empty)

    def test_overlap(self):
        a = Span(x=Interval(2, 6), y=1)
        self.assertFalse(a.overlaps(1, 1))
        self.assertFalse(a.overlaps(3, 0))
        self.assertTrue(a.overlaps(2, 1))
        self.assertTrue(a.overlaps(4, 1))
        self.assertTrue(a.overlaps(6, 1))
        self.assertFalse(a.overlaps(7, 1))
        self.assertFalse(a.overlaps(4, 2))
        self.assertFalse(a.overlaps(Span(x=Interval(-1, 1), y=1)))
        self.assertFalse(a.overlaps(Span(x=Interval(-1, 2), y=0)))
        self.assertTrue(a.overlaps(Span(x=Interval(-1, 2), y=1)))
        self.assertTrue(a.overlaps(Span(x=Interval(3, 4), y=1)))
        self.assertTrue(a.overlaps(Span(x=Interval(5, 7), y=1)))
        self.assertFalse(a.overlaps(Span(x=Interval(5, 7), y=2)))
        self.assertFalse(a.overlaps(Span(x=Interval(7, 9), y=1)))

if __name__ == "__main__":
    unittest.main()
