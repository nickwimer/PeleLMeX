#include "pyPeleLMeX.H"
#include <AMReX_ParmParse.H>
#include <PeleLMeX.H>
#include <AMReX_Parser.H>

#include <pelelmex_prob.H>

#include <PeleLMeX_DeriveFunc.H>
#include <PeleLMeX_BPatch.H>
#include <PelePhysics.H>
#include <AMReX_buildInfo.H>

using namespace amrex;

// Fuction to get a parameter value from ParmParse
py::object
getParmParseValue(const std::string& prefix, const std::string& name)
{
  amrex::ParmParse pp(prefix.c_str());

  if (pp.countval(name.c_str()) == 0) {
    throw std::runtime_error("Parameter not found in ParmParse");
  }

  std::string str_val;
  int int_val;
  amrex::Real real_val;

  if (pp.query(name.c_str(), str_val)) {
    return py::cast(str_val);
  } else if (pp.query(name.c_str(), int_val)) {
    return py::cast(int_val);
  } else if (pp.query(name.c_str(), real_val)) {
    return py::cast(real_val);
  } else {
    throw std::runtime_error("Parameter type not recognized in ParmParse");
  }
}

// // Helper function to update char* memory safely
// void
// updateCharPointer(char*& dest, const std::string& new_value)
// {
//   // Deallocate any existing memory if not nullptr
//   if (dest != nullptr) {
//     delete[] dest;
//     dest = nullptr;
//   }

//   // Allocate memory for the new string and copy the data
//   size_t len = new_value.length();
//   dest = new char[len + 1]; // +1 for null terminator
//   std::copy(new_value.begin(), new_value.end(), dest);
//   dest[len] = '\0'; // Null-terminate the string
// }

// void
// updateProbParmDict(const py::dict& params)
// {
//   // Get the PeleLM instance
//   auto& pelelm = getPeleLMeX();

//   // Get the prob_parm struct
//   auto& prob_parm = pelelm.prob_parm;

//   // Loop over the dictionary and update the prob_parm values
//   for (auto item : params) {
//     std::string key = py::str(item.first);
//     py::object value = py::reinterpret_borrow<py::object>(item.second);

//     if (key == "P_mean") {
//       if (py::isinstance<py::float_>(value)) {
//         prob_parm->P_mean = value.cast<amrex::Real>();
//       }

//     } else if (key == "press_expr") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->press_expr, val);
//       }
//     } else if (key == "press_dict") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->press_dict, val);
//       }
//     } else if (key == "temp_expr") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->temp_expr, val);
//       }
//     } else if (key == "temp_dict") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->temp_dict, val);
//       }
//     } else if (key == "velx_expr") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->velx_expr, val);
//       }
//     } else if (key == "vely_expr") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->vely_expr, val);
//       }
//     } else if (key == "velz_expr") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->velz_expr, val);
//       }
//     } else if (key == "velx_dict") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->velx_dict, val);
//       }
//     } else if (key == "vely_dict") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->vely_dict, val);
//       }
//     } else if (key == "velz_dict") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->velz_dict, val);
//       }
//     } else if (key == "spec_expr") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->spec_expr, val);
//       }
//     } else if (key == "spec_dict") {
//       if (py::isinstance<py::str>(value)) {
//         std::string val = value.cast<std::string>();
//         updateCharPointer(prob_parm->spec_dict, val);
//       }
//     }
//   }

//   //   auto& prob_parm_d =
//   //   (ProbParm*)amrex::The_Arena()->alloc(sizeof(ProbParm));
//   //   prob_parm_d = (ProbParm*)The_Arena()->alloc(sizeof(ProbParm));
//   Gpu::copy(Gpu::hostToDevice, prob_parm, prob_parm + 1, pelelm.prob_parm_d);

//   //   amrex::Gpu::copy(
//   //     amrex::Gpu::hostToDevice, prob_parm, prob_parm + 1,
//   //     pelelm.prob_parm_d);
// }