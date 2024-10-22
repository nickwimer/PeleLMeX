
#include "pyPeleLMeX.H"

#include <PeleLMeX.H>

#include <AMReX.H>
#include <AMReX_ParallelDescriptor.H>
#include <AMReX_ParmParse.H>

#include <string>
#include <memory>

using namespace amrex;

// Routines for initializing, setting up, and finalizing AMReX and PeleLM
void
init_pelelmex(py::module& m)
{
  // Initialize AMReX with input file
  m.def(
    "initialize_amrex", &initAmrex, py::arg("input_file") = "",
    "Initialize AMReX with an optional input file.");
  // Finalize AMReX
  m.def("finalize_amrex", &finalizeAmrex, "Finalize AMReX library.");
  // Create PeleLM instance
  m.def("create_pelelmex", &createPeleLMeX, "Create the PeleLM instance.");
  // Initialize the PeleLMeX Case
  m.def("initialize_pelelmex", &initPeleLMeX, "Initialize the PeleLM case.");
  // Get the PeleLM instance
  m.def(
    "get_pelelmex", []() -> PeleLM& { return getPeleLMeX(); },
    py::return_value_policy::reference, "Get the PeleLM instance.");
  // Setup the PeleLM instance
  m.def("setup_pelelmex", &setupPeleLMeX, "Setup the PeleLM instance.");
  // Determine PeleLM run mode
  m.def("get_run_mode", &getRunMode, "Get the PeleLM run mode.");
  // Finalize PeleLM instance
  m.def(
    "finalize_pelelmex", &finalizePeleLMeX, "Finalize the PeleLM instance.");
  // Initialize Sundials with optional thread count
  m.def(
    "initialize_sundials", &amrex::sundials::Initialize,
    "Initialize Sundials with optional thread count",
    py::arg("nthreads") = amrex::OpenMP::get_max_threads());
  // Finalize Sundials
  m.def("finalize_sundials", &amrex::sundials::Finalize, "Finalize Sundials.");
}

void
run_pelelmex(py::module& m)
{
  // Function to run PeleLMeX in evolve mode
  m.def("evolve_pelelmex", &evolvePeleLMeX, "Run PeleLM in evolve mode.");
  // Function to run PeleLMeX in evaluate mode
  m.def("evaluate_pelelmex", &evaluatePeleLMeX, "Run PeleLM in evaluate mode.");
}

void
pp_pelelmex(py::module& m)
{
  // Update the prob_parm dictionary with new values
  // m.def(
  //   "update_prob_parm_dict", &updateProbParmDict,
  //   "Update prob_parm dictionary with new values", py::arg("params"));
  // // Get a parameter value from ParmParse
  // m.def(
  //   "get_parm_parse_value", &getParmParseValue,
  //   "Get value from ParmParse object", py::arg("prefix"), py::arg("name"));
}
