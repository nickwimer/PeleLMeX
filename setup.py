import os
import re
import shutil
import subprocess
import sys

import setuptools
from setuptools import Extension, find_packages, setup
from setuptools.command.build import build
from setuptools.command.build_ext import build_ext

# dst_path = os.path.join(self.build_lib, "amrex")
#         shutil.copytree(PYAMREX_libdir, dst_path, dirs_exist_ok=True)


class CopyPreBuild(build):
    def initialize_options(self):
        build.initialize_options(self)
        # We just overwrite this because the default "build" (and "build/lib")
        # clashes with directories many developers have in their source trees;
        # this can create confusing results with "pip install .", which clones
        # the whole source tree by default
        self.build_base = "_tmppythonbuild"
        # self.build_base = os.path.join("_tmppythonbuild", "pele")

    def run(self):
        # remove existing build directory
        #   by default, this stays around. we want to make sure generated
        #   files like libwarpx.(1d|2d|rz|3d).(so|pyd) are always only the
        #   ones we want to package and not ones from an earlier wheel's stage
        if os.path.exists(self.build_base):
            shutil.rmtree(self.build_base)

        # call superclass
        build.run(self)

        # matches: warpx_pybind_(1d|2d|rz|3d). ... .(so|pyd)
        # re_libprefix = re.compile(r"pelelmex_pybind..\..*\.(?:so|pyd)")
        re_libprefix = re.compile(r"pelelmex_pybind.*\.(?:so|pyd)")
        libs_found = []
        for lib_name in os.listdir(PYPELELMEX_LIB_DIR):
            if re_libprefix.match(lib_name):
                print(f"Found {lib_name} in {PYPELELMEX_LIB_DIR}")
                lib_path = os.path.join(PYPELELMEX_LIB_DIR, lib_name)
                libs_found.append(lib_path)
            else:
                print(f"Skipping {lib_name} in {PYPELELMEX_LIB_DIR}")

        # exit()
        if len(libs_found) == 0:
            raise RuntimeError(
                "Error: no pre-build PeleLMeX modules found in "
                "PYPELELMEX_LIB_DIR='{}'".format(PYPELELMEX_LIB_DIR)
            )

        # copy external libs into collection of files in a temporary build dir
        dst_path = os.path.join(self.build_lib, "pypelelmex")
        for lib_path in libs_found:
            shutil.copy(lib_path, dst_path)
        # dst_path = os.path.join(self.build_lib, "amrex")
        # shutil.copytree(PYAMREX_libdir, dst_path, dirs_exist_ok=True)

        # Get the install_lib command object
        # install_lib_cmd = self.get_finalized_command("install_lib")
        # dest_dir = os.path.join(install_lib_cmd.install_dir, "amrex")
        dest_dir = os.path.join(
            os.getcwd(), "build_py", "lib", "site-packages", "amrex"
        )
        # Define the source directory
        src_dir = os.path.join(
            os.getcwd(), "build_py", "_deps", "fetchedpyamrex-src", "src", "amrex"
        )
        # Copy files
        if os.path.exists(src_dir):
            shutil.copytree(src_dir, dest_dir, dirs_exist_ok=True)
        else:
            print(f"Source directory {src_dir} does not exist.")


# class CMakeExtension(Extension):
#     def __init__(self, name, sourcedir=""):
#         Extension.__init__(self, name, sources=[])
#         self.sourcedir = os.path.abspath(sourcedir)


# class CMakeBuild(build_ext):
#     def run(self):
#         from packaging.version import parse

#         try:
#             out = subprocess.check_output(["cmake", "--version"])
#         except OSError:
#             raise RuntimeError(
#                 "CMake must be installed to build the following extensions: "
#                 + ", ".join(e.name for e in self.extensions)
#             )

#         cmake_version = parse(re.search(r"version\s*([\d.]+)", out.decode()).group(1))
#         if cmake_version < parse("3.20.0"):
#             raise RuntimeError("CMake >= 3.20.0 is required")

#         for ext in self.extensions:
#             self.build_extension(ext)

#     def build_extension(self, ext):
#         extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
#         cmake_args = [
#             "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + extdir,
#             "-DPYTHON_EXECUTABLE=" + sys.executable,
#         ]

#         cfg = "Debug" if self.debug else "Release"
#         build_args = ["--config", cfg]

#         if not os.path.exists(self.build_temp):
#             os.makedirs(self.build_temp)
#         subprocess.check_call(
#             ["cmake", ext.sourcedir] + cmake_args, cwd=self.build_temp
#         )
#         subprocess.check_call(
#             ["cmake", "--build", "."] + build_args, cwd=self.build_temp
#         )


cxx_modules = []
cmdclass = {}

PYPELELMEX_LIB_DIR = os.environ.get("PYPELELMEX_LIB_DIR")
# PYAMREX_libdir = os.environ.get("PYAMREX_LIB_DIR")

print(f"PYPELELMEX_LIB_DIR: {PYPELELMEX_LIB_DIR}")
# print(f"PYAMREX_libdir: {PYAMREX_libdir}")
# exit()

env = os.environ.copy()

ENABLE_MPI = os.environ.pop("ENABLE_MPI", "OFF")

if PYPELELMEX_LIB_DIR:
    cmdclass = dict(build=CopyPreBuild)
else:
    raise RuntimeError(
        "Error: environment variable PYPELELMEX_LIB_DIR not set. "
        "Please set PYPELELMEX_LIB_DIR to the directory containing the "
        "pre-built PeleLMeX libraries."
    )

# Get the package requirements from the requirements.txt file
install_requires = []
with open("./requirements.txt") as f:
    install_requires = [line.strip("\n") for line in f.readlines()]
    if ENABLE_MPI == "ON":
        install_requires.append("mpi4py>=2.1.0")


setup(
    name="pypelelmex",
    version="0.1.0",
    packages=["pypelelmex"],
    package_dir={"pypelelmex": "Python/pypelelmex"},
    # packages=find_packages(),
    include_package_data=True,
    # package_data={
    #     "amrex": ["*"],
    # },
    author="",
    author_email="",
    description="",
    long_description="",
    ext_modules=cxx_modules,
    cmdclass=cmdclass,
    install_requires=install_requires,
    extras_require={
        "all": ["yt>=4.1.0", "matplotlib"],
    },
    zip_safe=False,
)
