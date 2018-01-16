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
