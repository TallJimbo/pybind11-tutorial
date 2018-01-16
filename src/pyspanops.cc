#include "pybind11/pybind11.h"
#include "spanops.h"

namespace py = pybind11;

using namespace pybind11::literals;

namespace spanops {
namespace {

template <typename Class, typename ...Args>
void wrap_common(py::class_<Class, Args...> & cls) {
    cls.def("__and__", &Class::operator&, py::is_operator());
    cls.def("__iand__", &Class::operator&, py::is_operator());
    cls.def("__eq__", &Class::operator==, py::is_operator());
    cls.def("__ne__", &Class::operator!=, py::is_operator());
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

} // anonymous
} // spanops


PYBIND11_MODULE(spanops, m) {
    spanops::declareInterval(m);
    spanops::declareSpan(m);
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
