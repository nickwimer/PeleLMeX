#include <PeleLMeX.H>
#include <AMReX_ParmParse.H>

// Helper function to query a string and allocate memory for char*destination
void
queryAndAllocateString(
  amrex::ParmParse& pp, const std::string& param_name, char*& dest)
{
  // Query an arbitrary-length string into a temporary std::string
  std::string tmp_str;
  if (pp.query(param_name.c_str(), tmp_str)) {
    // Allocate memory for the string
    size_t len = tmp_str.length();

    // Deallocate any existing memory if not nullptr
    if (dest != nullptr) {
      delete[] dest;
      dest = nullptr;
    }

    // Allocate memory for the new string and copy the data
    dest = new char[len + 1]; // +1 for null terminator
    std::copy(tmp_str.begin(), tmp_str.end(), dest);
    dest[len] = '\0';
  }
}

void
PeleLM::readProbParm() // NOLINT(readability-make-member-function-const)
{
  // Placeholder
  amrex::ParmParse pp("prob");

  pp.query("P_mean", PeleLM::prob_parm->P_mean);
  // pp.query("T_mean", PeleLM::prob_parm->T_mean);

  // Get the initial condition expressions and dictionaries
  // ************************************************************
  queryAndAllocateString(pp, "ic_x_expr", PeleLM::prob_parm->ic_x_expr);
  queryAndAllocateString(pp, "ic_x_dict", PeleLM::prob_parm->ic_x_dict);
  queryAndAllocateString(pp, "ic_y_expr", PeleLM::prob_parm->ic_y_expr);
  queryAndAllocateString(pp, "ic_y_dict", PeleLM::prob_parm->ic_y_dict);
  queryAndAllocateString(pp, "ic_z_expr", PeleLM::prob_parm->ic_z_expr);
  queryAndAllocateString(pp, "ic_z_dict", PeleLM::prob_parm->ic_z_dict);
  queryAndAllocateString(pp, "ic_press_expr", PeleLM::prob_parm->ic_press_expr);
  queryAndAllocateString(pp, "ic_press_dict", PeleLM::prob_parm->ic_press_dict);
  queryAndAllocateString(pp, "ic_temp_expr", PeleLM::prob_parm->ic_temp_expr);
  queryAndAllocateString(pp, "ic_temp_dict", PeleLM::prob_parm->ic_temp_dict);
  queryAndAllocateString(pp, "ic_velx_expr", PeleLM::prob_parm->ic_velx_expr);
  queryAndAllocateString(pp, "ic_velx_dict", PeleLM::prob_parm->ic_velx_dict);
  queryAndAllocateString(pp, "ic_vely_expr", PeleLM::prob_parm->ic_vely_expr);
  queryAndAllocateString(pp, "ic_vely_dict", PeleLM::prob_parm->ic_vely_dict);
  queryAndAllocateString(pp, "ic_velz_expr", PeleLM::prob_parm->ic_velz_expr);
  queryAndAllocateString(pp, "ic_velz_dict", PeleLM::prob_parm->ic_velz_dict);
  queryAndAllocateString(pp, "ic_spec_expr", PeleLM::prob_parm->ic_spec_expr);
  queryAndAllocateString(pp, "ic_spec_dict", PeleLM::prob_parm->ic_spec_dict);
  // ************************************************************

  // Get the boundary condition expressions and dictionaries
  // ************************************************************
  // X-Lo boundary condition expressions and dictionaries
  queryAndAllocateString(
    pp, "bc_xlo_press_expr", PeleLM::prob_parm->bc_xlo_press_expr);
  queryAndAllocateString(
    pp, "bc_xlo_press_dict", PeleLM::prob_parm->bc_xlo_press_dict);
  queryAndAllocateString(
    pp, "bc_xlo_temp_expr", PeleLM::prob_parm->bc_xlo_temp_expr);
  queryAndAllocateString(
    pp, "bc_xlo_temp_dict", PeleLM::prob_parm->bc_xlo_temp_dict);
  queryAndAllocateString(
    pp, "bc_xlo_velx_expr", PeleLM::prob_parm->bc_xlo_velx_expr);
  queryAndAllocateString(
    pp, "bc_xlo_velx_dict", PeleLM::prob_parm->bc_xlo_velx_dict);
  queryAndAllocateString(
    pp, "bc_xlo_vely_expr", PeleLM::prob_parm->bc_xlo_vely_expr);
  queryAndAllocateString(
    pp, "bc_xlo_vely_dict", PeleLM::prob_parm->bc_xlo_vely_dict);
  queryAndAllocateString(
    pp, "bc_xlo_velz_expr", PeleLM::prob_parm->bc_xlo_velz_expr);
  queryAndAllocateString(
    pp, "bc_xlo_velz_dict", PeleLM::prob_parm->bc_xlo_velz_dict);
  queryAndAllocateString(
    pp, "bc_xlo_spec_expr", PeleLM::prob_parm->bc_xlo_spec_expr);
  queryAndAllocateString(
    pp, "bc_xlo_spec_dict", PeleLM::prob_parm->bc_xlo_spec_dict);

  // X-Hi boundary condition expressions and dictionaries
  queryAndAllocateString(
    pp, "bc_xhi_press_expr", PeleLM::prob_parm->bc_xhi_press_expr);
  queryAndAllocateString(
    pp, "bc_xhi_press_dict", PeleLM::prob_parm->bc_xhi_press_dict);
  queryAndAllocateString(
    pp, "bc_xhi_temp_expr", PeleLM::prob_parm->bc_xhi_temp_expr);
  queryAndAllocateString(
    pp, "bc_xhi_temp_dict", PeleLM::prob_parm->bc_xhi_temp_dict);
  queryAndAllocateString(
    pp, "bc_xhi_velx_expr", PeleLM::prob_parm->bc_xhi_velx_expr);
  queryAndAllocateString(
    pp, "bc_xhi_velx_dict", PeleLM::prob_parm->bc_xhi_velx_dict);
  queryAndAllocateString(
    pp, "bc_xhi_vely_expr", PeleLM::prob_parm->bc_xhi_vely_expr);
  queryAndAllocateString(
    pp, "bc_xhi_vely_dict", PeleLM::prob_parm->bc_xhi_vely_dict);
  queryAndAllocateString(
    pp, "bc_xhi_velz_expr", PeleLM::prob_parm->bc_xhi_velz_expr);
  queryAndAllocateString(
    pp, "bc_xhi_velz_dict", PeleLM::prob_parm->bc_xhi_velz_dict);
  queryAndAllocateString(
    pp, "bc_xhi_spec_expr", PeleLM::prob_parm->bc_xhi_spec_expr);
  queryAndAllocateString(
    pp, "bc_xhi_spec_dict", PeleLM::prob_parm->bc_xhi_spec_dict);

  // Y-Lo boundary condition expressions and dictionaries
  queryAndAllocateString(
    pp, "bc_ylo_press_expr", PeleLM::prob_parm->bc_ylo_press_expr);
  queryAndAllocateString(
    pp, "bc_ylo_press_dict", PeleLM::prob_parm->bc_ylo_press_dict);
  queryAndAllocateString(
    pp, "bc_ylo_temp_expr", PeleLM::prob_parm->bc_ylo_temp_expr);
  queryAndAllocateString(
    pp, "bc_ylo_temp_dict", PeleLM::prob_parm->bc_ylo_temp_dict);
  queryAndAllocateString(
    pp, "bc_ylo_velx_expr", PeleLM::prob_parm->bc_ylo_velx_expr);
  queryAndAllocateString(
    pp, "bc_ylo_velx_dict", PeleLM::prob_parm->bc_ylo_velx_dict);
  queryAndAllocateString(
    pp, "bc_ylo_vely_expr", PeleLM::prob_parm->bc_ylo_vely_expr);
  queryAndAllocateString(
    pp, "bc_ylo_vely_dict", PeleLM::prob_parm->bc_ylo_vely_dict);
  queryAndAllocateString(
    pp, "bc_ylo_velz_expr", PeleLM::prob_parm->bc_ylo_velz_expr);
  queryAndAllocateString(
    pp, "bc_ylo_velz_dict", PeleLM::prob_parm->bc_ylo_velz_dict);
  queryAndAllocateString(
    pp, "bc_ylo_spec_expr", PeleLM::prob_parm->bc_ylo_spec_expr);
  queryAndAllocateString(
    pp, "bc_ylo_spec_dict", PeleLM::prob_parm->bc_ylo_spec_dict);

  // Y-Hi boundary condition expressions and dictionaries
  queryAndAllocateString(
    pp, "bc_yhi_press_expr", PeleLM::prob_parm->bc_yhi_press_expr);
  queryAndAllocateString(
    pp, "bc_yhi_press_dict", PeleLM::prob_parm->bc_yhi_press_dict);
  queryAndAllocateString(
    pp, "bc_yhi_temp_expr", PeleLM::prob_parm->bc_yhi_temp_expr);
  queryAndAllocateString(
    pp, "bc_yhi_temp_dict", PeleLM::prob_parm->bc_yhi_temp_dict);
  queryAndAllocateString(
    pp, "bc_yhi_velx_expr", PeleLM::prob_parm->bc_yhi_velx_expr);
  queryAndAllocateString(
    pp, "bc_yhi_velx_dict", PeleLM::prob_parm->bc_yhi_velx_dict);
  queryAndAllocateString(
    pp, "bc_yhi_vely_expr", PeleLM::prob_parm->bc_yhi_vely_expr);
  queryAndAllocateString(
    pp, "bc_yhi_vely_dict", PeleLM::prob_parm->bc_yhi_vely_dict);
  queryAndAllocateString(
    pp, "bc_yhi_velz_expr", PeleLM::prob_parm->bc_yhi_velz_expr);
  queryAndAllocateString(
    pp, "bc_yhi_velz_dict", PeleLM::prob_parm->bc_yhi_velz_dict);
  queryAndAllocateString(
    pp, "bc_yhi_spec_expr", PeleLM::prob_parm->bc_yhi_spec_expr);
  queryAndAllocateString(
    pp, "bc_yhi_spec_dict", PeleLM::prob_parm->bc_yhi_spec_dict);

  // Z-Lo boundary condition expressions and dictionaries
  queryAndAllocateString(
    pp, "bc_zlo_press_expr", PeleLM::prob_parm->bc_zlo_press_expr);
  queryAndAllocateString(
    pp, "bc_zlo_press_dict", PeleLM::prob_parm->bc_zlo_press_dict);
  queryAndAllocateString(
    pp, "bc_zlo_temp_expr", PeleLM::prob_parm->bc_zlo_temp_expr);
  queryAndAllocateString(
    pp, "bc_zlo_temp_dict", PeleLM::prob_parm->bc_zlo_temp_dict);
  queryAndAllocateString(
    pp, "bc_zlo_velx_expr", PeleLM::prob_parm->bc_zlo_velx_expr);
  queryAndAllocateString(
    pp, "bc_zlo_velx_dict", PeleLM::prob_parm->bc_zlo_velx_dict);
  queryAndAllocateString(
    pp, "bc_zlo_vely_expr", PeleLM::prob_parm->bc_zlo_vely_expr);
  queryAndAllocateString(
    pp, "bc_zlo_vely_dict", PeleLM::prob_parm->bc_zlo_vely_dict);
  queryAndAllocateString(
    pp, "bc_zlo_velz_expr", PeleLM::prob_parm->bc_zlo_velz_expr);
  queryAndAllocateString(
    pp, "bc_zlo_velz_dict", PeleLM::prob_parm->bc_zlo_velz_dict);
  queryAndAllocateString(
    pp, "bc_zlo_spec_expr", PeleLM::prob_parm->bc_zlo_spec_expr);
  queryAndAllocateString(
    pp, "bc_zlo_spec_dict", PeleLM::prob_parm->bc_zlo_spec_dict);

  // Z-Hi boundary condition expressions and dictionaries
  queryAndAllocateString(
    pp, "bc_zhi_press_expr", PeleLM::prob_parm->bc_zhi_press_expr);
  queryAndAllocateString(
    pp, "bc_zhi_press_dict", PeleLM::prob_parm->bc_zhi_press_dict);
  queryAndAllocateString(
    pp, "bc_zhi_temp_expr", PeleLM::prob_parm->bc_zhi_temp_expr);
  queryAndAllocateString(
    pp, "bc_zhi_temp_dict", PeleLM::prob_parm->bc_zhi_temp_dict);
  queryAndAllocateString(
    pp, "bc_zhi_velx_expr", PeleLM::prob_parm->bc_zhi_velx_expr);
  queryAndAllocateString(
    pp, "bc_zhi_velx_dict", PeleLM::prob_parm->bc_zhi_velx_dict);
  queryAndAllocateString(
    pp, "bc_zhi_vely_expr", PeleLM::prob_parm->bc_zhi_vely_expr);
  queryAndAllocateString(
    pp, "bc_zhi_vely_dict", PeleLM::prob_parm->bc_zhi_vely_dict);
  queryAndAllocateString(
    pp, "bc_zhi_velz_expr", PeleLM::prob_parm->bc_zhi_velz_expr);
  queryAndAllocateString(
    pp, "bc_zhi_velz_dict", PeleLM::prob_parm->bc_zhi_velz_dict);
  queryAndAllocateString(
    pp, "bc_zhi_spec_expr", PeleLM::prob_parm->bc_zhi_spec_expr);
  queryAndAllocateString(
    pp, "bc_zhi_spec_dict", PeleLM::prob_parm->bc_zhi_spec_dict);
  // ************************************************************
}
