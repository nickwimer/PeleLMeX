
# set names and paths of temporary build directories
# the defaults in CMake are sub-ideal for historic reasons, lets make them more
# Unix-ish and portable.
#
macro(set_default_build_dirs)
    if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                CACHE PATH "Build directory for archives")
        mark_as_advanced(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    endif()
    if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                CACHE PATH "Build directory for libraries")
        mark_as_advanced(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    endif()
    if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
                CACHE PATH "Build directory for binaries")
        mark_as_advanced(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    endif()
    if(NOT CMAKE_PYTHON_OUTPUT_DIRECTORY)
        set(CMAKE_PYTHON_OUTPUT_DIRECTORY
            "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/site-packages"
            CACHE PATH "Build directory for python modules"
        )
    endif()
endmacro()


# set names and paths of install directories
# the defaults in CMake are sub-ideal for historic reasons, lets make them more
# Unix-ish and portable.
#
macro(set_default_install_dirs)
    if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
        include(GNUInstallDirs)
        if(NOT CMAKE_INSTALL_CMAKEDIR)
            set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake"
                    CACHE PATH "CMake config package location for installed targets")
            if(WIN32)
                set(CMAKE_INSTALL_LIBDIR Lib
                        CACHE PATH "Object code libraries")
                set_property(CACHE CMAKE_INSTALL_CMAKEDIR PROPERTY VALUE "cmake")
            endif()
            mark_as_advanced(CMAKE_INSTALL_CMAKEDIR)
        endif()
    endif()

    if(WIN32)
        set(PeleLMeX_INSTALL_CMAKEDIR "${CMAKE_INSTALL_CMAKEDIR}")
    else()
        set(PeleLMeX_INSTALL_CMAKEDIR "${CMAKE_INSTALL_CMAKEDIR}/PeleLMeX")
    endif()
endmacro()



# # set names and paths for Python modules
# # this needs to be slightly delayed until we found Python and know its
# # major and minor version number
# #
macro(set_default_install_dirs_python)
    if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
        # Python install and build output dirs
        if(NOT CMAKE_INSTALL_PYTHONDIR)
            if(WIN32)
                set(CMAKE_INSTALL_PYTHONDIR_DEFAULT
                        "${CMAKE_INSTALL_LIBDIR}/site-packages"
                        )
            else()
                set(CMAKE_INSTALL_PYTHONDIR_DEFAULT
                        "${CMAKE_INSTALL_LIBDIR}/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/site-packages"
                        )
            endif()
            set(CMAKE_INSTALL_PYTHONDIR "${CMAKE_INSTALL_PYTHONDIR_DEFAULT}"
                    CACHE STRING "Location for installed python package"
                    )
        endif()
    endif()
endmacro()



# Set the suffix for targets and binaries depending on dimension
#
# User specify 1;2;RZ;D in WarpX_DIMS.
# We append to CMake targets and binaries the suffix "Nd" for 1,2,3 and "rz" for RZ.
#
macro(pelelmex_set_suffix_dims suffix dim)
    if("${dim}" STREQUAL "RZ")
        set(${suffix} rz)
    else()
        set(${suffix} ${dim}d)
    endif()
endmacro()



# # Set a feature-based binary name for the WarpX executable and create a generic
# # warpx symlink to it. Only sets options relevant for users (see summary).
# #
function(set_pelelmex_binary_name D)
    # pelelmex_set_suffix_dims(SD ${D})

    set(pelelmex_bin_names)

    if (PELE_LIB)
        list(APPEND pelelmex_bin_names ${pele_exe_name})
        # On WIN32, the OUTPUT_NAME must not collide between lib and app!
        # if(WIN32)
        #     set_target_properties(lib_${SD} PROPERTIES OUTPUT_NAME "libpelelmex")
        # else()
        set_target_properties(${pele_exe_name} PROPERTIES OUTPUT_NAME "pelelmex")
        # endif()
    endif()

    foreach(tgt IN LISTS pelelmex_bin_names)
        # set_property(TARGET ${tgt} APPEND_STRING PROPERTY OUTPUT_NAME ".${SD}")

        if(PELE_ENABLE_MPI)
            set_property(TARGET ${tgt} APPEND_STRING PROPERTY OUTPUT_NAME ".MPI")
        else()
            set_property(TARGET ${tgt} APPEND_STRING PROPERTY OUTPUT_NAME ".NOMPI")
        endif()

        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            set_property(TARGET ${tgt} APPEND_STRING PROPERTY OUTPUT_NAME ".DEBUG")
        endif()

        if(PELE_LIB)
            # alias to the latest build; this is the one expected by Python bindings
            add_custom_command(TARGET ${pele_exe_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E create_symlink
                    $<TARGET_FILE_NAME:${pele_exe_name}>
                    $<TARGET_FILE_DIR:${pele_exe_name}>/libpelelmex$<TARGET_FILE_SUFFIX:${pele_exe_name}>
                    # $<TARGET_FILE_DIR:${pele_exe_name}>/libpelelmex.${SD}$<TARGET_FILE_SUFFIX:${pele_exe_name}>
            )
        endif()
    endforeach()
endfunction()


