#include "pyPeleLMeX.H"

#include <PeleLMeX_Index.H>
#include <Python.h>

#define PYPELELMEX_MODULE_NAME pelelmex_pybind

namespace py = pybind11;

// forward declarations of exposed classes
void init_pelelmex(py::module&);
void run_pelelmex(py::module&);
void pp_pelelmex(py::module&);

PYBIND11_MODULE(PYPELELMEX_MODULE_NAME, m)
{
  auto amr = py::module::import("amrex.space3d");

  m.doc() = "test module for PeleLMeX";

  init_pelelmex(m);
  run_pelelmex(m);
  pp_pelelmex(m);

  // expose our amrex module
  m.attr("amr") = amr;

  // Expose constants to Python
  m.attr("VELX") = VELX;
  m.attr("VELY") = VELY;
  m.attr("VELZ") = VELZ;
  m.attr("DENSITY") = DENSITY;
  m.attr("FIRSTSPEC") = FIRSTSPEC;
  m.attr("RHOH") = RHOH;
  m.attr("TEMP") = TEMP;
  m.attr("RHORT") = RHORT;
  m.attr("FIRSTAUX") = FIRSTAUX;
  m.attr("NVAR") = NVAR;
  m.attr("NUM_SPECIES") = NUM_SPECIES;
}