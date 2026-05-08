#include "pyPeleLMeX.H"

#include <PeleLMeX_Index.H>
#include <Python.h>
#include <AMReX_ParallelDescriptor.H>

#define PYPELELMEX_MODULE_NAME pelelmex_pybind

namespace py = pybind11;

// forward declarations of exposed classes
void init_pelelmex(py::module&);
void run_pelelmex(py::module&);
void pp_pelelmex(py::module&);

PYBIND11_MODULE(PYPELELMEX_MODULE_NAME, m)
{
#if (AMREX_SPACEDIM == 3)
  constexpr auto amrex_module_name = "amrex.space3d";
#elif (AMREX_SPACEDIM == 2)
  constexpr auto amrex_module_name = "amrex.space2d";
#else
#error "Unsupported AMREX_SPACEDIM for pyPeleLMeX."
#endif

  auto amr = py::module::import(amrex_module_name);

  m.doc() = "test module for PeleLMeX";

  init_pelelmex(m);
  run_pelelmex(m);
  pp_pelelmex(m);

  // m.def("get_mpi_rank", []() {
  //   int rank;
  //   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //   return rank;
  // });

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
#ifdef FIRSTAUX
  m.attr("FIRSTAUX") = FIRSTAUX;
#endif
  m.attr("NVAR") = NVAR;
  m.attr("NUM_SPECIES") = NUM_SPECIES;
}