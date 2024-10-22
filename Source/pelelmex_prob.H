#ifndef PELELM_PROB_H
#define PELELM_PROB_H

#include <AMReX_Geometry.H>
#include <AMReX_FArrayBox.H>
#include <AMReX_SPACE.H>
#include <AMReX_Parser.H>

#include <PeleLMeX_Index.H>
#include <pelelmex_prob_parm.H>
#include <PMFData.H>
#include <PelePhysics.H>

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void
pelelmex_initdata(
  int i,
  int j,
  int k,
  int /*is_incompressible*/,
  amrex::Array4<amrex::Real> const& state,
  amrex::Array4<amrex::Real> const& /*aux*/,
  amrex::GeometryData const& geomdata,
  ProbParm const& prob_parm,
  pele::physics::PMF::PmfData::DataContainer const* /*pmf_data*/)
{
  // Placeholder
  const amrex::Real* prob_lo = geomdata.ProbLo();
  const amrex::Real* prob_hi = geomdata.ProbHi();
  const amrex::Real* dx = geomdata.CellSize();

  AMREX_D_TERM(const amrex::Real x = prob_lo[0] + (i + 0.5) * dx[0];
               , const amrex::Real y = prob_lo[1] + (j + 0.5) * dx[1];
               , const amrex::Real z = prob_lo[2] + (k + 0.5) * dx[2];);

  // Get the coordinate expressions, replace placeholders, and create parsers
  std::string x_expr = prob_parm.ic_x_expr ? prob_parm.ic_x_expr : "0.0";
  x_expr = replacePlaceholders(x_expr, {{"%EQ%", "="}});
  amrex::Parser parser_x(x_expr);
  std::string y_expr = prob_parm.ic_y_expr ? prob_parm.ic_y_expr : "0.0";
  y_expr = replacePlaceholders(y_expr, {{"%EQ%", "="}});
  amrex::Parser parser_y(y_expr);
  std::string z_expr = prob_parm.ic_z_expr ? prob_parm.ic_z_expr : "0.0";
  z_expr = replacePlaceholders(z_expr, {{"%EQ%", "="}});
  amrex::Parser parser_z(z_expr);

  // Get the pressure expression, replace placeholders, and create a parser
  std::string press_expr =
    prob_parm.ic_press_expr ? prob_parm.ic_press_expr : "101325.0";
  press_expr = replacePlaceholders(press_expr, {{"%EQ%", "="}});
  amrex::Parser parser_press(press_expr);

  // Get the temperature expression, replace placeholders, and create a parser
  std::string temp_expr =
    prob_parm.ic_temp_expr ? prob_parm.ic_temp_expr : "300.0";
  temp_expr = replacePlaceholders(temp_expr, {{"%EQ%", "="}});
  amrex::Parser parser_temp(temp_expr);

  // Get the species expression, replace placeholders, and create parser
  std::string spec_expr = prob_parm.ic_spec_expr ? prob_parm.ic_spec_expr : "0";
  spec_expr = replacePlaceholders(spec_expr, {{"%EQ%", "="}});
  amrex::Parser parser_spec(spec_expr);

  // Get the velocity expressions, replace placeholders, and create parsers
  std::string velx_expr =
    prob_parm.ic_velx_expr ? prob_parm.ic_velx_expr : "0.0";
  velx_expr = replacePlaceholders(velx_expr, {{"%EQ%", "="}});
  amrex::Parser parser_velx(velx_expr);
  std::string vely_expr =
    prob_parm.ic_vely_expr ? prob_parm.ic_vely_expr : "0.0";
  vely_expr = replacePlaceholders(vely_expr, {{"%EQ%", "="}});
  amrex::Parser parser_vely(vely_expr);
  std::string velz_expr =
    prob_parm.ic_velz_expr ? prob_parm.ic_velz_expr : "0.0";
  velz_expr = replacePlaceholders(velz_expr, {{"%EQ%", "="}});
  amrex::Parser parser_velz(velz_expr);

  // Parse and set the constants for the coordinate expressions
  std::string x_dict_str =
    prob_parm.ic_x_dict ? prob_parm.ic_x_dict : "tmp@0.0;";
  parseAndSetConstants(x_dict_str, parser_x);
  std::string y_dict_str =
    prob_parm.ic_y_dict ? prob_parm.ic_y_dict : "tmp@0.0;";
  parseAndSetConstants(y_dict_str, parser_y);
  std::string z_dict_str =
    prob_parm.ic_z_dict ? prob_parm.ic_z_dict : "tmp@0.0;";
  parseAndSetConstants(z_dict_str, parser_z);

  // Parse and set the constants for the pressure expression
  std::string press_dict_str =
    prob_parm.ic_press_dict ? prob_parm.ic_press_dict : "tmp@0.0;";
  parseAndSetConstants(press_dict_str, parser_press);

  // Parse and set the constants for the temperature expression
  std::string temp_dict_str =
    prob_parm.ic_temp_dict ? prob_parm.ic_temp_dict : "tmp@0.0;";
  parseAndSetConstants(temp_dict_str, parser_temp);

  // Parse and set the constants for the species expressions
  std::string spec_dict_str =
    prob_parm.ic_spec_dict ? prob_parm.ic_spec_dict : "air@O2:0.233|N2:0.767;";
  // parseAndSetConstants(spec_dict_str, parser_spec);
  std::unordered_map<std::string, std::map<std::string, double>> species_map;
  std::vector<std::string> mixture_names;
  parseAndSetConstantsAndSpecies(
    spec_dict_str, parser_spec, species_map, mixture_names);

  // Parse and set the constants for the velocity expressions
  std::string velx_dict_str =
    prob_parm.ic_velx_dict ? prob_parm.ic_velx_dict : "tmp@0.0;";
  parseAndSetConstants(velx_dict_str, parser_velx);
  std::string vely_dict_str =
    prob_parm.ic_vely_dict ? prob_parm.ic_vely_dict : "tmp@0.0;";
  parseAndSetConstants(vely_dict_str, parser_vely);
  std::string velz_dict_str =
    prob_parm.ic_velz_dict ? prob_parm.ic_velz_dict : "tmp@0.0;";
  parseAndSetConstants(velz_dict_str, parser_velz);

  // Register the standard variables for each parser
  parser_x.registerVariables({"xlo", "xhi"});
  parser_y.registerVariables({"ylo", "yhi"});
  parser_z.registerVariables({"zlo", "zhi"});
  parser_press.registerVariables({"x", "y", "z"});
  parser_temp.registerVariables({"x", "y", "z"});
  parser_spec.registerVariables({"x", "y", "z"});
  parser_velx.registerVariables({"x", "y", "z"});
  parser_vely.registerVariables({"x", "y", "z"});
  parser_velz.registerVariables({"x", "y", "z"});

  // Compile the expressions and evaluate
  auto f_x = parser_x.compile<2>();
  auto f_y = parser_y.compile<2>();
  auto f_z = parser_z.compile<2>();
  auto f_press = parser_press.compile<3>();
  auto f_spec = parser_spec.compile<3>();
  auto f_temp = parser_temp.compile<3>();
  auto f_velx = parser_velx.compile<3>();
  auto f_vely = parser_vely.compile<3>();
  auto f_velz = parser_velz.compile<3>();

  amrex::Real x_c = f_x(prob_lo[0], prob_hi[0]);
  amrex::Real y_c = f_y(prob_lo[1], prob_hi[1]);
  amrex::Real z_c = f_z(prob_lo[2], prob_hi[2]);

  // Evaluate the expressions
  amrex::Real press = f_press(x - x_c, y - y_c, z - z_c);
  amrex::Real temp = f_temp(x - x_c, y - y_c, z - z_c);
  amrex::Real velx = f_velx(x - x_c, y - y_c, z - z_c);
  amrex::Real vely = f_vely(x - x_c, y - y_c, z - z_c);
  amrex::Real velz = f_velz(x - x_c, y - y_c, z - z_c);

  // Evaluate the expression to determine which mixture to use
  int mixture_index = static_cast<int>(f_spec(x - x_c, y - y_c, z - z_c));

  amrex::Real massfrac[NUM_SPECIES] = {0.0};

  for (const auto& [species_name, fraction] :
       species_map.at(mixture_names[mixture_index])) {
    massfrac[get_spec_id_by_name(species_name)] = fraction;
  }

  // ******************************************************
  // Set the state variables
  state(i, j, k, TEMP) = temp;
  AMREX_D_TERM(state(i, j, k, VELX) = velx;, state(i, j, k, VELY) = vely;
               , state(i, j, k, VELZ) = velz);

  // Compute density, enthalpy, and species mass
  auto eos = pele::physics::PhysicsType::eos();
  // amrex::Real P_cgs = prob_parm.P_mean * 10.0;
  amrex::Real P_cgs = press * 10.0;
  amrex::Real rho_cgs = 0.0;
  eos.PYT2R(P_cgs, massfrac, temp, rho_cgs);
  state(i, j, k, DENSITY) = rho_cgs * 1.0e3;

  amrex::Real h_cgs = 0.0;
  eos.TY2H(temp, massfrac, h_cgs);
  state(i, j, k, RHOH) = h_cgs * 1.0e-4 * state(i, j, k, DENSITY);

  // Species mass
  for (int n = 0; n < NUM_SPECIES; n++) {
    state(i, j, k, FIRSTSPEC + n) = massfrac[n] * state(i, j, k, DENSITY);
  }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void
bcnormal(
  const amrex::Real x[AMREX_SPACEDIM],
  const int /*m_nAux*/,
  amrex::Real s_ext[NVAR],
  const int idir,
  const int sgn,
  const amrex::Real time,
  amrex::GeometryData const& /*geomdata*/,
  ProbParm const& prob_parm,
  pele::physics::PMF::PmfData::DataContainer const* /*pmf_data*/)
{

  auto eos = pele::physics::PhysicsType::eos();

  // Get all of the boundary condition expressions and dictionaries
  // ******************************************************
  // X-Lo boundary condition expressions and dictionaries
  std::string xlo_press_expr =
    prob_parm.bc_xlo_press_expr ? prob_parm.bc_xlo_press_expr : "101325.0";
  std::string xlo_press_dict =
    prob_parm.bc_xlo_press_dict ? prob_parm.bc_xlo_press_dict : "tmp@0.0;";
  std::string xlo_temp_expr =
    prob_parm.bc_xlo_temp_expr ? prob_parm.bc_xlo_temp_expr : "300.0";
  std::string xlo_temp_dict =
    prob_parm.bc_xlo_temp_dict ? prob_parm.bc_xlo_temp_dict : "tmp@0.0;";
  std::string xlo_spec_expr =
    prob_parm.bc_xlo_spec_expr ? prob_parm.bc_xlo_spec_expr : "0";
  std::string xlo_spec_dict = prob_parm.bc_xlo_spec_dict
                                ? prob_parm.bc_xlo_spec_dict
                                : "air@O2:0.233|N2:0.767;";
  std::string xlo_velx_expr =
    prob_parm.bc_xlo_velx_expr ? prob_parm.bc_xlo_velx_expr : "0.0";
  std::string xlo_velx_dict =
    prob_parm.bc_xlo_velx_dict ? prob_parm.bc_xlo_velx_dict : "tmp@0.0;";
  std::string xlo_vely_expr =
    prob_parm.bc_xlo_vely_expr ? prob_parm.bc_xlo_vely_expr : "0.0";
  std::string xlo_vely_dict =
    prob_parm.bc_xlo_vely_dict ? prob_parm.bc_xlo_vely_dict : "tmp@0.0;";
  std::string xlo_velz_expr =
    prob_parm.bc_xlo_velz_expr ? prob_parm.bc_xlo_velz_expr : "0.0";
  std::string xlo_velz_dict =
    prob_parm.bc_xlo_velz_dict ? prob_parm.bc_xlo_velz_dict : "tmp@0.0;";
  // ******************************************************
  // X-Hi boundary condition expressions and dictionaries
  std::string xhi_press_expr =
    prob_parm.bc_xhi_press_expr ? prob_parm.bc_xhi_press_expr : "101325.0";
  std::string xhi_press_dict =
    prob_parm.bc_xhi_press_dict ? prob_parm.bc_xhi_press_dict : "tmp@0.0;";
  std::string xhi_temp_expr =
    prob_parm.bc_xhi_temp_expr ? prob_parm.bc_xhi_temp_expr : "300.0";
  std::string xhi_temp_dict =
    prob_parm.bc_xhi_temp_dict ? prob_parm.bc_xhi_temp_dict : "tmp@0.0;";
  std::string xhi_spec_expr =
    prob_parm.bc_xhi_spec_expr ? prob_parm.bc_xhi_spec_expr : "0";
  std::string xhi_spec_dict = prob_parm.bc_xhi_spec_dict
                                ? prob_parm.bc_xhi_spec_dict
                                : "air@O2:0.233|N2:0.767;";
  std::string xhi_velx_expr =
    prob_parm.bc_xhi_velx_expr ? prob_parm.bc_xhi_velx_expr : "0.0";
  std::string xhi_velx_dict =
    prob_parm.bc_xhi_velx_dict ? prob_parm.bc_xhi_velx_dict : "tmp@0.0;";
  std::string xhi_vely_expr =
    prob_parm.bc_xhi_vely_expr ? prob_parm.bc_xhi_vely_expr : "0.0";
  std::string xhi_vely_dict =
    prob_parm.bc_xhi_vely_dict ? prob_parm.bc_xhi_vely_dict : "tmp@0.0;";
  std::string xhi_velz_expr =
    prob_parm.bc_xhi_velz_expr ? prob_parm.bc_xhi_velz_expr : "0.0";
  std::string xhi_velz_dict =
    prob_parm.bc_xhi_velz_dict ? prob_parm.bc_xhi_velz_dict : "tmp@0.0;";
  // ******************************************************
  // Y-Lo boundary condition expressions and dictionaries
  std::string ylo_press_expr =
    prob_parm.bc_ylo_press_expr ? prob_parm.bc_ylo_press_expr : "101325.0";
  std::string ylo_press_dict =
    prob_parm.bc_ylo_press_dict ? prob_parm.bc_ylo_press_dict : "tmp@0.0;";
  std::string ylo_temp_expr =
    prob_parm.bc_ylo_temp_expr ? prob_parm.bc_ylo_temp_expr : "300.0";
  std::string ylo_temp_dict =
    prob_parm.bc_ylo_temp_dict ? prob_parm.bc_ylo_temp_dict : "tmp@0.0;";
  std::string ylo_spec_expr =
    prob_parm.bc_ylo_spec_expr ? prob_parm.bc_ylo_spec_expr : "0";
  std::string ylo_spec_dict = prob_parm.bc_ylo_spec_dict
                                ? prob_parm.bc_ylo_spec_dict
                                : "air@O2:0.233|N2:0.767;";
  std::string ylo_velx_expr =
    prob_parm.bc_ylo_velx_expr ? prob_parm.bc_ylo_velx_expr : "0.0";
  std::string ylo_velx_dict =
    prob_parm.bc_ylo_velx_dict ? prob_parm.bc_ylo_velx_dict : "tmp@0.0;";
  std::string ylo_vely_expr =
    prob_parm.bc_ylo_vely_expr ? prob_parm.bc_ylo_vely_expr : "0.0";
  std::string ylo_vely_dict =
    prob_parm.bc_ylo_vely_dict ? prob_parm.bc_ylo_vely_dict : "tmp@0.0;";
  std::string ylo_velz_expr =
    prob_parm.bc_ylo_velz_expr ? prob_parm.bc_ylo_velz_expr : "0.0";
  std::string ylo_velz_dict =
    prob_parm.bc_ylo_velz_dict ? prob_parm.bc_ylo_velz_dict : "tmp@0.0;";
  // ******************************************************
  // Y-Hi boundary condition expressions and dictionaries
  std::string yhi_press_expr =
    prob_parm.bc_yhi_press_expr ? prob_parm.bc_yhi_press_expr : "101325.0";
  std::string yhi_press_dict =
    prob_parm.bc_yhi_press_dict ? prob_parm.bc_yhi_press_dict : "tmp@0.0;";
  std::string yhi_temp_expr =
    prob_parm.bc_yhi_temp_expr ? prob_parm.bc_yhi_temp_expr : "300.0";
  std::string yhi_temp_dict =
    prob_parm.bc_yhi_temp_dict ? prob_parm.bc_yhi_temp_dict : "tmp@0.0;";
  std::string yhi_spec_expr =
    prob_parm.bc_yhi_spec_expr ? prob_parm.bc_yhi_spec_expr : "0";
  std::string yhi_spec_dict = prob_parm.bc_yhi_spec_dict
                                ? prob_parm.bc_yhi_spec_dict
                                : "air@O2:0.233|N2:0.767;";
  std::string yhi_velx_expr =
    prob_parm.bc_yhi_velx_expr ? prob_parm.bc_yhi_velx_expr : "0.0";
  std::string yhi_velx_dict =
    prob_parm.bc_yhi_velx_dict ? prob_parm.bc_yhi_velx_dict : "tmp@0.0;";
  std::string yhi_vely_expr =
    prob_parm.bc_yhi_vely_expr ? prob_parm.bc_yhi_vely_expr : "0.0";
  std::string yhi_vely_dict =
    prob_parm.bc_yhi_vely_dict ? prob_parm.bc_yhi_vely_dict : "tmp@0.0;";
  std::string yhi_velz_expr =
    prob_parm.bc_yhi_velz_expr ? prob_parm.bc_yhi_velz_expr : "0.0";
  std::string yhi_velz_dict =
    prob_parm.bc_yhi_velz_dict ? prob_parm.bc_yhi_velz_dict : "tmp@0.0;";
  // ******************************************************
  // Z-Lo boundary condition expressions and dictionaries
  std::string zlo_press_expr =
    prob_parm.bc_zlo_press_expr ? prob_parm.bc_zlo_press_expr : "101325.0";
  std::string zlo_press_dict =
    prob_parm.bc_zlo_press_dict ? prob_parm.bc_zlo_press_dict : "tmp@0.0;";
  std::string zlo_temp_expr =
    prob_parm.bc_zlo_temp_expr ? prob_parm.bc_zlo_temp_expr : "300.0";
  std::string zlo_temp_dict =
    prob_parm.bc_zlo_temp_dict ? prob_parm.bc_zlo_temp_dict : "tmp@0.0;";
  std::string zlo_spec_expr =
    prob_parm.bc_zlo_spec_expr ? prob_parm.bc_zlo_spec_expr : "0";
  std::string zlo_spec_dict = prob_parm.bc_zlo_spec_dict
                                ? prob_parm.bc_zlo_spec_dict
                                : "air@O2:0.233|N2:0.767;";
  std::string zlo_velx_expr =
    prob_parm.bc_zlo_velx_expr ? prob_parm.bc_zlo_velx_expr : "0.0";
  std::string zlo_velx_dict =
    prob_parm.bc_zlo_velx_dict ? prob_parm.bc_zlo_velx_dict : "tmp@0.0;";
  std::string zlo_vely_expr =
    prob_parm.bc_zlo_vely_expr ? prob_parm.bc_zlo_vely_expr : "0.0";
  std::string zlo_vely_dict =
    prob_parm.bc_zlo_vely_dict ? prob_parm.bc_zlo_vely_dict : "tmp@0.0;";
  std::string zlo_velz_expr =
    prob_parm.bc_zlo_velz_expr ? prob_parm.bc_zlo_velz_expr : "0.0";
  std::string zlo_velz_dict =
    prob_parm.bc_zlo_velz_dict ? prob_parm.bc_zlo_velz_dict : "tmp@0.0;";
  // ******************************************************
  // Z-Hi boundary condition expressions and dictionaries
  std::string zhi_press_expr =
    prob_parm.bc_zhi_press_expr ? prob_parm.bc_zhi_press_expr : "101325.0";
  std::string zhi_press_dict =
    prob_parm.bc_zhi_press_dict ? prob_parm.bc_zhi_press_dict : "tmp@0.0;";
  std::string zhi_temp_expr =
    prob_parm.bc_zhi_temp_expr ? prob_parm.bc_zhi_temp_expr : "300.0";
  std::string zhi_temp_dict =
    prob_parm.bc_zhi_temp_dict ? prob_parm.bc_zhi_temp_dict : "tmp@0.0;";
  std::string zhi_spec_expr =
    prob_parm.bc_zhi_spec_expr ? prob_parm.bc_zhi_spec_expr : "0";
  std::string zhi_spec_dict = prob_parm.bc_zhi_spec_dict
                                ? prob_parm.bc_zhi_spec_dict
                                : "air@O2:0.233|N2:0.767;";
  std::string zhi_velx_expr =
    prob_parm.bc_zhi_velx_expr ? prob_parm.bc_zhi_velx_expr : "0.0";
  std::string zhi_velx_dict =
    prob_parm.bc_zhi_velx_dict ? prob_parm.bc_zhi_velx_dict : "tmp@0.0;";
  std::string zhi_vely_expr =
    prob_parm.bc_zhi_vely_expr ? prob_parm.bc_zhi_vely_expr : "0.0";
  std::string zhi_vely_dict =
    prob_parm.bc_zhi_vely_dict ? prob_parm.bc_zhi_vely_dict : "tmp@0.0;";
  std::string zhi_velz_expr =
    prob_parm.bc_zhi_velz_expr ? prob_parm.bc_zhi_velz_expr : "0.0";
  std::string zhi_velz_dict =
    prob_parm.bc_zhi_velz_dict ? prob_parm.bc_zhi_velz_dict : "tmp@0.0;";
  // ******************************************************

  // Replace placeholders in the expressions
  // xlo expressions
  xlo_press_expr = replacePlaceholders(xlo_press_expr, {{"%EQ%", "="}});
  xlo_temp_expr = replacePlaceholders(xlo_temp_expr, {{"%EQ%", "="}});
  xlo_spec_expr = replacePlaceholders(xlo_spec_expr, {{"%EQ%", "="}});
  xlo_velx_expr = replacePlaceholders(xlo_velx_expr, {{"%EQ%", "="}});
  xlo_vely_expr = replacePlaceholders(xlo_vely_expr, {{"%EQ%", "="}});
  xlo_velz_expr = replacePlaceholders(xlo_velz_expr, {{"%EQ%", "="}});
  // xhi expressions
  xhi_press_expr = replacePlaceholders(xhi_press_expr, {{"%EQ%", "="}});
  xhi_temp_expr = replacePlaceholders(xhi_temp_expr, {{"%EQ%", "="}});
  xhi_spec_expr = replacePlaceholders(xhi_spec_expr, {{"%EQ%", "="}});
  xhi_velx_expr = replacePlaceholders(xhi_velx_expr, {{"%EQ%", "="}});
  xhi_vely_expr = replacePlaceholders(xhi_vely_expr, {{"%EQ%", "="}});
  xhi_velz_expr = replacePlaceholders(xhi_velz_expr, {{"%EQ%", "="}});
  // ylo expressions
  ylo_press_expr = replacePlaceholders(ylo_press_expr, {{"%EQ%", "="}});
  ylo_temp_expr = replacePlaceholders(ylo_temp_expr, {{"%EQ%", "="}});
  ylo_spec_expr = replacePlaceholders(ylo_spec_expr, {{"%EQ%", "="}});
  ylo_velx_expr = replacePlaceholders(ylo_velx_expr, {{"%EQ%", "="}});
  ylo_vely_expr = replacePlaceholders(ylo_vely_expr, {{"%EQ%", "="}});
  ylo_velz_expr = replacePlaceholders(ylo_velz_expr, {{"%EQ%", "="}});
  // yhi expressions
  yhi_press_expr = replacePlaceholders(yhi_press_expr, {{"%EQ%", "="}});
  yhi_temp_expr = replacePlaceholders(yhi_temp_expr, {{"%EQ%", "="}});
  yhi_spec_expr = replacePlaceholders(yhi_spec_expr, {{"%EQ%", "="}});
  yhi_velx_expr = replacePlaceholders(yhi_velx_expr, {{"%EQ%", "="}});
  yhi_vely_expr = replacePlaceholders(yhi_vely_expr, {{"%EQ%", "="}});
  yhi_velz_expr = replacePlaceholders(yhi_velz_expr, {{"%EQ%", "="}});
  // zlo expressions
  zlo_press_expr = replacePlaceholders(zlo_press_expr, {{"%EQ%", "="}});
  zlo_temp_expr = replacePlaceholders(zlo_temp_expr, {{"%EQ%", "="}});
  zlo_spec_expr = replacePlaceholders(zlo_spec_expr, {{"%EQ%", "="}});
  zlo_velx_expr = replacePlaceholders(zlo_velx_expr, {{"%EQ%", "="}});
  zlo_vely_expr = replacePlaceholders(zlo_vely_expr, {{"%EQ%", "="}});
  zlo_velz_expr = replacePlaceholders(zlo_velz_expr, {{"%EQ%", "="}});
  // zhi expressions
  zhi_press_expr = replacePlaceholders(zhi_press_expr, {{"%EQ%", "="}});
  zhi_temp_expr = replacePlaceholders(zhi_temp_expr, {{"%EQ%", "="}});
  zhi_spec_expr = replacePlaceholders(zhi_spec_expr, {{"%EQ%", "="}});
  zhi_velx_expr = replacePlaceholders(zhi_velx_expr, {{"%EQ%", "="}});
  zhi_vely_expr = replacePlaceholders(zhi_vely_expr, {{"%EQ%", "="}});
  zhi_velz_expr = replacePlaceholders(zhi_velz_expr, {{"%EQ%", "="}});
  // ******************************************************

  // Create parsers for the expressions
  // xlo parsers
  amrex::Parser parser_xlo_press(xlo_press_expr);
  amrex::Parser parser_xlo_temp(xlo_temp_expr);
  amrex::Parser parser_xlo_spec(xlo_spec_expr);
  amrex::Parser parser_xlo_velx(xlo_velx_expr);
  amrex::Parser parser_xlo_vely(xlo_vely_expr);
  amrex::Parser parser_xlo_velz(xlo_velz_expr);
  // xhi parsers
  amrex::Parser parser_xhi_press(xhi_press_expr);
  amrex::Parser parser_xhi_temp(xhi_temp_expr);
  amrex::Parser parser_xhi_spec(xhi_spec_expr);
  amrex::Parser parser_xhi_velx(xhi_velx_expr);
  amrex::Parser parser_xhi_vely(xhi_vely_expr);
  amrex::Parser parser_xhi_velz(xhi_velz_expr);
  // ylo parsers
  amrex::Parser parser_ylo_press(ylo_press_expr);
  amrex::Parser parser_ylo_temp(ylo_temp_expr);
  amrex::Parser parser_ylo_spec(ylo_spec_expr);
  amrex::Parser parser_ylo_velx(ylo_velx_expr);
  amrex::Parser parser_ylo_vely(ylo_vely_expr);
  amrex::Parser parser_ylo_velz(ylo_velz_expr);
  // yhi parsers
  amrex::Parser parser_yhi_press(yhi_press_expr);
  amrex::Parser parser_yhi_temp(yhi_temp_expr);
  amrex::Parser parser_yhi_spec(yhi_spec_expr);
  amrex::Parser parser_yhi_velx(yhi_velx_expr);
  amrex::Parser parser_yhi_vely(yhi_vely_expr);
  amrex::Parser parser_yhi_velz(yhi_velz_expr);
  // zlo parsers
  amrex::Parser parser_zlo_press(zlo_press_expr);
  amrex::Parser parser_zlo_temp(zlo_temp_expr);
  amrex::Parser parser_zlo_spec(zlo_spec_expr);
  amrex::Parser parser_zlo_velx(zlo_velx_expr);
  amrex::Parser parser_zlo_vely(zlo_vely_expr);
  amrex::Parser parser_zlo_velz(zlo_velz_expr);
  // zhi parsers
  amrex::Parser parser_zhi_press(zhi_press_expr);
  amrex::Parser parser_zhi_temp(zhi_temp_expr);
  amrex::Parser parser_zhi_spec(zhi_spec_expr);
  amrex::Parser parser_zhi_velx(zhi_velx_expr);
  amrex::Parser parser_zhi_vely(zhi_vely_expr);
  amrex::Parser parser_zhi_velz(zhi_velz_expr);
  // ******************************************************

  // Parse the dictionaries and set the constants for the expressions
  // xlo dictionaries
  std::vector<std::string> xlo_mixture_names;
  std::unordered_map<std::string, std::map<std::string, double>>
    xlo_species_map;
  parseAndSetConstantsAndSpecies(
    xlo_spec_dict, parser_xlo_spec, xlo_species_map, xlo_mixture_names);
  parseAndSetConstants(xlo_press_dict, parser_xlo_press);
  parseAndSetConstants(xlo_temp_dict, parser_xlo_temp);
  parseAndSetConstants(xlo_velx_dict, parser_xlo_velx);
  parseAndSetConstants(xlo_vely_dict, parser_xlo_vely);
  parseAndSetConstants(xlo_velz_dict, parser_xlo_velz);
  // xhi dictionaries
  std::vector<std::string> xhi_mixture_names;
  std::unordered_map<std::string, std::map<std::string, double>>
    xhi_species_map;
  parseAndSetConstantsAndSpecies(
    xhi_spec_dict, parser_xhi_spec, xhi_species_map, xhi_mixture_names);
  parseAndSetConstants(xhi_press_dict, parser_xhi_press);
  parseAndSetConstants(xhi_temp_dict, parser_xhi_temp);
  parseAndSetConstants(xhi_velx_dict, parser_xhi_velx);
  parseAndSetConstants(xhi_vely_dict, parser_xhi_vely);
  parseAndSetConstants(xhi_velz_dict, parser_xhi_velz);
  // ylo dictionaries
  std::vector<std::string> ylo_mixture_names;
  std::unordered_map<std::string, std::map<std::string, double>>
    ylo_species_map;
  parseAndSetConstantsAndSpecies(
    ylo_spec_dict, parser_ylo_spec, ylo_species_map, ylo_mixture_names);
  parseAndSetConstants(ylo_press_dict, parser_ylo_press);
  parseAndSetConstants(ylo_temp_dict, parser_ylo_temp);
  parseAndSetConstants(ylo_velx_dict, parser_ylo_velx);
  parseAndSetConstants(ylo_vely_dict, parser_ylo_vely);
  parseAndSetConstants(ylo_velz_dict, parser_ylo_velz);
  // yhi dictionaries
  std::vector<std::string> yhi_mixture_names;
  std::unordered_map<std::string, std::map<std::string, double>>
    yhi_species_map;
  parseAndSetConstantsAndSpecies(
    yhi_spec_dict, parser_yhi_spec, yhi_species_map, yhi_mixture_names);
  parseAndSetConstants(yhi_press_dict, parser_yhi_press);
  parseAndSetConstants(yhi_temp_dict, parser_yhi_temp);
  parseAndSetConstants(yhi_velx_dict, parser_yhi_velx);
  parseAndSetConstants(yhi_vely_dict, parser_yhi_vely);
  parseAndSetConstants(yhi_velz_dict, parser_yhi_velz);
  // zlo dictionaries
  std::vector<std::string> zlo_mixture_names;
  std::unordered_map<std::string, std::map<std::string, double>>
    zlo_species_map;
  parseAndSetConstantsAndSpecies(
    zlo_spec_dict, parser_zlo_spec, zlo_species_map, zlo_mixture_names);
  parseAndSetConstants(zlo_press_dict, parser_zlo_press);
  parseAndSetConstants(zlo_temp_dict, parser_zlo_temp);
  parseAndSetConstants(zlo_velx_dict, parser_zlo_velx);
  parseAndSetConstants(zlo_vely_dict, parser_zlo_vely);
  parseAndSetConstants(zlo_velz_dict, parser_zlo_velz);
  // zhi dictionaries
  std::vector<std::string> zhi_mixture_names;
  std::unordered_map<std::string, std::map<std::string, double>>
    zhi_species_map;
  parseAndSetConstantsAndSpecies(
    zhi_spec_dict, parser_zhi_spec, zhi_species_map, zhi_mixture_names);
  parseAndSetConstants(zhi_press_dict, parser_zhi_press);
  parseAndSetConstants(zhi_temp_dict, parser_zhi_temp);
  parseAndSetConstants(zhi_velx_dict, parser_zhi_velx);
  parseAndSetConstants(zhi_vely_dict, parser_zhi_vely);
  parseAndSetConstants(zhi_velz_dict, parser_zhi_velz);
  // ******************************************************

  // Register the standard variables for each parser
  // xlo variables
  parser_xlo_press.registerVariables({"x", "y", "z", "time"});
  parser_xlo_temp.registerVariables({"x", "y", "z", "time"});
  parser_xlo_spec.registerVariables({"x", "y", "z", "time"});
  parser_xlo_velx.registerVariables({"x", "y", "z", "time"});
  parser_xlo_vely.registerVariables({"x", "y", "z", "time"});
  parser_xlo_velz.registerVariables({"x", "y", "z", "time"});
  // xhi variables
  parser_xhi_press.registerVariables({"x", "y", "z", "time"});
  parser_xhi_temp.registerVariables({"x", "y", "z", "time"});
  parser_xhi_spec.registerVariables({"x", "y", "z", "time"});
  parser_xhi_velx.registerVariables({"x", "y", "z", "time"});
  parser_xhi_vely.registerVariables({"x", "y", "z", "time"});
  parser_xhi_velz.registerVariables({"x", "y", "z", "time"});
  // ylo variables
  parser_ylo_press.registerVariables({"x", "y", "z", "time"});
  parser_ylo_temp.registerVariables({"x", "y", "z", "time"});
  parser_ylo_spec.registerVariables({"x", "y", "z", "time"});
  parser_ylo_velx.registerVariables({"x", "y", "z", "time"});
  parser_ylo_vely.registerVariables({"x", "y", "z", "time"});
  parser_ylo_velz.registerVariables({"x", "y", "z", "time"});
  // yhi variables
  parser_yhi_press.registerVariables({"x", "y", "z", "time"});
  parser_yhi_temp.registerVariables({"x", "y", "z", "time"});
  parser_yhi_spec.registerVariables({"x", "y", "z", "time"});
  parser_yhi_velx.registerVariables({"x", "y", "z", "time"});
  parser_yhi_vely.registerVariables({"x", "y", "z", "time"});
  parser_yhi_velz.registerVariables({"x", "y", "z", "time"});
  // zlo variables
  parser_zlo_press.registerVariables({"x", "y", "z", "time"});
  parser_zlo_temp.registerVariables({"x", "y", "z", "time"});
  parser_zlo_spec.registerVariables({"x", "y", "z", "time"});
  parser_zlo_velx.registerVariables({"x", "y", "z", "time"});
  parser_zlo_vely.registerVariables({"x", "y", "z", "time"});
  parser_zlo_velz.registerVariables({"x", "y", "z", "time"});
  // zhi variables
  parser_zhi_press.registerVariables({"x", "y", "z", "time"});
  parser_zhi_temp.registerVariables({"x", "y", "z", "time"});
  parser_zhi_spec.registerVariables({"x", "y", "z", "time"});
  parser_zhi_velx.registerVariables({"x", "y", "z", "time"});
  parser_zhi_vely.registerVariables({"x", "y", "z", "time"});
  parser_zhi_velz.registerVariables({"x", "y", "z", "time"});
  // ******************************************************

  // Compile the expressions
  // xlo expressions
  auto f_xlo_press = parser_xlo_press.compile<4>();
  auto f_xlo_temp = parser_xlo_temp.compile<4>();
  auto f_xlo_velx = parser_xlo_velx.compile<4>();
  auto f_xlo_vely = parser_xlo_vely.compile<4>();
  auto f_xlo_velz = parser_xlo_velz.compile<4>();
  auto f_xlo_spec = parser_xlo_spec.compile<4>();
  // xhi expressions
  auto f_xhi_press = parser_xhi_press.compile<4>();
  auto f_xhi_temp = parser_xhi_temp.compile<4>();
  auto f_xhi_velx = parser_xhi_velx.compile<4>();
  auto f_xhi_vely = parser_xhi_vely.compile<4>();
  auto f_xhi_velz = parser_xhi_velz.compile<4>();
  auto f_xhi_spec = parser_xhi_spec.compile<4>();
  // ylo expressions
  auto f_ylo_press = parser_ylo_press.compile<4>();
  auto f_ylo_temp = parser_ylo_temp.compile<4>();
  auto f_ylo_velx = parser_ylo_velx.compile<4>();
  auto f_ylo_vely = parser_ylo_vely.compile<4>();
  auto f_ylo_velz = parser_ylo_velz.compile<4>();
  auto f_ylo_spec = parser_ylo_spec.compile<4>();
  // yhi expressions
  auto f_yhi_press = parser_yhi_press.compile<4>();
  auto f_yhi_temp = parser_yhi_temp.compile<4>();
  auto f_yhi_velx = parser_yhi_velx.compile<4>();
  auto f_yhi_vely = parser_yhi_vely.compile<4>();
  auto f_yhi_velz = parser_yhi_velz.compile<4>();
  auto f_yhi_spec = parser_yhi_spec.compile<4>();
  // zlo expressions
  auto f_zlo_press = parser_zlo_press.compile<4>();
  auto f_zlo_temp = parser_zlo_temp.compile<4>();
  auto f_zlo_velx = parser_zlo_velx.compile<4>();
  auto f_zlo_vely = parser_zlo_vely.compile<4>();
  auto f_zlo_velz = parser_zlo_velz.compile<4>();
  auto f_zlo_spec = parser_zlo_spec.compile<4>();
  // zhi expressions
  auto f_zhi_press = parser_zhi_press.compile<4>();
  auto f_zhi_temp = parser_zhi_temp.compile<4>();
  auto f_zhi_velx = parser_zhi_velx.compile<4>();
  auto f_zhi_vely = parser_zhi_vely.compile<4>();
  auto f_zhi_velz = parser_zhi_velz.compile<4>();
  auto f_zhi_spec = parser_zhi_spec.compile<4>();
  // ******************************************************

  // Set the boundary conditions based on the direction and sign
  // xlo
  if (idir == 0 and sgn == 1) {
    amrex::Real massfrac[NUM_SPECIES] = {0.0};
    int mixture_index = static_cast<int>(f_xlo_spec(x[0], x[1], x[2], time));
    for (const auto& [species_name, fraction] :
         xlo_species_map.at(xlo_mixture_names[mixture_index])) {
      massfrac[get_spec_id_by_name(species_name)] = fraction;
    }

    amrex::Real press = f_xlo_press(x[0], x[1], x[2], time);
    amrex::Real rho_cgs, P_cgs, RhoH_temp;
    P_cgs = press * 10.0;

    s_ext[TEMP] = f_xlo_temp(x[0], x[1], x[2], time);

    // ******************************************************
    eos.PYT2R(P_cgs, massfrac, s_ext[TEMP], rho_cgs);
    s_ext[DENSITY] = rho_cgs * 1.0e3;

    eos.TY2H(s_ext[TEMP], massfrac, RhoH_temp);
    s_ext[RHOH] = RhoH_temp * 1.0e-4 * s_ext[DENSITY]; // CGS -> MKS conversion

    for (int n = 0; n < NUM_SPECIES; n++) {
      s_ext[FIRSTSPEC + n] = massfrac[n] * s_ext[DENSITY];
    }

    // Velocity
    s_ext[VELX] = f_xlo_velx(x[0], x[1], x[2], time);
    s_ext[VELY] = f_xlo_vely(x[0], x[1], x[2], time);
    s_ext[VELZ] = f_xlo_velz(x[0], x[1], x[2], time);
    // ******************************************************
  } else if (idir == 0 and sgn == -1) {
    // xhi
    amrex::Real massfrac[NUM_SPECIES] = {0.0};
    int mixture_index = static_cast<int>(f_xhi_spec(x[0], x[1], x[2], time));
    for (const auto& [species_name, fraction] :
         xhi_species_map.at(xhi_mixture_names[mixture_index])) {
      massfrac[get_spec_id_by_name(species_name)] = fraction;
    }

    amrex::Real press = f_xhi_press(x[0], x[1], x[2], time);
    amrex::Real rho_cgs, P_cgs, RhoH_temp;
    P_cgs = press * 10.0;

    s_ext[TEMP] = f_xhi_temp(x[0], x[1], x[2], time);

    // ******************************************************
    eos.PYT2R(P_cgs, massfrac, s_ext[TEMP], rho_cgs);
    s_ext[DENSITY] = rho_cgs * 1.0e3;

    eos.TY2H(s_ext[TEMP], massfrac, RhoH_temp);
    s_ext[RHOH] = RhoH_temp * 1.0e-4 * s_ext[DENSITY]; // CGS -> MKS conversion

    for (int n = 0; n < NUM_SPECIES; n++) {
      s_ext[FIRSTSPEC + n] = massfrac[n] * s_ext[DENSITY];
    }

    // Velocity
    s_ext[VELX] = f_xhi_velx(x[0], x[1], x[2], time);
    s_ext[VELY] = f_xhi_vely(x[0], x[1], x[2], time);
    s_ext[VELZ] = f_xhi_velz(x[0], x[1], x[2], time);
    // ******************************************************
  } else if (idir == 1 and sgn == 1) {
    // ylo
    amrex::Real massfrac[NUM_SPECIES] = {0.0};
    int mixture_index = static_cast<int>(f_ylo_spec(x[0], x[1], x[2], time));
    for (const auto& [species_name, fraction] :
         ylo_species_map.at(ylo_mixture_names[mixture_index]))
      massfrac[get_spec_id_by_name(species_name)] = fraction;

    amrex::Real press = f_ylo_press(x[0], x[1], x[2], time);
    amrex::Real rho_cgs, P_cgs, RhoH_temp;
    P_cgs = press * 10.0;

    s_ext[TEMP] = f_ylo_temp(x[0], x[1], x[2], time);

    // ******************************************************
    eos.PYT2R(P_cgs, massfrac, s_ext[TEMP], rho_cgs);
    s_ext[DENSITY] = rho_cgs * 1.0e3;

    eos.TY2H(s_ext[TEMP], massfrac, RhoH_temp);
    s_ext[RHOH] = RhoH_temp * 1.0e-4 * s_ext[DENSITY]; // CGS -> MKS conversion

    for (int n = 0; n < NUM_SPECIES; n++) {
      s_ext[FIRSTSPEC + n] = massfrac[n] * s_ext[DENSITY];
    }

    // Velocity
    s_ext[VELX] = f_ylo_velx(x[0], x[1], x[2], time);
    s_ext[VELY] = f_ylo_vely(x[0], x[1], x[2], time);
    s_ext[VELZ] = f_ylo_velz(x[0], x[1], x[2], time);
    // ******************************************************
  } else if (idir == 1 and sgn == -1) {
    // yhi
    amrex::Real massfrac[NUM_SPECIES] = {0.0};
    int mixture_index = static_cast<int>(f_yhi_spec(x[0], x[1], x[2], time));
    for (const auto& [species_name, fraction] :
         yhi_species_map.at(yhi_mixture_names[mixture_index]))
      massfrac[get_spec_id_by_name(species_name)] = fraction;

    amrex::Real press = f_yhi_press(x[0], x[1], x[2], time);
    amrex::Real rho_cgs, P_cgs, RhoH_temp;
    P_cgs = press * 10.0;

    s_ext[TEMP] = f_yhi_temp(x[0], x[1], x[2], time);

    // ******************************************************
    eos.PYT2R(P_cgs, massfrac, s_ext[TEMP], rho_cgs);
    s_ext[DENSITY] = rho_cgs * 1.0e3;

    eos.TY2H(s_ext[TEMP], massfrac, RhoH_temp);
    s_ext[RHOH] = RhoH_temp * 1.0e-4 * s_ext[DENSITY]; // CGS -> MKS conversion

    for (int n = 0; n < NUM_SPECIES; n++) {
      s_ext[FIRSTSPEC + n] = massfrac[n] * s_ext[DENSITY];
    }

    // Velocity
    s_ext[VELX] = f_yhi_velx(x[0], x[1], x[2], time);
    s_ext[VELY] = f_yhi_vely(x[0], x[1], x[2], time);
    s_ext[VELZ] = f_yhi_velz(x[0], x[1], x[2], time);
    // ******************************************************
  } else if (idir == 2 and sgn == 1) {
    // zlo
    amrex::Real massfrac[NUM_SPECIES] = {0.0};
    int mixture_index = static_cast<int>(f_zlo_spec(x[0], x[1], x[2], time));
    for (const auto& [species_name, fraction] :
         zlo_species_map.at(zlo_mixture_names[mixture_index]))
      massfrac[get_spec_id_by_name(species_name)] = fraction;

    amrex::Real press = f_zlo_press(x[0], x[1], x[2], time);
    amrex::Real rho_cgs, P_cgs, RhoH_temp;
    P_cgs = press * 10.0;

    s_ext[TEMP] = f_zlo_temp(x[0], x[1], x[2], time);

    // ******************************************************
    eos.PYT2R(P_cgs, massfrac, s_ext[TEMP], rho_cgs);
    s_ext[DENSITY] = rho_cgs * 1.0e3;

    eos.TY2H(s_ext[TEMP], massfrac, RhoH_temp);
    s_ext[RHOH] = RhoH_temp * 1.0e-4 * s_ext[DENSITY]; // CGS -> MKS conversion

    for (int n = 0; n < NUM_SPECIES; n++) {
      s_ext[FIRSTSPEC + n] = massfrac[n] * s_ext[DENSITY];
    }

    // Velocity
    s_ext[VELX] = f_zlo_velx(x[0], x[1], x[2], time);
    s_ext[VELY] = f_zlo_vely(x[0], x[1], x[2], time);
    s_ext[VELZ] = f_zlo_velz(x[0], x[1], x[2], time);
    // ******************************************************
  } else if (idir == 2 and sgn == -1) {
    // zhi
    amrex::Real massfrac[NUM_SPECIES] = {0.0};
    int mixture_index = static_cast<int>(f_zhi_spec(x[0], x[1], x[2], time));
    for (const auto& [species_name, fraction] :
         zhi_species_map.at(zhi_mixture_names[mixture_index]))
      massfrac[get_spec_id_by_name(species_name)] = fraction;

    amrex::Real press = f_zhi_press(x[0], x[1], x[2], time);
    amrex::Real rho_cgs, P_cgs, RhoH_temp;
    P_cgs = press * 10.0;

    s_ext[TEMP] = f_zhi_temp(x[0], x[1], x[2], time);

    // ******************************************************
    eos.PYT2R(P_cgs, massfrac, s_ext[TEMP], rho_cgs);
    s_ext[DENSITY] = rho_cgs * 1.0e3;

    eos.TY2H(s_ext[TEMP], massfrac, RhoH_temp);
    s_ext[RHOH] = RhoH_temp * 1.0e-4 * s_ext[DENSITY]; // CGS -> MKS conversion

    for (int n = 0; n < NUM_SPECIES; n++) {
      s_ext[FIRSTSPEC + n] = massfrac[n] * s_ext[DENSITY];
    }

    // Velocity
    s_ext[VELX] = f_zhi_velx(x[0], x[1], x[2], time);
    s_ext[VELY] = f_zhi_vely(x[0], x[1], x[2], time);
    s_ext[VELZ] = f_zhi_velz(x[0], x[1], x[2], time);
    // ******************************************************
  }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void
zero_visc(
  int i,
  int j,
  int k,
  amrex::Array4<amrex::Real> const& beta,
  amrex::GeometryData const& geomdata,
  amrex::Box const& domainBox,
  const int dir,
  const int beta_comp,
  const int nComp)
{
  amrex::ignore_unused(
    i, j, k, beta, geomdata, domainBox, dir, beta_comp, nComp);
  // We treat species when beta_comp == 0 and nComp == NUM_SPECIES
  // otherwise this routine could be called for other face diffusivity (Temp,
  // velocity, ...)
}
#endif
