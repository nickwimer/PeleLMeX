import os
import time

import numpy as np
from mpi4py import MPI

from pypelelmex._libpelelmex import libpelelmex


def rank_print(message, rank=0):
    if MPI.COMM_WORLD.Get_rank() == rank:
        print(message, flush=True)


# if rank == 0:
input_file = os.path.join(os.getcwd(), "hot_bubble.inp")
# input_file = os.path.join(os.getcwd(), "jet.inp")
# input_file = os.path.join(os.getcwd(), "taylor_green.inp")

# Load the library
rank_print("Loading the PeleLM library...")
libpelelmex.load_library()
rank_print("Library loaded.")

# write all user defined variables to a text input file...

# Initialize AMReX
rank_print(f"Initializing AMReX with input file: {input_file}")
libpelelmex.libpelelmex.initialize_amrex(input_file)
rank_print("AMReX initialized.")

# Initialize Sundials
rank_print("Initializing SUNDIALS...")
libpelelmex.libpelelmex.initialize_sundials()
rank_print("SUNDIALS initialized.")


# Start timer
if MPI.COMM_WORLD.Get_rank() == 0:
    start_time = time.time()

# Create PeleLM instance
rank_print("Creating PeleLM instance...")
libpelelmex.libpelelmex.create_pelelmex()
rank_print("PeleLM instance created.")


# Set up the PeleLM instance
try:
    if hasattr(libpelelmex.libpelelmex, "setup_pelelmex"):
        rank_print("Setting up PeleLM instance...")
        libpelelmex.libpelelmex.setup_pelelmex()
        rank_print("PeleLM setup completed.")
except Exception as e:
    rank_print(f"Setup PeleLM failed: {e}")


rank_print(f"Vel X index: {libpelelmex.libpelelmex.VELX}")
rank_print(f"Vel Y index: {libpelelmex.libpelelmex.VELY}")
rank_print(f"Vel Z index: {libpelelmex.libpelelmex.VELZ}")
rank_print(f"Temp index: {libpelelmex.libpelelmex.TEMP}")

# TODO: Either override prob parm data or read in from file like HIT problem

# Initialize the PeleLMeX Case
rank_print("Initializing PeleLMeX case...")
libpelelmex.libpelelmex.initialize_pelelmex()
rank_print("PeleLMeX case initialized.")

# Switch between Evolve and UnitTest mode based on runmode
run_mode = libpelelmex.libpelelmex.get_run_mode()
if run_mode == "normal":
    rank_print("Running PeleLM in Evolve mode...")
    libpelelmex.libpelelmex.evolve_pelelmex()
elif run_mode == "evaluate":
    rank_print("Running PeleLM in Evaluate mode...")
    libpelelmex.libpelelmex.evaluate_pelelmex()
else:
    raise ValueError(f"Invalid run mode: {run_mode}")

# Stop timer
if MPI.COMM_WORLD.Get_rank() == 0:
    end_time = time.time()
    rank_print(f"Total time: {end_time - start_time:.2f} seconds")

# Finalize Sundials
rank_print("Finalizing SUNDIALS...")
libpelelmex.libpelelmex.finalize_sundials()

# Finalize PeleLM
rank_print("Finalizing PeleLM instance...")
libpelelmex.libpelelmex.finalize_pelelmex()
rank_print("PeleLM instance finalized.")

# Finalize AMReX
rank_print("Finalizing AMReX...")
libpelelmex.libpelelmex.finalize_amrex()
rank_print("AMReX finalized.")
