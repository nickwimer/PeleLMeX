import importlib
import os
import sys


class LibPeleLMeX:

    def __init__(self):
        self.initialized = False

        # atexit.register(self.finalize)

    def __getattr__(self, attribute):
        if attribute == "libpelelmex":
            self.load_library()
            return self.__dict__[attribute]
        else:
            return self.__getattribute__(attribute)

    def load_library(self):
        if "libpelelmex" in self.__dict__:
            raise RuntimeError("libpelelmex has already been loaded")

        # Try both dimensional AMReX Python modules so runtime can match
        # the build dimension instead of hard-coding 3D.
        amrex_import_errors = []
        self.amr = None
        for amrex_module_name in ("amrex.space3d", "amrex.space2d"):
            try:
                self.amr = importlib.import_module(amrex_module_name)
                break
            except ImportError as exc:
                amrex_import_errors.append(f"{amrex_module_name}: {exc}")

        if self.amr is None:
            raise ImportError(
                "Could not import AMReX Python module (tried amrex.space3d/amrex.space2d).\n"
                + "\n".join(amrex_import_errors)
            )

        try:
            cxx = importlib.import_module(".pelelmex_pybind", package=__package__)
            self.libpelelmex = cxx
        except ImportError as exc:
            raise ImportError(
                "Could not import pypelelmex.pelelmex_pybind. "
                "Verify pypelelmex was built/installed and that the extension exists on sys.path.\n"
                f"cwd={os.getcwd()}\n"
                f"package={__package__}\n"
                f"sys.path[0:5]={sys.path[:5]}\n"
                f"original error: {exc}"
            ) from exc

    # def initialize(self):
    #     self.amr.initialize([])

    #     self.initialized = True

    # def finalize(self):
    #     if self.initialized:
    #         self.amr.finalize()
    #         self.initialized = False

    # def test_read(self):
    #     self.load_library()
    #     self.initialize()
    #     self.libpelelmex.read_params()
    #     # self.libpelelmex.setup()
    #     self.finalize()


libpelelmex = LibPeleLMeX()
