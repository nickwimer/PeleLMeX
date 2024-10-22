#ifndef PELELMEX_PYPELELMEX_H
#define PELELMEX_PYPELELMEX_H

#include <PeleLMeX.H>

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <list>
#include <string>

namespace py = pybind11;

// Function to initialize AMReX and pass in input file
void initAmrex(const std::string& input_file);

// Function to finalize AMReX
void finalizeAmrex();

// Function to create the PeleLM instance
void createPeleLMeX();

// Function to initialize the PeleLM case
void initPeleLMeX();

// Function to access the PeleLM instance
PeleLM& getPeleLMeX();

// Function to finalize (delete) the PeleLM instance
void finalizePeleLMeX();

// Function to call the setup function on the PeleLM instance
void setupPeleLMeX();

// Function to determine PeleLM run mode
std::string getRunMode();

// Update ParmParse with a Python dictionary
void updateParmParse(const std::string& prefix, const py::dict& params);

// Function to run PeleLM in Evolve mode
void evolvePeleLMeX();

// Function to run PeleLM in Evaluate mode
void evaluatePeleLMeX();

// // Function to update the prob_parm dictionary with new values
// void updateProbParmDict(const py::dict& params);

// // Get a parameter value from ParmParse
// py::object
// getParmParseValue(const std::string& prefix, const std::string& name);

#endif // PELELMEX_PYPELELMEX_H
