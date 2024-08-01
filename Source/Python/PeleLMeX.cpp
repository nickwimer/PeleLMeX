
#include "pyPeleLMeX.H"

#include <PeleLMeX.H>

#include <AMReX.H>
#include <AMReX_ParallelDescriptor.H>
#include <AMReX_ParmParse.H>

#include <string>

// namespace pelelmex {
// struct Config {};
// } // namespace pelelmex

void
init_dummy(py::module& m)
{
  m.def("dummy", []() {});

  m.def(
    "amrex_hello", []() { amrex::Print() << "Hello from AMReX" << std::endl; });
}

void
init_pelelmex(py::module& m)
{
  // py::class_<PeleLM>(m, "PeleLM");
  PeleLM pelelmex;
  m.def(
    "read_params", &PeleLM::readParameters, "Read parameters from inputs file");

  // m.def("setup", &PeleLM::Setup, "setup the sim");
}
