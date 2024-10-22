#include "pyPeleLMeX.H"
#include <AMReX_ParmParse.H>
#include <PeleLMeX.H>
#include <AMReX_Parser.H>

#include <pelelmex_prob.H>

static bool amrex_initialized = false;
static std::unique_ptr<PeleLM> pelelm_instance = nullptr;

// Function to initialize AMReX and pass in input file
void
initAmrex(const std::string& input_file)
{
  if (!amrex_initialized) {
    int argc = 0;
    char** argv = nullptr;

    // Constructing arguments to mimic command line args
    std::vector<std::string> args;
    args.push_back("amrex"); // Program name (typically argv[0])

    if (!input_file.empty()) {
      args.push_back(input_file); // The input file argument
    }

    argc = args.size();
    std::vector<char*> cstr_args(argc);
    for (size_t i = 0; i < args.size(); ++i) {
      cstr_args[i] = &args[i][0];
    }
    argv = cstr_args.data();

    // Initialize AMReX with the constructed arguments
    amrex::Initialize(argc, argv);
    amrex_initialized = true;
  } else {
    throw std::runtime_error("AMReX is already initialized.");
  }
}

// Function to finalize AMReX
void
finalizeAmrex()
{
  if (amrex_initialized) {
    amrex::Finalize();
    amrex_initialized = false;
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
