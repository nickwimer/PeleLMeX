#include "pyPeleLMeX.H"
#include <AMReX_ParmParse.H>
#include <PeleLMeX.H>
#include <AMReX_Parser.H>
#include <AMReX_ParallelDescriptor.H>
#include <AMReX_AmrMesh.H>
#include <AMReX_Amr.H>
#include <AMReX_MultiFabUtil.H>

#include <pelelmex_prob.H>

#include <stdexcept>
#include <vector>

static bool amrex_initialized = false;
static std::unique_ptr<PeleLM> pelelm_instance = nullptr;
// static std::unique_ptr<amrex::AmrMesh> amr_mesh_instance = nullptr;
// std::shared_ptr<amrex::AmrMesh> amr_mesh_instance;

// Function to initialize AMReX and pass in input file
// int
MPI_Comm
initAmrex(const std::string& input_file, MPI_Comm mpi_comm)
{
  // Disallow re-initialization without explicit finalization.
  if (amrex_initialized) {
    throw std::runtime_error(
      "AMReX already initialized. Call finalize_amrex() before reinitializing.");
  }

  if (pelelm_instance) {
    throw std::runtime_error(
      "PeleLM instance already exists. Call finalize_pelelmex() before reinitializing AMReX.");
  }

  int argc = 0;
  char** argv = nullptr;

  // Constructing arguments to mimic command line args
  std::vector<std::string> args;
  args.push_back("amrex"); // Program name (typically argv[0])

  if (!input_file.empty()) {
    args.push_back(input_file); // The input file argument
  }

  argc = static_cast<int>(args.size());
  // argv must be null-terminated for MPI runtimes that iterate until nullptr.
  std::vector<char*> cstr_args(static_cast<std::size_t>(argc) + 1, nullptr);
  for (size_t i = 0; i < args.size(); ++i) {
    cstr_args[i] = &args[i][0];
  }
  cstr_args[static_cast<std::size_t>(argc)] = nullptr;
  argv = cstr_args.data();

  // amr_mesh_instance = std::make_shared<amrex::AmrMesh>();
  // real_box, max_level, n_cell, coord, ref_ratio, is_per);

  // Initialize AMReX with the constructed arguments.
#ifdef AMREX_USE_MPI
  if (mpi_comm != MPI_COMM_WORLD) {
    throw std::runtime_error(
      "Only MPI_COMM_WORLD is currently supported by pyPeleLMeX initialization.");
  }
  amrex::Initialize(argc, argv, true, mpi_comm);
#else
  (void)mpi_comm;
  amrex::Initialize(argc, argv, true);
#endif

  // amrex::Vector<amrex::Geometry> geom;
  // amrex::Vector<amrex::BoxArray> grids;

  // geom.resize(2);
  // grids.resize(2);

  // amrex::Amr::Initialize();
  // amr_mesh_instance = std::make_shared<amrex::AmrMesh>();
  // amrex::AmrMesh();
  // amrex::Geometry::Setup();
  // int max_level_in = -1;
  // amrex::Vector<int> n_cell_in(AMREX_SPACEDIM, -1);
  // amrex::AmrMesh::InitAmrMesh(max_level_in, n_cell_in);
  // amrex::ParmParse pp("amr");
  // pp.query("max_level", pelelmex::max_level);

  // amr_mesh_instance = std::make_unique<amrex::AmrMesh>();

  // amr_mesh_instance->InitAmrMesh(-1, amrex::Vector<int>(AMREX_SPACEDIM, -1));

  amrex_initialized = true;

  // return amrex::ParallelDescriptor::MyProc();
  return amrex::ParallelDescriptor::Communicator();
  // return mpi_comm;
  // return 0;
}

// Function to finalize AMReX
void
finalizeAmrex()
{
  if (amrex_initialized) {
    // Ensure PeleLM is deleted before tearing down AMReX internals.
    pelelm_instance.reset();
    // amrex::ParmParse::Finalize();
    // amrex::ParallelDescriptor::EndParallel();
    amrex::Finalize();
    amrex_initialized = false;
    // amrex::ParmParse::Finalize();
    // amrex::ParallelDescriptor::EndParallel();
  }
}

// Function to create the PeleLM instance
void
createPeleLMeX()
{
  if (!amrex_initialized) {
    throw std::runtime_error(
      "AMReX must be initialized before creating PeleLM instance.");
  }
  if (!pelelm_instance) {
    try {
      pelelm_instance = std::make_unique<PeleLM>();
    } catch (const std::exception& e) {
      throw std::runtime_error(
        "Failed to create PeleLM instance: " + std::string(e.what()));
    }
  } else {
    throw std::runtime_error(
      "PeleLM instance already created. Use get_pelelmex().");
  }
}

// Function to access the PeleLM instance
PeleLM&
getPeleLMeX()
{
  if (!pelelm_instance) {
    throw std::runtime_error(
      "PeleLM instance not created. Call initialize_pelelmex() first.");
  }
  return *pelelm_instance;
}

// Function to finalize (delete) the PeleLM instance
void
finalizePeleLMeX()
{
  pelelm_instance.reset(); // This will delete the instance
  // pelelm_instance = nullptr;
}

// Function to call the setup function on the PeleLM instance
void
setupPeleLMeX()
{
  if (pelelm_instance) {
    pelelm_instance->Setup();
  } else {
    throw std::runtime_error(
      "PeleLM instance not created. Call create_pelelmex() first.");
  }
}

// Function to initialize the PeleLMeX case
void
initPeleLMeX()
{
  if (pelelm_instance) {
    // amrex::ParmParse pp("amr");
    // pp.query("max_level", pelelm_instance->max_level);

    pelelm_instance->Init();
  } else {
    throw std::runtime_error(
      "PeleLM instance not created. Call create_pelelmex() first.");
  }
}

// Function to determine PeleLM run mode
std::string
getRunMode()
{
  if (pelelm_instance) {
    return pelelm_instance->runMode();
  } else {
    throw std::runtime_error(
      "PeleLM instance not created. Call create_pelelmex() first.");
  }
}
