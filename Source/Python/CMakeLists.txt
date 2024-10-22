# set(SD ${PELE_DIM}d)

target_sources(pyPeleLMeX
    PRIVATE
    # pybind11
    PeleLMeX.cpp
    pyPeleLMeX_Setup.cpp
    pyPeleLMeX_run.cpp
    pyPeleLMeX_parmparse.cpp
)

