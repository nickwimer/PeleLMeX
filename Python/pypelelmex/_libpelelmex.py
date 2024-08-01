import atexit


class LibPeleLMeX:

    def __init__(self):
        self.initialized = False

        atexit.register(self.finalize)

    def __getattr__(self, attribute):
        if attribute == "libpelelmex":
            self.load_library()
            return self.__dict__[attribute]
        else:
            return self.__getattribute__(attribute)

    def load_library(self):

        if "libpelelmex_so" in self.__dict__:
            raise RuntimeError("libpelelmex has already been loaded")

        try:
            import amrex.space3d as amr

            self.amr = amr

            from . import pelelmex_pybind as cxx

            self.libpelelmex = cxx

        except ImportError:
            raise ImportError("Could not import libpelelmex")

    def initialize(self):
        self.amr.initialize([])

        self.initialized = True

    def finalize(self):
        if self.initialized:
            self.amr.finalize()
            self.initialized = False

    def test_read(self):
        self.load_library()
        self.initialize()
        self.libpelelmex.read_params()
        # self.libpelelmex.setup()
        self.finalize()


libpelelmex = LibPeleLMeX()
