#include <PeleLMeX.H>
#include <AMReX_ParmParse.H>

#include <algorithm>

namespace
{

std::string
trimCopy(const std::string& s)
{
  auto begin = s.find_first_not_of(" \t\n\r");
  if (begin == std::string::npos) {
    return "";
  }
  auto end = s.find_last_not_of(" \t\n\r");
  return s.substr(begin, end - begin + 1);
}

std::string
toStringOrDefault(const char* ptr, const char* fallback)
{
  return ptr != nullptr ? std::string(ptr) : std::string(fallback);
}

void
parseAndPackSpeciesDict(
  const std::string& dict_str,
  amrex::Parser& parser,
  const std::unordered_map<std::string, int>& species_ids,
  int& num_mixtures,
  int* mix_species_count,
  int* mix_species_id,
  amrex::Real* mix_species_val,
  const std::string& context)
{
  auto handle_pair = [&](const std::string& raw_pair) {
    std::string pair_str = trimCopy(raw_pair);
    if (pair_str.empty()) {
      return;
    }

    size_t separator = pair_str.find('@');
    if (separator == std::string::npos) {
      return;
    }

    std::string key = trimCopy(pair_str.substr(0, separator));
    std::string value_str = trimCopy(pair_str.substr(separator + 1));
    if (key.empty() || value_str.empty()) {
      return;
    }

    if (value_str.find('|') == std::string::npos) {
      parser.setConstant(key, std::stod(value_str));
      return;
    }

    if (num_mixtures >= ProbParm::MAX_MIXTURES) {
      amrex::Abort("Too many mixtures in " + context);
    }
    int mix_idx = num_mixtures;
    num_mixtures += 1;

    size_t start = 0;
    size_t end = value_str.find('|');
    while (true) {
      std::string species_entry = trimCopy(value_str.substr(start, end - start));
      size_t colon_pos = species_entry.find(':');
      if (colon_pos != std::string::npos) {
        std::string species_name = trimCopy(species_entry.substr(0, colon_pos));
        std::string frac_str = trimCopy(species_entry.substr(colon_pos + 1));
        auto it = species_ids.find(species_name);
        if (it == species_ids.end()) {
          amrex::Abort(
            "Unknown species name in " + context + ": " + species_name);
        }

        int count = mix_species_count[mix_idx];
        if (count >= ProbParm::MAX_SPECIES_PER_MIXTURE) {
          amrex::Abort("Too many species in one mixture in " + context);
        }

        const int flat_idx =
          mix_idx * ProbParm::MAX_SPECIES_PER_MIXTURE + count;
        mix_species_id[flat_idx] = it->second;
        mix_species_val[flat_idx] = std::stod(frac_str);
        mix_species_count[mix_idx] = count + 1;
      }

      if (end == std::string::npos) {
        break;
      }
      start = end + 1;
      end = value_str.find('|', start);
    }
  };

  if (dict_str.empty()) {
    return;
  }

  size_t start = 0;
  size_t end = dict_str.find(';');
  while (end != std::string::npos) {
    handle_pair(dict_str.substr(start, end - start));
    start = end + 1;
    end = dict_str.find(';', start);
  }
  handle_pair(dict_str.substr(start));
}

} // namespace

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

void
PeleLM::buildProbParmRuntime()
{
  prob_parm->ic_runtime_ready = 0;
  prob_parm->bc_runtime_ready = 0;
  prob_parm->ic_num_mixtures = 0;
  std::fill_n(
    prob_parm->ic_mix_species_count,
    ProbParm::MAX_MIXTURES,
    0);
  std::fill_n(
    prob_parm->ic_mix_species_id,
    ProbParm::MAX_MIXTURES * ProbParm::MAX_SPECIES_PER_MIXTURE,
    0);
  std::fill_n(
    prob_parm->ic_mix_species_val,
    ProbParm::MAX_MIXTURES * ProbParm::MAX_SPECIES_PER_MIXTURE,
    0.0_rt);
  std::fill_n(prob_parm->bc_num_mixtures, ProbParm::NUM_BC_FACES, 0);
  std::fill_n(
    prob_parm->bc_mix_species_count,
    ProbParm::NUM_BC_FACES * ProbParm::MAX_MIXTURES,
    0);
  std::fill_n(
    prob_parm->bc_mix_species_id,
    ProbParm::NUM_BC_FACES * ProbParm::MAX_MIXTURES *
      ProbParm::MAX_SPECIES_PER_MIXTURE,
    0);
  std::fill_n(
    prob_parm->bc_mix_species_val,
    ProbParm::NUM_BC_FACES * ProbParm::MAX_MIXTURES *
      ProbParm::MAX_SPECIES_PER_MIXTURE,
    0.0_rt);

  // Keep parser owners alive for the runtime of the simulation
  m_prob_parsers.clear();
  m_prob_parsers.reserve(45);

  auto make_parser = [this](const std::string& expr) -> amrex::Parser& {
    m_prob_parsers.emplace_back(std::make_unique<amrex::Parser>(expr));
    return *(m_prob_parsers.back());
  };

  const std::string x_expr =
    replacePlaceholders(toStringOrDefault(prob_parm->ic_x_expr, "0.0"), {{"%EQ%", "="}});
  const std::string y_expr =
    replacePlaceholders(toStringOrDefault(prob_parm->ic_y_expr, "0.0"), {{"%EQ%", "="}});
  const std::string z_expr =
    replacePlaceholders(toStringOrDefault(prob_parm->ic_z_expr, "0.0"), {{"%EQ%", "="}});
  const std::string press_expr =
    replacePlaceholders(toStringOrDefault(prob_parm->ic_press_expr, "101325.0"), {{"%EQ%", "="}});
  const std::string temp_expr =
    replacePlaceholders(toStringOrDefault(prob_parm->ic_temp_expr, "300.0"), {{"%EQ%", "="}});
  const std::string spec_expr =
    replacePlaceholders(toStringOrDefault(prob_parm->ic_spec_expr, "0"), {{"%EQ%", "="}});
  const std::string velx_expr =
    replacePlaceholders(toStringOrDefault(prob_parm->ic_velx_expr, "0.0"), {{"%EQ%", "="}});
  const std::string vely_expr =
    replacePlaceholders(toStringOrDefault(prob_parm->ic_vely_expr, "0.0"), {{"%EQ%", "="}});
  const std::string velz_expr =
    replacePlaceholders(toStringOrDefault(prob_parm->ic_velz_expr, "0.0"), {{"%EQ%", "="}});

  auto& parser_x = make_parser(x_expr);
  auto& parser_y = make_parser(y_expr);
  auto& parser_z = make_parser(z_expr);
  auto& parser_press = make_parser(press_expr);
  auto& parser_temp = make_parser(temp_expr);
  auto& parser_spec = make_parser(spec_expr);
  auto& parser_velx = make_parser(velx_expr);
  auto& parser_vely = make_parser(vely_expr);
  auto& parser_velz = make_parser(velz_expr);

  parseAndSetConstants(
    toStringOrDefault(prob_parm->ic_x_dict, "tmp@0.0;"), parser_x);
  parseAndSetConstants(
    toStringOrDefault(prob_parm->ic_y_dict, "tmp@0.0;"), parser_y);
  parseAndSetConstants(
    toStringOrDefault(prob_parm->ic_z_dict, "tmp@0.0;"), parser_z);
  parseAndSetConstants(
    toStringOrDefault(prob_parm->ic_press_dict, "tmp@0.0;"), parser_press);
  parseAndSetConstants(
    toStringOrDefault(prob_parm->ic_temp_dict, "tmp@0.0;"), parser_temp);
  parseAndSetConstants(
    toStringOrDefault(prob_parm->ic_velx_dict, "tmp@0.0;"), parser_velx);
  parseAndSetConstants(
    toStringOrDefault(prob_parm->ic_vely_dict, "tmp@0.0;"), parser_vely);
  parseAndSetConstants(
    toStringOrDefault(prob_parm->ic_velz_dict, "tmp@0.0;"), parser_velz);

  amrex::Vector<std::string> names;
  pele::physics::eos::speciesNames<pele::physics::PhysicsType::eos_type>(names);
  std::unordered_map<std::string, int> species_ids;
  species_ids.reserve(names.size());
  for (int n = 0; n < static_cast<int>(names.size()); ++n) {
    species_ids[names[n]] = n;
  }

  parseAndPackSpeciesDict(
    toStringOrDefault(prob_parm->ic_spec_dict, "air@O2:0.233|N2:0.767;"),
    parser_spec,
    species_ids,
    prob_parm->ic_num_mixtures,
    prob_parm->ic_mix_species_count,
    prob_parm->ic_mix_species_id,
    prob_parm->ic_mix_species_val,
    "prob.ic_spec_dict");

  if (prob_parm->ic_num_mixtures == 0) {
    prob_parm->ic_num_mixtures = 1;
    prob_parm->ic_mix_species_count[0] = 1;
    prob_parm->ic_mix_species_id[0] = 0;
    prob_parm->ic_mix_species_val[0] = 1.0_rt;
  }

  parser_x.registerVariables({"xlo", "xhi"});
  parser_y.registerVariables({"ylo", "yhi"});
  parser_z.registerVariables({"zlo", "zhi"});
  parser_press.registerVariables({"x", "y", "z"});
  parser_temp.registerVariables({"x", "y", "z"});
  parser_spec.registerVariables({"x", "y", "z"});
  parser_velx.registerVariables({"x", "y", "z"});
  parser_vely.registerVariables({"x", "y", "z"});
  parser_velz.registerVariables({"x", "y", "z"});

  prob_parm->ic_center_x = parser_x.compile<2>();
  prob_parm->ic_center_y = parser_y.compile<2>();
  prob_parm->ic_center_z = parser_z.compile<2>();
  prob_parm->ic_press = parser_press.compile<3>();
  prob_parm->ic_temp = parser_temp.compile<3>();
  prob_parm->ic_spec = parser_spec.compile<3>();
  prob_parm->ic_velx = parser_velx.compile<3>();
  prob_parm->ic_vely = parser_vely.compile<3>();
  prob_parm->ic_velz = parser_velz.compile<3>();

  struct BCFaceInput
  {
    const char* press_expr;
    const char* press_dict;
    const char* temp_expr;
    const char* temp_dict;
    const char* spec_expr;
    const char* spec_dict;
    const char* velx_expr;
    const char* velx_dict;
    const char* vely_expr;
    const char* vely_dict;
    const char* velz_expr;
    const char* velz_dict;
    const char* label;
  };

  const BCFaceInput bc_faces[ProbParm::NUM_BC_FACES] = {
    {prob_parm->bc_xlo_press_expr,
     prob_parm->bc_xlo_press_dict,
     prob_parm->bc_xlo_temp_expr,
     prob_parm->bc_xlo_temp_dict,
     prob_parm->bc_xlo_spec_expr,
     prob_parm->bc_xlo_spec_dict,
     prob_parm->bc_xlo_velx_expr,
     prob_parm->bc_xlo_velx_dict,
     prob_parm->bc_xlo_vely_expr,
     prob_parm->bc_xlo_vely_dict,
     prob_parm->bc_xlo_velz_expr,
     prob_parm->bc_xlo_velz_dict,
     "prob.bc_xlo_spec_dict"},
    {prob_parm->bc_xhi_press_expr,
     prob_parm->bc_xhi_press_dict,
     prob_parm->bc_xhi_temp_expr,
     prob_parm->bc_xhi_temp_dict,
     prob_parm->bc_xhi_spec_expr,
     prob_parm->bc_xhi_spec_dict,
     prob_parm->bc_xhi_velx_expr,
     prob_parm->bc_xhi_velx_dict,
     prob_parm->bc_xhi_vely_expr,
     prob_parm->bc_xhi_vely_dict,
     prob_parm->bc_xhi_velz_expr,
     prob_parm->bc_xhi_velz_dict,
     "prob.bc_xhi_spec_dict"},
    {prob_parm->bc_ylo_press_expr,
     prob_parm->bc_ylo_press_dict,
     prob_parm->bc_ylo_temp_expr,
     prob_parm->bc_ylo_temp_dict,
     prob_parm->bc_ylo_spec_expr,
     prob_parm->bc_ylo_spec_dict,
     prob_parm->bc_ylo_velx_expr,
     prob_parm->bc_ylo_velx_dict,
     prob_parm->bc_ylo_vely_expr,
     prob_parm->bc_ylo_vely_dict,
     prob_parm->bc_ylo_velz_expr,
     prob_parm->bc_ylo_velz_dict,
     "prob.bc_ylo_spec_dict"},
    {prob_parm->bc_yhi_press_expr,
     prob_parm->bc_yhi_press_dict,
     prob_parm->bc_yhi_temp_expr,
     prob_parm->bc_yhi_temp_dict,
     prob_parm->bc_yhi_spec_expr,
     prob_parm->bc_yhi_spec_dict,
     prob_parm->bc_yhi_velx_expr,
     prob_parm->bc_yhi_velx_dict,
     prob_parm->bc_yhi_vely_expr,
     prob_parm->bc_yhi_vely_dict,
     prob_parm->bc_yhi_velz_expr,
     prob_parm->bc_yhi_velz_dict,
     "prob.bc_yhi_spec_dict"},
    {prob_parm->bc_zlo_press_expr,
     prob_parm->bc_zlo_press_dict,
     prob_parm->bc_zlo_temp_expr,
     prob_parm->bc_zlo_temp_dict,
     prob_parm->bc_zlo_spec_expr,
     prob_parm->bc_zlo_spec_dict,
     prob_parm->bc_zlo_velx_expr,
     prob_parm->bc_zlo_velx_dict,
     prob_parm->bc_zlo_vely_expr,
     prob_parm->bc_zlo_vely_dict,
     prob_parm->bc_zlo_velz_expr,
     prob_parm->bc_zlo_velz_dict,
     "prob.bc_zlo_spec_dict"},
    {prob_parm->bc_zhi_press_expr,
     prob_parm->bc_zhi_press_dict,
     prob_parm->bc_zhi_temp_expr,
     prob_parm->bc_zhi_temp_dict,
     prob_parm->bc_zhi_spec_expr,
     prob_parm->bc_zhi_spec_dict,
     prob_parm->bc_zhi_velx_expr,
     prob_parm->bc_zhi_velx_dict,
     prob_parm->bc_zhi_vely_expr,
     prob_parm->bc_zhi_vely_dict,
     prob_parm->bc_zhi_velz_expr,
     prob_parm->bc_zhi_velz_dict,
     "prob.bc_zhi_spec_dict"}};

  for (int face = 0; face < ProbParm::NUM_BC_FACES; ++face) {
    const auto& fi = bc_faces[face];

    auto& p_press = make_parser(replacePlaceholders(
      toStringOrDefault(fi.press_expr, "101325.0"), {{"%EQ%", "="}}));
    auto& p_temp = make_parser(replacePlaceholders(
      toStringOrDefault(fi.temp_expr, "300.0"), {{"%EQ%", "="}}));
    auto& p_spec = make_parser(replacePlaceholders(
      toStringOrDefault(fi.spec_expr, "0"), {{"%EQ%", "="}}));
    auto& p_velx = make_parser(replacePlaceholders(
      toStringOrDefault(fi.velx_expr, "0.0"), {{"%EQ%", "="}}));
    auto& p_vely = make_parser(replacePlaceholders(
      toStringOrDefault(fi.vely_expr, "0.0"), {{"%EQ%", "="}}));
    auto& p_velz = make_parser(replacePlaceholders(
      toStringOrDefault(fi.velz_expr, "0.0"), {{"%EQ%", "="}}));

    parseAndSetConstants(toStringOrDefault(fi.press_dict, "tmp@0.0;"), p_press);
    parseAndSetConstants(toStringOrDefault(fi.temp_dict, "tmp@0.0;"), p_temp);
    parseAndSetConstants(toStringOrDefault(fi.velx_dict, "tmp@0.0;"), p_velx);
    parseAndSetConstants(toStringOrDefault(fi.vely_dict, "tmp@0.0;"), p_vely);
    parseAndSetConstants(toStringOrDefault(fi.velz_dict, "tmp@0.0;"), p_velz);

    int* face_counts =
      prob_parm->bc_mix_species_count + face * ProbParm::MAX_MIXTURES;
    int* face_ids =
      prob_parm->bc_mix_species_id +
      face * ProbParm::MAX_MIXTURES * ProbParm::MAX_SPECIES_PER_MIXTURE;
    amrex::Real* face_vals =
      prob_parm->bc_mix_species_val +
      face * ProbParm::MAX_MIXTURES * ProbParm::MAX_SPECIES_PER_MIXTURE;
    int& face_num_mix = prob_parm->bc_num_mixtures[face];

    parseAndPackSpeciesDict(
      toStringOrDefault(fi.spec_dict, "air@O2:0.233|N2:0.767;"),
      p_spec,
      species_ids,
      face_num_mix,
      face_counts,
      face_ids,
      face_vals,
      fi.label);

    if (face_num_mix == 0) {
      face_num_mix = 1;
      face_counts[0] = 1;
      face_ids[0] = 0;
      face_vals[0] = 1.0_rt;
    }

    p_press.registerVariables({"x", "y", "z", "time"});
    p_temp.registerVariables({"x", "y", "z", "time"});
    p_spec.registerVariables({"x", "y", "z", "time"});
    p_velx.registerVariables({"x", "y", "z", "time"});
    p_vely.registerVariables({"x", "y", "z", "time"});
    p_velz.registerVariables({"x", "y", "z", "time"});

    prob_parm->bc_press[face] = p_press.compile<4>();
    prob_parm->bc_temp[face] = p_temp.compile<4>();
    prob_parm->bc_spec[face] = p_spec.compile<4>();
    prob_parm->bc_velx[face] = p_velx.compile<4>();
    prob_parm->bc_vely[face] = p_vely.compile<4>();
    prob_parm->bc_velz[face] = p_velz.compile<4>();
  }

  prob_parm->ic_runtime_ready = 1;
  prob_parm->bc_runtime_ready = 1;
}
