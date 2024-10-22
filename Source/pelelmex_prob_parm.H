#ifndef PELELM_PROB_PARM_H
#define PELELM_PROB_PARM_H

#include <AMReX_REAL.H>
#include <AMReX_GpuMemory.H>
#include <AMReX_Parser.H>

using namespace amrex::literals;

struct ProbParm
{
  // Placeholder
  amrex::Real P_mean = 101325.0_rt;

  // Initial condition expressions and dictionaries
  // ************************************************************
  // Placeholder for coordinate expressions
  char* ic_x_expr = nullptr;
  char* ic_y_expr = nullptr;
  char* ic_z_expr = nullptr;
  char* ic_x_dict = nullptr;
  char* ic_y_dict = nullptr;
  char* ic_z_dict = nullptr;
  // Pressure expression and dictionary character arrays
  char* ic_press_expr = nullptr;
  char* ic_press_dict = nullptr;
  // Temperature expression and dictionary character arrays
  char* ic_temp_expr = nullptr;
  char* ic_temp_dict = nullptr;
  // Velocity expressions and dictionary character arrays
  char* ic_velx_expr = nullptr;
  char* ic_velx_dict = nullptr;
  char* ic_vely_expr = nullptr;
  char* ic_vely_dict = nullptr;
  char* ic_velz_expr = nullptr;
  char* ic_velz_dict = nullptr;
  // Species expressions and dictionary character arrays
  char* ic_spec_expr = nullptr;
  char* ic_spec_dict = nullptr;
  // ************************************************************

  // Boundary condition expressions and dictionaries
  // ************************************************************
  // X-Lo boundary condition expressions and dictionaries
  char* bc_xlo_press_expr = nullptr;
  char* bc_xlo_press_dict = nullptr;
  char* bc_xlo_temp_expr = nullptr;
  char* bc_xlo_temp_dict = nullptr;
  char* bc_xlo_spec_expr = nullptr;
  char* bc_xlo_spec_dict = nullptr;
  char* bc_xlo_velx_expr = nullptr;
  char* bc_xlo_velx_dict = nullptr;
  char* bc_xlo_vely_expr = nullptr;
  char* bc_xlo_vely_dict = nullptr;
  char* bc_xlo_velz_expr = nullptr;
  char* bc_xlo_velz_dict = nullptr;

  // X-Hi boundary condition expressions and dictionaries
  char* bc_xhi_press_expr = nullptr;
  char* bc_xhi_press_dict = nullptr;
  char* bc_xhi_temp_expr = nullptr;
  char* bc_xhi_temp_dict = nullptr;
  char* bc_xhi_spec_expr = nullptr;
  char* bc_xhi_spec_dict = nullptr;
  char* bc_xhi_velx_expr = nullptr;
  char* bc_xhi_velx_dict = nullptr;
  char* bc_xhi_vely_expr = nullptr;
  char* bc_xhi_vely_dict = nullptr;
  char* bc_xhi_velz_expr = nullptr;
  char* bc_xhi_velz_dict = nullptr;

  // Y-Lo boundary condition expressions and dictionaries
  char* bc_ylo_press_expr = nullptr;
  char* bc_ylo_press_dict = nullptr;
  char* bc_ylo_temp_expr = nullptr;
  char* bc_ylo_temp_dict = nullptr;
  char* bc_ylo_spec_expr = nullptr;
  char* bc_ylo_spec_dict = nullptr;
  char* bc_ylo_velx_expr = nullptr;
  char* bc_ylo_velx_dict = nullptr;
  char* bc_ylo_vely_expr = nullptr;
  char* bc_ylo_vely_dict = nullptr;
  char* bc_ylo_velz_expr = nullptr;
  char* bc_ylo_velz_dict = nullptr;

  // Y-Hi boundary condition expressions and dictionaries
  char* bc_yhi_press_expr = nullptr;
  char* bc_yhi_press_dict = nullptr;
  char* bc_yhi_temp_expr = nullptr;
  char* bc_yhi_temp_dict = nullptr;
  char* bc_yhi_spec_expr = nullptr;
  char* bc_yhi_spec_dict = nullptr;
  char* bc_yhi_velx_expr = nullptr;
  char* bc_yhi_velx_dict = nullptr;
  char* bc_yhi_vely_expr = nullptr;
  char* bc_yhi_vely_dict = nullptr;
  char* bc_yhi_velz_expr = nullptr;
  char* bc_yhi_velz_dict = nullptr;

  // Z-Lo boundary condition expressions and dictionaries
  char* bc_zlo_press_expr = nullptr;
  char* bc_zlo_press_dict = nullptr;
  char* bc_zlo_temp_expr = nullptr;
  char* bc_zlo_temp_dict = nullptr;
  char* bc_zlo_spec_expr = nullptr;
  char* bc_zlo_spec_dict = nullptr;
  char* bc_zlo_velx_expr = nullptr;
  char* bc_zlo_velx_dict = nullptr;
  char* bc_zlo_vely_expr = nullptr;
  char* bc_zlo_vely_dict = nullptr;
  char* bc_zlo_velz_expr = nullptr;
  char* bc_zlo_velz_dict = nullptr;

  // Z-Hi boundary condition expressions and dictionaries
  char* bc_zhi_press_expr = nullptr;
  char* bc_zhi_press_dict = nullptr;
  char* bc_zhi_temp_expr = nullptr;
  char* bc_zhi_temp_dict = nullptr;
  char* bc_zhi_spec_expr = nullptr;
  char* bc_zhi_spec_dict = nullptr;
  char* bc_zhi_velx_expr = nullptr;
  char* bc_zhi_velx_dict = nullptr;
  char* bc_zhi_vely_expr = nullptr;
  char* bc_zhi_vely_dict = nullptr;
  char* bc_zhi_velz_expr = nullptr;
  char* bc_zhi_velz_dict = nullptr;
  // ************************************************************
};

// Function to replace placeholders in the given string
AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
std::string
replacePlaceholders(
  std::string input_str,
  const std::unordered_map<std::string, std::string>& replacements)
{
  for (const auto& [placeholder, replacement] : replacements) {
    size_t pos = 0;
    while ((pos = input_str.find(placeholder, pos)) != std::string::npos) {
      input_str.replace(pos, placeholder.length(), replacement);
      pos += replacement.length();
    }
  }
  return input_str;
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void
parseAndSetConstants(const std::string& dict_str, amrex::Parser& parser)
{
  if (!dict_str.empty()) {
    size_t start = 0;
    size_t end = dict_str.find(';');
    while (end != std::string::npos) {
      std::string pair_str = dict_str.substr(start, end - start);

      // Split by '@' to get key and value
      size_t separator = pair_str.find('@');
      if (separator != std::string::npos) {
        std::string key = pair_str.substr(0, separator);
        double value = std::stod(pair_str.substr(separator + 1));

        // Set the key-value pair in the parser as a constant
        parser.setConstant(key, value);
      }

      start = end + 1;
      end = dict_str.find(';', start);
    }

    // Handle the last pair (if any)
    std::string last_pair_str = dict_str.substr(start);
    size_t separator = last_pair_str.find('@');
    if (separator != std::string::npos) {
      std::string key = last_pair_str.substr(0, separator);
      double value = std::stod(last_pair_str.substr(separator + 1));
      parser.setConstant(key, value);
    }
  }
}

AMREX_GPU_DEVICE
AMREX_FORCE_INLINE
void
parseAndSetConstantsAndSpecies(
  const std::string& dict_str,
  amrex::Parser& parser,
  std::unordered_map<std::string, std::map<std::string, double>>& species_map,
  std::vector<std::string>& mixture_names)
{
  if (!dict_str.empty()) {
    size_t start = 0;
    size_t end = dict_str.find(';');
    while (end != std::string::npos) {
      std::string pair_str = dict_str.substr(start, end - start);

      // Split by '@' to get key and value
      size_t separator = pair_str.find('@');
      if (separator != std::string::npos) {
        std::string key = pair_str.substr(0, separator);
        std::string value_str = pair_str.substr(separator + 1);

        // Check if the value contains '|', indicating species information
        if (value_str.find('|') != std::string::npos) {
          // Parse species information
          std::istringstream species_stream(value_str);
          std::string species_entry;
          while (std::getline(species_stream, species_entry, '|')) {
            size_t colon_pos = species_entry.find(':');
            if (colon_pos != std::string::npos) {
              std::string species_name = species_entry.substr(0, colon_pos);
              double fraction = std::stod(species_entry.substr(colon_pos + 1));
              species_map[key][species_name] = fraction;
            }
          }
          mixture_names.push_back(key); // Store the mixture name for indexing
        } else {
          // Parse as a normal variable and set as a constant
          double value = std::stod(value_str);
          parser.setConstant(key, value);
        }
      }

      start = end + 1;
      end = dict_str.find(';', start);
    }

    // Handle the last pair (if any)
    std::string last_pair_str = dict_str.substr(start);
    size_t separator = last_pair_str.find('@');
    if (separator != std::string::npos) {
      std::string key = last_pair_str.substr(0, separator);
      std::string value_str = last_pair_str.substr(separator + 1);

      // Check if the value contains '|', indicating species information
      if (value_str.find('|') != std::string::npos) {
        // Parse species information
        std::istringstream species_stream(value_str);
        std::string species_entry;
        while (std::getline(species_stream, species_entry, '|')) {
          size_t colon_pos = species_entry.find(':');
          if (colon_pos != std::string::npos) {
            std::string species_name = species_entry.substr(0, colon_pos);
            double fraction = std::stod(species_entry.substr(colon_pos + 1));
            species_map[key][species_name] = fraction;
          }
        }
        mixture_names.push_back(key); // Store the mixture name for indexing
      } else {
        // Parse as a normal variable and set as a constant
        double value = std::stod(value_str);
        parser.setConstant(key, value);
      }
    }
  }
}

#endif
