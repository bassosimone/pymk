// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

#include <measurement_kit/ooni.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// Fetch compatibility code required to make this module work
// with MeasurementKit v0.3.x versions
#include "compat-0.3.hpp"

namespace py = pybind11;

PYBIND11_PLUGIN(pybind) {
    py::module m("pybind", "MeasurementKit pybind bindings");

    m.def("set_verbosity", &mk::set_verbosity);
    m.def("increase_verbosity", &mk::increase_verbosity);

    m.def("web_connectivity",
          [](std::string input, std::map<std::string, std::string> settings,
             py::function callback) {
              py::gil_scoped_release release;
              mk::Settings cxx_settings(settings.begin(), settings.end());
              mk::ooni::scriptable::web_connectivity(
                  input, cxx_settings, [=](std::string s) {
                      py::gil_scoped_acquire acquire;
                      callback(s);
                  });
          });

    return m.ptr();
}
