#include "pyPeleLMeX.H"

// #include <PeleLMeX.H>

#define PYPELELMEX_MODULE_NAME pelelmex_pybind

// forward declarations of exposed classes
void init_dummy(py::module&);
void init_pelelmex(py::module&);

PYBIND11_MODULE(PYPELELMEX_MODULE_NAME, m)
{
  auto amr = py::module::import("amrex.space3d");

  m.doc() = "test module for PeleLMeX";

  init_dummy(m);
  init_pelelmex(m);

  // expose our amrex module
  m.attr("amr") = amr;
}