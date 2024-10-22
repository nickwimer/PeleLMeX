function(find_pyamrex)
    if(PeleLMeX_pyamrex_src)
        message(STATUS "Compiling local pyAMReX ...")
        message(STATUS "pyAMReX source path: ${PeleLMeX_pyamrex_src}")
        if(NOT IS_DIRECTORY ${PeleLMeX_pyamrex_src})
            message(FATAL_ERROR "Specified directory PeleLMeX_pyamrex_src='${PeleLMeX_pyamrex_src}' does not exist!")
        endif()
    elseif(PeleLMeX_pyamrex_internal)
        message(STATUS "Downloading pyAMReX ...")
        message(STATUS "pyAMReX repository: ${PeleLMeX_pyamrex_repo} (${PeleLMeX_pyamrex_branch})")
        include(FetchContent)
    endif()

    # transitive control for AMReX & pybind11 superbuild
    set(pyAMReX_amrex_internal ${PeleLMeX_amrex_internal} CACHE BOOL
        "Download & build AMReX" FORCE)
    set(pyAMReX_pybind11_internal ${PeleLMeX_pybind11_internal} CACHE BOOL
        "Download & build AMReX" FORCE)

    if(PeleLMeX_amrex_src)
        set(pyAMReX_amrex_src ${PeleLMeX_amrex_src} CACHE PATH
            "Local path to AMReX source directory (preferred if set)" FORCE)
    elseif(PeleLMeX_amrex_internal)
        if(PeleLMeX_amrex_repo)
            set(pyAMReX_amrex_repo ${PeleLMeX_amrex_repo} CACHE STRING
                "Repository URI to pull and build AMReX from if(PeleLMeX_amrex_internal)" FORCE)
        endif()
        if(PeleLMeX_amrex_branch)
            set(pyAMReX_amrex_branch ${PeleLMeX_amrex_branch} CACHE STRING
                "Repository branch for PeleLMeX_amrex_repo if(PeleLMeX_amrex_internal)" FORCE)
        endif()
    endif()

    if(PeleLMeX_pyamrex_internal OR PeleLMeX_pyamrex_src)
        set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

        if(PeleLMeX_pyamrex_src)
            add_subdirectory(${PeleLMeX_pyamrex_src} _deps/localpyamrex-build/)
        else()
            FetchContent_Declare(fetchedpyamrex
                GIT_REPOSITORY ${PeleLMeX_pyamrex_repo}
                GIT_TAG        ${PeleLMeX_pyamrex_branch}
                # GIT_REPOSITORY   "https://github.com/ax3l/pyamrex.git"
                # GIT_TAG        "fix-setup-clash-tmp-build"
                BUILD_IN_SOURCE 0
            )
            FetchContent_GetProperties(fetchedpyamrex)

            if(NOT fetchedpyamrex_POPULATED)
                FetchContent_Populate(fetchedpyamrex)
                add_subdirectory(${fetchedpyamrex_SOURCE_DIR} ${fetchedpyamrex_BINARY_DIR})
            endif()

            # advanced fetch options
            mark_as_advanced(FETCHCONTENT_BASE_DIR)
            mark_as_advanced(FETCHCONTENT_FULLY_DISCONNECTED)
            mark_as_advanced(FETCHCONTENT_QUIET)
            mark_as_advanced(FETCHCONTENT_SOURCE_DIR_FETCHEDpyamrex)
            mark_as_advanced(FETCHCONTENT_UPDATES_DISCONNECTED)
            mark_as_advanced(FETCHCONTENT_UPDATES_DISCONNECTED_FETCHEDpyamrex)
        endif()
    elseif(NOT PeleLMeX_pyamrex_internal)
        # TODO: MPI control
        find_package(pyAMReX 24.07 CONFIG REQUIRED)
        message(STATUS "pyAMReX: Found version '${pyAMReX_VERSION}'")
    endif()
endfunction()

# local source-tree
set(PeleLMeX_pyamrex_src ""
    CACHE PATH
    "Local path to pyAMReX source directory (preferred if set)")

# Git fetcher
option(PeleLMeX_pyamrex_internal "Download & build pyAMReX" ON)
set(PeleLMeX_pyamrex_repo "https://github.com/AMReX-Codes/pyamrex.git"
    CACHE STRING
    "Repository URI to pull and build pyamrex from if(PeleLMeX_pyamrex_internal)")
set(PeleLMeX_pyamrex_branch "24.07"
    CACHE STRING
    "Repository branch for PeleLMeX_pyamrex_repo if(PeleLMeX_pyamrex_internal)")

find_pyamrex()
