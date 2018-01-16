#include "pybind11/pybind11.h"
#include "spanops.h"

namespace py = pybind11;

using namespace pybind11::literals;

namespace spanops {
namespace {

void declareInterval(py::module & mod) {
    py::class_<Interval> cls(mod, "Interval");
    cls.def(py::init<>());
    cls.def(py::init<int>());
    cls.def(py::init<int, int>(), "min"_a, "max"_a);
    cls.def_property_readonly("min", &Interval::min);
    cls.def_property_readonly("max", &Interval::max);
    cls.def_property_readonly("length", &Interval::length);
    cls.def_property_readonly("empty", &Interval::empty);
    cls.def("__and__", &Interval::operator&, py::is_operator());
    cls.def("__iand__", &Interval::operator&=, py::is_operator());
    cls.def("__eq__", &Interval::operator==, py::is_operator());
    cls.def("__ne__", &Interval::operator!=, py::is_operator());
    cls.def("expand_to", (void (Interval::*)(int))&Interval::expand_to);
    cls.def("expanded_to", (Interval (Interval::*)(int) const)&Interval::expanded_to);
    cls.def("expand_to", (void (Interval::*)(Interval const &))&Interval::expand_to);
    cls.def("expanded_to", (Interval (Interval::*)(Interval const &) const)&Interval::expanded_to);
    cls.def("overlaps", (bool (Interval::*)(int) const)&Interval::overlaps);
    cls.def("overlaps", (bool (Interval::*)(Interval const &) const)&Interval::overlaps);
}

} // anonymous
} // spanops


PYBIND11_MODULE(spanops, m) {
    spanops::declareInterval(m);
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
