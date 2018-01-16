#include "pybind11/pybind11.h"
#include "pybind11/operators.h"
#include "pybind11/stl.h"
#include "spanops.h"

namespace py = pybind11;

using namespace pybind11::literals;

namespace spanops {
namespace {

template <typename Class, typename ...Args>
void wrap_common(py::class_<Class, Args...> & cls) {
    cls.def(py::self & py::self);
    cls.def(py::self &= py::self);
    cls.def(py::self == py::self);
    cls.def(py::self != py::self);
    cls.def_property_readonly("empty", &Class::empty);
}

void declareInterval(py::module & mod) {
    py::class_<Interval> cls(mod, "Interval");
    cls.def(py::init<>());
    cls.def(py::init<int>());
    cls.def(py::init<int, int>(), "min"_a, "max"_a);
    cls.def_property_readonly("min", &Interval::min);
    cls.def_property_readonly("max", &Interval::max);
    cls.def_property_readonly("length", &Interval::length);
    cls.def("expand_to", (void (Interval::*)(int))&Interval::expand_to);
    cls.def("expanded_to", (Interval (Interval::*)(int) const)&Interval::expanded_to);
    cls.def("expand_to", (void (Interval::*)(Interval const &))&Interval::expand_to);
    cls.def("expanded_to", (Interval (Interval::*)(Interval const &) const)&Interval::expanded_to);
    cls.def("overlaps", (bool (Interval::*)(int) const)&Interval::overlaps);
    cls.def("overlaps", (bool (Interval::*)(Interval const &) const)&Interval::overlaps);
    cls.def(
        "__repr__",
        [](Interval const & self) {
            if (self.empty()) {
                return py::str("Interval()");
            }
            return py::str("Interval(min={}, max={})").format(self.min(), self.max());
        }
    );
    cls.def(
        "__str__",
        [](Interval const & self) {
            if (self.empty()) {
                return py::str("()");
            }
            return py::str("{}..{}").format(self.min(), self.max());
        }
    );
    wrap_common(cls);
}

void declareSpan(py::module & mod) {
    py::class_<Span> cls(mod, "Span");
    cls
        .def(py::init<>())
        .def(py::init<Interval, int>(), "x"_a, "y"_a)
        .def_property_readonly("x0", &Span::x0)
        .def_property_readonly("x1", &Span::x1)
        .def_property_readonly("x", &Span::x)
        .def_property_readonly("y", &Span::y)
        .def_property_readonly("width", &Span::width)
        .def("overlaps", (bool (Span::*)(int, int) const)&Span::overlaps, "x"_a, "y"_a)
        .def("overlaps", (bool (Span::*)(Span const &) const)&Span::overlaps)
        .def(
            "__repr__",
            [](Span const & self) {
                if (self.empty()) {
                    return py::str("Span()");
                }
                return py::str("Span(x={!r}, y={!r})").format(self.x(), self.y());
            }
        )
        .def(
            "__str__",
            [](Span const & self) {
                if (self.empty()) {
                    return py::str("()");
                }
                return py::str("(x={!s}, y={!s})").format(self.x(), self.y());
            }
        );
    wrap_common(cls);
}

void declareBox(py::module & mod) {
    py::class_<Box> cls(mod, "Box");
    cls
        .def(py::init<>())
        .def(py::init<Span>(), "span"_a)
        .def(py::init<Interval, Interval>(), "x"_a, "y"_a)
        .def_property_readonly("x", &Box::x)
        .def_property_readonly("y", &Box::y)
        .def_property_readonly("x0", &Box::x0)
        .def_property_readonly("x1", &Box::x1)
        .def_property_readonly("y0", &Box::y0)
        .def_property_readonly("y1", &Box::y1)
        .def_property_readonly("width", &Box::width)
        .def_property_readonly("height", &Box::height)
        .def_property_readonly("area", &Box::area)
        .def("expand_to", (void (Box::*)(int, int))&Box::expand_to, "x"_a, "y"_a)
        .def("expanded_to", (Box (Box::*)(int, int) const)&Box::expanded_to, "x"_a, "y"_a)
        .def("expand_to", (void (Box::*)(Span const &))&Box::expand_to)
        .def("expanded_to", (Box (Box::*)(Span const &) const)&Box::expanded_to)
        .def("expand_to", (void (Box::*)(Box const &))&Box::expand_to)
        .def("expanded_to", (Box (Box::*)(Box const &) const)&Box::expanded_to)
        .def("overlaps", (bool (Box::*)(int, int) const)&Box::overlaps, "x"_a, "y"_a)
        .def("overlaps", (bool (Box::*)(Span const &) const)&Box::overlaps)
        .def("overlaps", (bool (Box::*)(Box const &) const)&Box::overlaps)
        .def(
            "__repr__",
            [](Box const & self) {
                if (self.empty()) {
                    return py::str("Box()");
                }
                return py::str("Box(x={!r}, y={!r})").format(self.x(), self.y());
            }
        )
        .def(
            "__str__",
            [](Box const & self) {
                if (self.empty()) {
                    return py::str("()");
                }
                return py::str("(x={!s}, y={!s})").format(self.x(), self.y());
            }
        );
    wrap_common(cls);
}

void declareSpanSet(py::module & mod) {
    py::class_<SpanSet> cls(mod, "SpanSet");
    cls
        .def(py::init<>())
        .def(py::init<Box>())
        .def_property_readonly("area", &SpanSet::area)
        .def_property_readonly("bbox", &SpanSet::bbox)
        .def("__len__", &SpanSet::size)
        .def(
             "__iter__",
             [](SpanSet const & self) {
                return py::make_iterator(self.begin(), self.end());
             }
        )
        .def(py::self | py::self)
        .def(py::self |= py::self)
        .def("split", &SpanSet::split)
    ;
    wrap_common(cls);
}


} // anonymous
} // spanops


PYBIND11_MODULE(spanops, m) {
    spanops::declareInterval(m);
    spanops::declareSpan(m);
    spanops::declareBox(m);
    spanops::declareSpanSet(m);
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
