import argparse
import os
import tempfile

COMM = None
RANK = 0
libpelelmex = None


def rank_print(message, rank=0):
    if RANK == rank:
        print(message, flush=True)


def init_runtime():
    global COMM, RANK, libpelelmex

    from mpi4py import MPI

    from pypelelmex._libpelelmex import libpelelmex as _libpelelmex

    COMM = MPI.COMM_WORLD
    RANK = COMM.Get_rank()
    libpelelmex = _libpelelmex


def ensure_tmpdir_writable():
    def _can_create_tmp(dir_path):
        if not (os.path.isdir(dir_path) and os.access(dir_path, os.W_OK | os.X_OK)):
            return False
        try:
            test_dir = tempfile.mkdtemp(prefix="pypelelmex_tmpcheck_", dir=dir_path)
            os.rmdir(test_dir)
            return True
        except OSError:
            return False

    candidates = []
    if os.environ.get("PYPELELMEX_TMPDIR"):
        candidates.append(os.environ["PYPELELMEX_TMPDIR"])
    candidates.append(os.path.join(os.path.expanduser("~"), ".pypelelmex_tmp"))
    candidates.append(tempfile.gettempdir())

    selected_tmpdir = None
    for cand in candidates:
        try:
            os.makedirs(cand, exist_ok=True)
        except OSError:
            continue
        if _can_create_tmp(cand):
            selected_tmpdir = cand
            break

    if selected_tmpdir is None:
        raise PermissionError(
            "No writable temp directory available for MPI runtime initialization. "
            f"Tried: {candidates}."
        )

    # Set OpenMPI/PRTE temp-dir hints before importing mpi4py.
    os.environ["TMPDIR"] = selected_tmpdir
    os.environ["OMPI_MCA_orte_tmpdir_base"] = selected_tmpdir
    os.environ["OMPI_MCA_prte_tmpdir_base"] = selected_tmpdir
    rank_print(f"Using MPI temp directory: {selected_tmpdir}")


def parse_args():
    parser = argparse.ArgumentParser(description="Run pyPeleLMeX with MPI")
    parser.add_argument(
        "--input",
        default="hot_bubble.inp",
        help="Input file name or absolute path",
    )
    parser.add_argument(
        "--sundials-threads",
        type=int,
        default=1,
        help="Thread count passed to initialize_sundials",
    )
    return parser.parse_args()


def resolve_input_file(input_arg):
    if os.path.isabs(input_arg):
        return input_arg
    script_dir = os.path.dirname(os.path.abspath(__file__))
    return os.path.join(script_dir, input_arg)


def main():
    args = parse_args()
    ensure_tmpdir_writable()
    init_runtime()
    input_file = resolve_input_file(args.input)

    if not os.path.exists(input_file):
        raise FileNotFoundError(f"Input file does not exist: {input_file}")

    amrex_initialized = False
    sundials_initialized = False
    pelelm_created = False

    try:
        rank_print("Loading the PeleLM library...")
        libpelelmex.load_library()
        rank_print("Library loaded.")

        rank_print(f"Initializing AMReX with input file: {input_file}")
        libpelelmex.libpelelmex.initialize_amrex(input_file)
        amrex_initialized = True
        rank_print("AMReX initialized.")

        rank_print("Initializing SUNDIALS...")
        libpelelmex.libpelelmex.initialize_sundials(args.sundials_threads)
        sundials_initialized = True
        rank_print("SUNDIALS initialized.")

        rank_print("Creating PeleLM instance...")
        libpelelmex.libpelelmex.create_pelelmex()
        pelelm_created = True
        rank_print("PeleLM instance created.")

        rank_print("Setting up PeleLM instance...")
        libpelelmex.libpelelmex.setup_pelelmex()
        rank_print("PeleLM setup completed.")

        rank_print("Initializing PeleLMeX case...")
        libpelelmex.libpelelmex.initialize_pelelmex()
        rank_print("PeleLMeX case initialized.")

        run_mode_local = libpelelmex.libpelelmex.get_run_mode()
        run_mode = COMM.bcast(run_mode_local if RANK == 0 else None, root=0)

        if run_mode == "normal":
            rank_print("Running PeleLM in Evolve mode...")
            libpelelmex.libpelelmex.evolve_pelelmex()
        elif run_mode == "evaluate":
            rank_print("Running PeleLM in Evaluate mode...")
            libpelelmex.libpelelmex.evaluate_pelelmex()
        else:
            raise ValueError(f"Invalid run mode: {run_mode}")

    except Exception as exc:
        print(f"[rank {RANK}] Fatal error: {exc}", flush=True)
        COMM.Abort(1)
    finally:
        if pelelm_created:
            rank_print("Finalizing PeleLM instance...")
            libpelelmex.libpelelmex.finalize_pelelmex()
            rank_print("PeleLM instance finalized.")

        if sundials_initialized:
            rank_print("Finalizing SUNDIALS...")
            libpelelmex.libpelelmex.finalize_sundials()

        if amrex_initialized:
            rank_print("Finalizing AMReX...")
            libpelelmex.libpelelmex.finalize_amrex()
            rank_print("AMReX finalized.")


if __name__ == "__main__":
    main()
