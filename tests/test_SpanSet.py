#!/usr/bin/env python
"""Test code for the SpanSet class."""
from spanops import SpanSet, Box, Span, Interval
import unittest
import numpy as np


class SpanSetTestCase(unittest.TestCase):

    def test_empty(self):
        a = SpanSet()
        self.assertTrue(a.empty)
        self.assertEqual(len(a), 0)

    def test_box(self):
        box = Box(x=Interval(min=1, max=5), y=Interval(min=7, max=9))
        s = SpanSet(box)
        self.assertEqual(len(s), box.height)
        self.assertEqual(list(s), [Span(x=box.x, y=y) for y in range(box.y0, box.y1 + 1)])
        e = SpanSet()
        self.assertEqual((s | e), s)
        self.assertEqual((s & e), e)

    def test_disjoint(self):
        box1 = Box(x=Interval(min=1, max=5), y=Interval(min=7, max=9))
        box2 = Box(x=Interval(min=7, max=9), y=Interval(min=6, max=8))
        s1 = SpanSet(box1)
        s2 = SpanSet(box2)
        s = s1 | s2
        for span in s:
            self.assertTrue(box1.overlaps(span) or box2.overlaps(span))
        self.assertEqual(s & s1, s1)
        self.assertEqual(s & s2, s2)
        s1a, s2a = s.split()
        self.assertTrue((s1 == s1a and s2 == s2a) or (s1 == s2a and s2 == s1a))

    def test_overlapping(self):
        box1 = Box(x=Interval(min=1, max=5), y=Interval(min=7, max=9))
        box2 = Box(x=Interval(min=3, max=7), y=Interval(min=6, max=8))
        s1 = SpanSet(box1)
        s2 = SpanSet(box2)
        s = s1 | s2
        self.assertEqual(s1 & s2, SpanSet(box1 & box2))
        self.assertEqual(s & s2, s2)
        self.assertEqual(s & s1, s1)

    def test_image_ops(self):
        rng = np.random.RandomState(50)
        original = rng.randn(10, 10) > 0.3
        x0, y0 = (-2, 5)
        all_spans = SpanSet.extract(original, True, x0=x0, y0=y0)
        for span in all_spans:
            row = original[span.y - y0, span.x0 - x0: span.x1 - x0 + 1]
            self.assertTrue(row.all())
        im_all = np.zeros((10, 10), dtype=bool)
        all_spans.insert(im_all, True, x0=x0, y0=y0)
        np.testing.assert_equal(original, im_all)
        im_split = np.zeros((10, 10), dtype=int)
        for n, child in enumerate(all_spans.split()):
            for span in child:
                row = original[span.y - y0, span.x0 - x0: span.x1 - x0 + 1]
                self.assertTrue(row.all())
            child.insert(im_split, n + 1, x0=x0, y0=y0)
        np.testing.assert_equal(original, im_split != 0)


if __name__ == "__main__":
    unittest.main()
