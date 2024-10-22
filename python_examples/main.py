import os
import time

import numpy as np

from pypelelmex._libpelelmex import libpelelmex

input_file = os.path.join(os.getcwd(), "hot_bubble.inp")
# input_file = os.path.join(os.getcwd(), "jet.inp")

# Load the library
print("Loading the PeleLM library...")
libpelelmex.load_library()
print("Library loaded.")

# Initialize AMReX
print(f"Initializing AMReX with input file: {input_file}")
libpelelmex.libpelelmex.initialize_amrex(input_file)
print("AMReX initialized.")

# Initialize Sundials
print("Initializing SUNDIALS...")
libpelelmex.libpelelmex.initialize_sundials()
print("SUNDIALS initialized.")


# Start timer
start_time = time.time()

# Create PeleLM instance
print("Creating PeleLM instance...")
libpelelmex.libpelelmex.create_pelelmex()
print("PeleLM instance created.")


# Set up the PeleLM instance
try:
    if hasattr(libpelelmex.libpelelmex, "setup_pelelmex"):
        print("Setting up PeleLM instance...")
        libpelelmex.libpelelmex.setup_pelelmex()
        print("PeleLM setup completed.")
except Exception as e:
    print(f"Setup PeleLM failed: {e}")


print(f"Vel X index: {libpelelmex.libpelelmex.VELX}")
print(f"Vel Y index: {libpelelmex.libpelelmex.VELY}")
print(f"Vel Z index: {libpelelmex.libpelelmex.VELZ}")
print(f"Temp index: {libpelelmex.libpelelmex.TEMP}")

# TODO: Either override prob parm data or read in from file like HIT problem

# Initialize the PeleLMeX Case
print("Initializing PeleLMeX case...")
libpelelmex.libpelelmex.initialize_pelelmex()
print("PeleLMeX case initialized.")

# Switch between Evolve and UnitTest mode based on runmode
run_mode = libpelelmex.libpelelmex.get_run_mode()
if run_mode == "normal":
    print("Running PeleLM in Evolve mode...")
    libpelelmex.libpelelmex.evolve_pelelmex()
elif run_mode == "evaluate":
    print("Running PeleLM in Evaluate mode...")
    libpelelmex.libpelelmex.evaluate_pelelmex()
else:
    raise ValueError(f"Invalid run mode: {run_mode}")

# Stop timer
end_time = time.time()
print(f"Total time: {end_time - start_time:.2f} seconds")

# Finalize Sundials
print("Finalizing SUNDIALS...")
libpelelmex.libpelelmex.finalize_sundials()

# Finalize PeleLM
print("Finalizing PeleLM instance...")
libpelelmex.libpelelmex.finalize_pelelmex()
print("PeleLM instance finalized.")

# Finalize AMReX
print("Finalizing AMReX...")
libpelelmex.libpelelmex.finalize_amrex()
print("AMReX finalized.")
