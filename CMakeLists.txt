cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(PeleLMeX LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(CMakePackageConfigHelpers)
include(PeleUtils)
include(peleFunctions)

# if(NOT DEFINED CMAKE_CXX_STANDARD)
#     set(CMAKE_CXX_STANDARD 17)
# endif()
# if(NOT DEFINED CMAKE_CXX_EXTENSIONS)
#     set(CMAKE_CXX_EXTENSIONS OFF)
# endif()
# if(NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
#     set(CMAKE_CXX_STANDARD_REQUIRED ON)
# endif()

# if(NOT DEFINED CMAKE_CUDA_STANDARD)
#     set(CMAKE_CUDA_STANDARD 17)
# endif()
# if(NOT DEFINED CMAKE_CUDA_EXTENSIONS)
#     set(CMAKE_CUDA_EXTENSIONS OFF)
# endif()
# if(NOT DEFINED CMAKE_CUDA_STANDARD_REQUIRED)
#     set(CMAKE_CUDA_STANDARD_REQUIRED ON)
# endif()

# # Output Dircetories
# set_default_build_dirs()
# set_default_install_dirs()
enable_testing()
include(CTest)

# General options
include(CMakeDependentOption)
set(PELE_DIM "3" CACHE STRING "Number of physical dimensions")
option(PELE_ENABLE_CLANG_TIDY "Enable clang-tidy analysis" OFF)
option(PELE_ENABLE_CPPCHECK "Enable cppcheck analysis" OFF)
option(PELE_ENABLE_FCOMPARE "Enable building fcompare when not testing" OFF)
option(PELE_ENABLE_FCOMPARE_FOR_TESTS "Check test plots against gold files" OFF)
option(PELE_SAVE_GOLDS "Enable saving of gold files to a specified directory" OFF)
option(PELE_ENABLE_SANITIZE_FOR_TESTS "Currently only disables certain long running MMS tests if set" OFF)
option(PELE_ENABLE_FPE_TRAP_FOR_TESTS "Enable FPE trapping in tests" ON)
option(PELE_ENABLE_TINY_PROFILE "Enable tiny profiler in AMReX" OFF)
option(PELE_ENABLE_HDF5 "Enable plot file output using HDF5" OFF)
option(PELE_ENABLE_HDF5_ZFP "Enable ZFP compression in HDF5" OFF)
option(PELE_ENABLE_ASCENT "Enable Ascent in-situ visualization" OFF)
option(PELE_ENABLE_HYPRE "Enable Offload to Hypre" OFF)
set(PELE_PRECISION "DOUBLE" CACHE STRING "Floating point precision SINGLE or DOUBLE")

# Physics options
option(PELE_ENABLE_EB "Enable Embedded Boundary" OFF)
option(PELE_ENABLE_PARTICLES "Enable particles and spray" ON)

# HPC options
option(PELE_ENABLE_MPI "Enable MPI" OFF)
option(PELE_ENABLE_OPENMP "Enable OpenMP" OFF)
option(PELE_ENABLE_CUDA "Enable CUDA" OFF)
option(PELE_ENABLE_HIP "Enable HIP" OFF)
option(PELE_ENABLE_SYCL "Enable SyCL" OFF)

# Python options
option(PELE_PYTHON "Python bindings" OFF)
option(PELE_LIB "Build PeleLMeX as a library" OFF)

if(PELE_PYTHON)
    set(PELE_LIB ON CACHE STRING "Build PeleLMeX as a library" FORCE)
endif()

# Options that are only necessary for when we are doing the superbuild for pyPeleLMeX
if (PELE_PYTHON)
  set(PELE_PHYSICS_EOS_MODEL "Fuego" CACHE STRING "Equation of state model")
  set(PELE_PHYSICS_TRANSPORT_MODEL "Simple" CACHE STRING "Transport model")
  set(PELE_PHYSICS_CHEMISTRY_MODEL "drm19" CACHE STRING "Chemistry model")
endif()

# C++ Options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(PELE_ENABLE_CUDA)
  enable_language(CUDA)
  if(CMAKE_CUDA_COMPILER_VERSION VERSION_LESS "10.0")
    message(FATAL_ERROR "Your nvcc version is ${CMAKE_CUDA_COMPILER_VERSION} which is unsupported."
      "Please use CUDA toolkit version 10.0 or newer.")
  endif()
  if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
    set(CMAKE_CUDA_ARCHITECTURES 70)
  endif()
endif()

if(PELE_ENABLE_HIP)
   enable_language(HIP)
endif ()

if(PELE_DIM EQUAL 1)
    message(FATAL_ERROR "${PROJECT_NAME} does not support 1D.")
endif()

if (PELE_LIB)
  # Output Dircetories
  set_default_build_dirs()
  set_default_install_dirs()

  # set(PeleLMeX_DIMS_VALUES 2 3 RZ)
  set(PeleLMeX_DIMS_VALUES 2 3)
  set(PeleLMeX_DIMS 2 3 CACHE STRING "Dimensions to build for")
  list(REMOVE_DUPLICATES PeleLMeX_DIMS)
  foreach(D IN LISTS PeleLMeX_DIMS)
    if(NOT D IN_LIST PeleLMeX_DIMS_VALUES)
      message(FATAL_ERROR "Invalid dimension specified: ${D}")
    endif()
  endforeach()

  list(LENGTH PeleLMeX_DIMS list_len)
  math(EXPR list_last "${list_len} - 1")
  list(GET PeleLMeX_DIMS ${list_last} PeleLMeX_DIMS_LAST)
  pelelmex_set_suffix_dims(PeleLMeX_DIMS_LAST ${PeleLMeX_DIMS_LAST})

  set_default_build_type("Release")
endif()



########################### AMReX #####################################
message(STATUS "AMReX Configure Section")
set(AMREX_SUBMOD_LOCATION "${CMAKE_SOURCE_DIR}/Submodules/PelePhysics/Submodules/amrex")
include(SetAmrexOptions)


if (PELE_LIB)
  option(PeleLMeX_amrex_internal                    "Download & build AMReX" OFF)
  set(PeleLMeX_COMPUTE "NONE" CACHE STRING "AMReX compute type")
  set(PeleLMeX_amrex_dim "${PELE_DIM}" CACHE STRING "AMReX dimension")

  set(PeleLMeX_amrex_src ${AMREX_SUBMOD_LOCATION})


  message(STATUS "Compiling local AMReX ...")
  message(STATUS "AMReX source path: ${AMREX_SUBMOD_LOCATION}")
  if (NOT IS_DIRECTORY ${AMREX_SUBMOD_LOCATION})
    message(FATAL_ERROR "Specified directory AMREX_SUBMOD_LOCATION='${AMREX_SUBMOD_LOCATION}' does not exist!")
  endif()
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
  list(APPEND CMAKE_MODULE_PATH "${AMREX_SUBMOD_LOCATION}/Tools/CMake")

  set(AMReX_PIC ON CACHE INTERNAL "" FORCE)
  set(AMReX_BUILD_SHARED_LIBS ON CACHE BOOL "Build AMReX shared library" FORCE)
  set(AMReX_INSTALL ${AMReX_BUILD_SHARED_LIBS} CACHE INTERNAL "Generate Install Targets" FORCE)
  
  set(PeleLMeX_amrex_dim ${PeleLMeX_DIMS})
  list(TRANSFORM PeleLMeX_amrex_dim REPLACE RZ 2)
  list(REMOVE_DUPLICATES PeleLMeX_amrex_dim)
  set(AMReX_SPACEDIM ${PeleLMeX_amrex_dim} CACHE INTERNAL "")

  
  list(APPEND CMAKE_MODULE_PATH "${PeleLMeX_amrex_src}/Tools/CMake")
  add_subdirectory(${PeleLMeX_amrex_src} _deps/localamrex-build/)
  include(SetAmrexCompileFlags)
  message(STATUS "AMReX: Using version '${AMREX_PKG_VERSION}' (${AMREX_GIT_VERSION})")
  list(APPEND CMAKE_MODULE_PATH "${AMReX_DIR}/AMReXCMakeModules")


  foreach(D IN LISTS PeleLMeX_amrex_dim)
    #   suppress warnings in AMReX headers (use -isystem instead of -I)
    pelelmex_make_third_party_includes_system(AMReX::amrex_${D} amrex_${D}d)
  endforeach()
  # For <AMReX_buildInfo.H>
  include(AMReXBuildInfo)

  add_library(PeleLMeX::thirdparty::amrex_${PELE_DIM}d INTERFACE IMPORTED)
  target_link_libraries(PeleLMeX::thirdparty::amrex_${PELE_DIM}d INTERFACE AMReX::amrex_${PELE_DIM}d)


else()
  list(APPEND CMAKE_MODULE_PATH "${AMREX_SUBMOD_LOCATION}/Tools/CMake")
  add_subdirectory(${AMREX_SUBMOD_LOCATION})
  include(AMReXBuildInfo)
  include(SetAmrexCompileFlags)
endif()


########################### AMReX-Hydro ###############################
message(STATUS "AMReX-Hydro Configure Section")
set(AMREX_HYDRO_SUBMOD_LOCATION "${CMAKE_SOURCE_DIR}/Submodules/AMReX-Hydro")
include(SetAmrexHydroOptions)

if (PELE_LIB)
  add_subdirectory(${AMREX_HYDRO_SUBMOD_LOCATION} _deps/localamrexhydro-build/)
  include(AMReXBuildInfo)
else()
  add_subdirectory(${AMREX_HYDRO_SUBMOD_LOCATION})
endif()


# ########################### SUNDIALS ##################################
message(STATUS "Sundials Configure Section")
set(SUNDIALS_SUBMOD_LOCATION "${CMAKE_SOURCE_DIR}/Submodules/PelePhysics/Submodules/sundials")
include(SetSundialsOptions)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
if (PELE_LIB)
  add_subdirectory(${SUNDIALS_SUBMOD_LOCATION} _deps/localsundials-build/)
  include(AMReXBuildInfo)
else()
  add_subdirectory(${SUNDIALS_SUBMOD_LOCATION})
endif()


if (PELE_PYTHON)
  find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

  set_default_install_dirs_python()

  # pybind11
  include(${PeleLMeX_SOURCE_DIR}/cmake/dependencies/pybind11.cmake)


  # pyAMReX
  include(${PeleLMeX_SOURCE_DIR}/cmake/dependencies/pyAMReX.cmake)


endif()

########################### PeleLMeX ##################################
if(PELE_ENABLE_MPI)
  message(STATUS "MPI Configure Section")
  find_package(MPI REQUIRED CXX)
endif()

# General information about machine, compiler, and build type
message(STATUS "${PROJECT_NAME} Information:")
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION = ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "PELE_PRECISION = ${PELE_PRECISION}")
message(STATUS "PELE_PYTHON = ${PELE_PYTHON}")
message(STATUS "PELE_LIB = ${PELE_LIB}")

init_code_checks()

include(SetRpath)

# enable_testing()
# include(CTest)
add_subdirectory(Exec)
add_subdirectory(Tests)
add_subdirectory(${CMAKE_SOURCE_DIR}/Submodules/PelePhysics/Testing/Exec/Radiation)



# Targets for installation
if (PELE_LIB)

  option(PeleLMeX_amrex_internal                    "Download & build AMReX" OFF)
  set(PeleLMeX_COMPUTE "NONE" CACHE STRING "AMReX compute type")
  set(PeleLMeX_amrex_dim "${PELE_DIM}" CACHE STRING "AMReX dimension")

  set(PeleLMeX_amrex_src ${AMREX_SUBMOD_LOCATION})

  set(PeleLMeX_amrex_dim ${PeleLMeX_DIMS})
  list(TRANSFORM PeleLMeX_amrex_dim REPLACE RZ 2)
  list(REMOVE_DUPLICATES PeleLMeX_amrex_dim)
  set(AMReX_SPACEDIM ${PeleLMeX_amrex_dim} CACHE INTERNAL "")

  set(PELE_PHYSICS_EOS_MODEL Fuego)
  set(PELE_PHYSICS_CHEMISTRY_MODEL air)
  set(PELE_PHYSICS_TRANSPORT_MODEL Constant)
  set(PELE_PHYSICS_ENABLE_SPRAY OFF)
  set(PELE_PHYSICS_SPRAY_FUEL_NUM 0)
  set(PELE_PHYSICS_ENABLE_SOOT OFF)
  set(PELE_PHYSICS_ENABLE_RADIATION OFF)

  include(BuildExeAndLib)

endif()

if (PELE_PYTHON)
  find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)

  set_default_install_dirs_python()

  # pybind11
  include(${PeleLMeX_SOURCE_DIR}/cmake/dependencies/pybind11.cmake)


  # pyAMReX
  include(${PeleLMeX_SOURCE_DIR}/cmake/dependencies/pyAMReX.cmake)


endif()


if (PELE_PYTHON)

  set(SD ${PELE_DIM}d)
  target_include_directories(${pele_exe_name}
    PRIVATE
      ${PeleLMeX_SOURCE_DIR}/Source
      ${PeleLMeX_BINARY_DIR}/Source
  )
  

  add_library(pyPeleLMeX MODULE 
    Source/Python/pyPeleLMeX.cpp
    Source/pelelmex_prob.H
  )


  add_library(PeleLMeX::pyPeleLMeX ALIAS pyPeleLMeX)
  target_link_libraries(pyPeleLMeX PUBLIC ${pele_exe_name})
  set(_BUILDINFO_SRC pyPeleLMeX)
  list(APPEND _ALL_TARGETS pyPeleLMeX)

  include_directories(${PeleLMeX_SOURCE_DIR}/Source)

  # add_library(pyPeleLMeX SHARED ${PYTHON_SOURCE_DIR}/PeleLMeX.cpp)
  target_include_directories(pyPeleLMeX PRIVATE ${PeleLMeX_SOURCE_DIR}/Source)


  set_target_properties(pyPeleLMeX PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    CUDA_VISIBILITY_PRESET "hidden"
    ARCHIVE_OUTPUT_NAME pelelmex_pybind
    LIBRARY_OUTPUT_NAME pelelmex_pybind
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/pypelelmex
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/pypelelmex
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/pypelelmex
    PDB_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/pypelelmex
    COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/pypelelmex
  )
  get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if(isMultiConfig)
      foreach(CFG IN LISTS CMAKE_CONFIGURATION_TYPES)
          string(TOUPPER "${CFG}" CFG_UPPER)
          set_target_properties(pyPeleLMeX PROPERTIES
              # build output directories - mainly set to run tests from CMake & IDEs
              # note: same as above, but for Multi-Config generators
              ARCHIVE_OUTPUT_DIRECTORY_${CFG_UPPER} ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/pypelelmex
              LIBRARY_OUTPUT_DIRECTORY_${CFG_UPPER} ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/pypelelmex
              RUNTIME_OUTPUT_DIRECTORY_${CFG_UPPER} ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/pypelelmex
              PDB_OUTPUT_DIRECTORY_${CFG_UPPER} ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/pypelelmex
              COMPILE_PDB_OUTPUT_DIRECTORY_${CFG_UPPER} ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/pypelelmex
          )
      endforeach()
  endif()
  if(EMSCRIPTEN)
      set_target_properties(pyPeleLMeX PROPERTIES
          PREFIX "")
  else()
    pybind11_extension(pyPeleLMeX)
  endif()
  if(NOT MSVC AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
      pybind11_strip(pyPeleLMeX)
  endif()

  generate_buildinfo(${_BUILDINFO_SRC} "${PeleLMeX_SOURCE_DIR}")
  target_link_libraries(amrex_${SD} PRIVATE buildInfo::${_BUILDINFO_SRC})
  target_link_libraries(pyPeleLMeX PRIVATE buildInfo::${_BUILDINFO_SRC})
  unset(_BUILDINFO_SRC)

  target_sources(${pele_exe_name} PRIVATE Source/PeleLMeX.cpp)


  # # # This might only be needed for globals (from warpx implementation...)
  # generate_export_header(amrex_${SD}
  #   BASE_NAME AMReX
  #   EXPORT_FILE_NAME Source/AMReX_Export.H
  # )

  # generate_export_header(${pele_exe_name}
  #   BASE_NAME PeleLMeX
  #   EXPORT_FILE_NAME Source/PeleLMeX_Export.H
  # )

  # target_compile_definitions(amrex_${SD} PRIVATE amrex_${SD}_EXPORTS)
  target_compile_definitions(${pele_exe_name} PRIVATE PELELMEX_EXPORTS)
  target_compile_definitions(pyPeleLMeX PRIVATE PYPELELMEX_EXPORTS)
  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(amrex_${SD} PRIVATE AMREX_STATIC_DEFINE)
    target_compile_definitions(${pele_exe_name} PRIVATE PELELMEX_STATIC_DEFINE)
    target_compile_definitions(pyPeleLMeX PRIVATE PYPELELMEX_STATIC_DEFINE)
  endif()
  
  if (PELE_PYTHON)
    add_custom_command(TARGET pyPeleLMeX POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${PeleLMeX_SOURCE_DIR}/Python/pypelelmex
      $<TARGET_FILE_DIR:pyPeleLMeX>
    )
  endif()

  add_subdirectory(Source/Python)

  # link dependencies
  include(AMReXBuildInfo)
  generate_buildinfo(${pele_exe_name} ${CMAKE_SOURCE_DIR})
  # target_link_libraries(${pele_exe_name} PUBLIC ${pele_physics_lib_name} AMReX-Hydro::amrex_hydro_api AMReX::amrex)
  target_link_libraries(${pele_exe_name} PUBLIC ${pele_physics_lib_name})
  target_link_libraries(${pele_exe_name} PUBLIC pybind11::module)
  target_link_libraries(pyPeleLMeX PUBLIC pybind11::module)
  target_link_libraries(pyPeleLMeX PUBLIC PeleLMeX::thirdparty::amrex_3d)

  foreach(pelelmex_tgt IN LISTS _ALL_TARGETS)
    target_compile_features(${pelelmex_tgt} PUBLIC cxx_std_17)
  endforeach()
  set_target_properties(${_ALL_TARGETS} PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
  )

  set_pelelmex_binary_name(${PELE_DIM})

  # Defines
  foreach(D IN LISTS PeleLMeX_DIMS)
    target_compile_definitions(amrex_${D}d PUBLIC PELELMEX_DIM_${PELE_DIM}D)
    target_compile_definitions(pyPeleLMeX PRIVATE PYPELELMEX_VERSION_INFO=${PELELMEX_VERSION_INFO})
    target_compile_definitions(${pele_exe_name} PRIVATE PELELMEX_DIM_${PELE_DIM}D)
    target_compile_definitions(${pele_exe_name} PRIVATE PELELMEX_DIM_${PELE_DIM}D)
    target_compile_definitions(pyPeleLMeX PRIVATE PYPELELMEX_VERSION_INFO=${PELELMEX_VERSION_INFO})
  endforeach()


  # Installs
  set(PeleLMeX_INSTALL_TARGET_NAMES)
  foreach(D IN LISTS PeleLMeX_DIMS)
    list(APPEND PeleLMeX_INSTALL_TARGET_NAMES amrex_${D}d)
    list(APPEND PeleLMeX_INSTALL_TARGET_NAMES ${pele_physics_lib_name})
    list(APPEND PeleLMeX_INSTALL_TARGET_NAMES ${pele_exe_name})
    list(APPEND PeleLMeX_INSTALL_TARGET_NAMES pyPeleLMeX)

    install(TARGETS ${PeleLMeX_INSTALL_TARGET_NAMES}
      EXPORT ${PROJECT_NAME}Targets
      # EXPORT PeleLMeXTargets
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # simplified library alias
    if(IS_ABSOLUTE ${CMAKE_INSTALL_LIBDIR})
        set(ABS_INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}")
    else()
        set(ABS_INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    endif()
    # escape spaces for generated cmake_install.cmake file
    file(TO_CMAKE_PATH "${ABS_INSTALL_LIB_DIR}" ABS_INSTALL_LIB_DIR)

    install(CODE "file(CREATE_LINK
        $<TARGET_FILE_NAME:${pele_exe_name}>
        \"${ABS_INSTALL_LIB_DIR}/libpelelmex<TARGET_FILE_SUFFIX:${pele_exe_name}>\"
        COPY_ON_ERROR SYMBOLIC)")
  endforeach()


  # pip helpers for pypelelmex package
  if (PELE_PYTHON)
    set(PYINSTALLOPTIONS "" CACHE STRING
      "Additional parameters to pass to `pip install`")

    # ensure all targets are built before we package them in a wheel
    set(pyPeleLMeX_INSTALL_TARGET_NAMES)
    list(APPEND pyPeleLMeX_INSTALL_TARGET_NAMES pyPeleLMeX)

    # # add a prefix to custom targets so we do not collide if used as a subproject
    if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
      set(_PeleLMeX_CUSTOM_TARGET_PREFIX_DEFAULT "")
    else()
      set(_PeleLMeX_CUSTOM_TARGET_PREFIX_DEFAULT "pelelmex_")
    endif()
    set(PeleLMeX_CUSTOM_TARGET_PREFIX "${_PeleLMeX_CUSTOM_TARGET_PREFIX_DEFAULT}"
          CACHE STRING "Prefix for custom targets")

    # build the wheel by re-using the python module that we build
    add_custom_target(${PeleLMeX_CUSTOM_TARGET_PREFIX}pip_wheel
      ${CMAKE_COMMAND} -E rm -f -r pelelmex-whl
      COMMAND
          ${CMAKE_COMMAND} -E env PYPELELMEX_LIB_DIR=$<TARGET_FILE_DIR:pyPeleLMeX>
              ${Python_EXECUTABLE} -m pip wheel -v --no-build-isolation --no-deps --wheel-dir=pelelmex-whl ${PeleLMeX_SOURCE_DIR}
      WORKING_DIRECTORY
          ${PeleLMeX_BINARY_DIR}
      DEPENDS
          ${pyPeleLMeX_INSTALL_TARGET_NAMES}
    )

    # this will also upgrade/downgrade dependencies
    if(PELE_ENABLE_MPI)
      set(pyPeleLMeX_REQUIREMENT_FILE "requirements_mpi.txt")
    else()
      set(pyPeleLMeX_REQUIREMENT_FILE "requirements.txt")
    endif()
    add_custom_target(${PeleLMeX_CUSTOM_TARGET_PREFIX}pip_install_requirements
      ${Python_EXECUTABLE} -m pip install ${PYINSTALLOPTIONS} -r "${PeleLMeX_SOURCE_DIR}/${pyPeleLMeX_REQUIREMENT_FILE}"
      WORKING_DIRECTORY
          ${PeleLMeX_BINARY_DIR}
    )

    # target
    set(_EXTRA_INSTALL_DEPENDS)
    if(PeleLMeX_pyamrex_internal OR PeleLMeX_pyamrex_src)
        set(_EXTRA_INSTALL_DEPENDS pyamrex_pip_install)
    endif()

    # We force-install because in development, it is likely that the version of
    # the package does not change, but the code did change. We need --no-deps,
    # because otherwise pip would also force reinstall all dependencies.
    add_custom_target(${PeleLMeX_CUSTOM_TARGET_PREFIX}pip_install
      ${CMAKE_COMMAND} -E env PELE_ENABLE_MPI=${PELE_ENABLE_MPI}
          ${Python_EXECUTABLE} -m pip install --force-reinstall --no-index --no-deps ${PYINSTALLOPTIONS} --find-links=pelelmex-whl pypelelmex
      WORKING_DIRECTORY
          ${PeleLMeX_BINARY_DIR}
      DEPENDS
          # pyPeleLMeX_${PeleLMeX_DIMS_LAST}
          pyPeleLMeX
          ${PeleLMeX_CUSTOM_TARGET_PREFIX}pip_wheel
          ${PeleLMeX_CUSTOM_TARGET_PREFIX}pip_install_requirements
          ${_EXTRA_INSTALL_DEPENDS}
    )

    # this is for package managers only
    add_custom_target(${PeleLMeX_CUSTOM_TARGET_PREFIX}pip_install_nodeps
      ${CMAKE_COMMAND} -E env PELE_ENABLE_MPI=${PELE_ENABLE_MPI}
          ${Python_EXECUTABLE} -m pip install --force-reinstall --no-index --no-deps ${PYINSTALLOPTIONS} --find-links=pelelmex-whl pypelelmex
      WORKING_DIRECTORY
          ${PeleLMeX_BINARY_DIR}
      DEPENDS
          # pyPeleLMeX_${PeleLMeX_DIMS_LAST}
          pyPeleLMeX
          ${PeleLMeX_CUSTOM_TARGET_PREFIX}pip_wheel
    )


  endif()

endif()