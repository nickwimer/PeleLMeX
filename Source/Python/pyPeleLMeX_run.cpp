#include "pyPeleLMeX.H"
#include <AMReX_ParmParse.H>
#include <PeleLMeX.H>
#include <AMReX_ParallelDescriptor.H>

// Function to run PeleLMeX in Evolve mode
void
evolvePeleLMeX()
{
  auto& pelelm = getPeleLMeX();
  pelelm.Evolve();
}

// Function to run PeleLMeX in Evaluate mode
void
evaluatePeleLMeX()
{
  auto& pelelm = getPeleLMeX();
  pelelm.Evaluate();
}