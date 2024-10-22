import argparse
import os
import sys

from setuptools import setup

# Allow to control options via environment vars.
# Work-around for https://github.com/pypa/setuptools/issues/1712
PYPELELMEX_LIB_DIR = os.environ.get("PYPELELMEX_LIB_DIR")


allowed_dims = ["2d", "3d"]

# CMake and Package Managers
package_data = {"pypelelmex": []}
lib_dir = PYPELELMEX_LIB_DIR
my_path = os.path.dirname(os.path.realpath(__file__))
for dim in allowed_dims:
    lib_name = "libpelelmex.so"
    lib_path = os.path.join(lib_dir, lib_name)
    link_name = os.path.join(my_path, "pypelelmex", lib_name)
    if os.path.isfile(link_name):
        os.remove(link_name)
    if os.path.isfile(lib_path) and os.access(lib_path, os.R_OK):
        os.symlink(lib_path, link_name)
        package_data["pypelelmex"].append(lib_name)


setup(
    name="pypelelmex",
    version="0.1.0",
    packages=["pypelelmex"],
    package_dir={"pypelelmex": "pypelelmex"},
    package_data=package_data,
    install_requires=["numpy"],
    zip_safe=False,
)
