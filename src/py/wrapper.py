import os
import time
import sys

sys.path.insert(0, 'scripts')
import launch_spearfish
import launch_fastme
import launch_generax
import utils
sys.path.insert(0, 'tools/data')
import fam
import metrics
sys.path.insert(0, 'tools/msa')
import dist_matrix_converter

compute_string = ["NJ", "NJ", "FastME"]
compute_string_short = ["nj", "nj", "fm"]
algo_string = ["APro", "MAD", "NoTag"]


def run(datadir, subst_model, is_dna, cores, algo, compute):
    algo_str = algo_string[algo]
    compute_str = compute_string[compute]
    utils.printFlush(
        f"Run Spearfish with {compute_str}...\n****************************")
    try:
        start = time.time()
        # ========= Step 1: Convert to valid input data =========
        # Convert species tree and alignments to distance matrix
        dist_matrix_converter.convert_input(datadir, subst_model, is_dna, cores)
        species_tree = fam.get_true_species_tree_matrix(datadir)
        # ========= Step 2: Infer trees =========
        # Combine Matrix and compute trees
        inferred_trees = launch_spearfish.run_spearfish_on_families(datadir, subst_model, is_dna, species_tree,
                                                                    algo=algo, mat_out=compute,
                                                                    cores=cores)
        if (compute == 2):
            launch_fastme.run_fastme_on_families_matrices(datadir, "spearfish." + subst_model, algo="B",
                                                          use_spr=True, cores=cores)
        elapsed = time.time() - start
        print(f"Completed {compute_str} (tag={algo_str}). Elapsed time: {elapsed}s")
        # print("=#=#= Took {}s per tree =#=#=".format(elapsed / ( int(
        # simphy.get_param_from_dataset_name("families", datadir)) * inferred_trees)))
        metrics.save_metrics(datadir,
                             f"spearfish+{compute_string_short[compute]}+{algo_str.lower()}_trees",
                             elapsed, "runtimes")
    except Exception as exc:
        utils.printFlush(
            f"Failed running Spearfish with {compute_str} (tag={algo_str})\n{exc}")
        return
    # ========= Step 2: Pick best tree with GeneRax evaluation =========
    utils.printFlush("Picking best tree...\n**********")
    try:
        species_tree = fam.get_species_tree(datadir)
        resultsdir = fam.get_run_dir(datadir, subst_model, "generax_eval_run")
        launch_generax.run(datadir, subst_model, "EVAL", species_tree, "spearfish", cores,
                           ["--rec-model", "UndatedDL", "--per-family-rates"], resultsdir, False)
        elapsed2 = time.time() - start
        elapsed = elapsed2 - elapsed
        print(f"Completed pick. Elapsed time: {elapsed}s")
        print(
            f"End of Spearfish with {compute_str} (tag={algo_str})). Elapsed time: {elapsed2}s")
        metrics.save_metrics(datadir,
                             f"spearfish+{compute_string_short[compute]}+{algo_str.lower()}_pick",
                             elapsed, "runtimes")
        metrics.save_metrics(datadir,
                             f"spearfish+{compute_str}+{algo_str.lower()}_full",
                             elapsed2, "runtimes")
    except Exception as exc:
        utils.printFlush("Failed running pick\n" + str(exc))


def test(datadir, subst_model, is_dna, cores):
    print("testing")
    for compute in [0, 2]:
        for algo in [0, 1, 2]:
            run(datadir, subst_model, is_dna, cores, algo, compute)


def list_or_int(arg):
    arg = arg.split("=")[1]
    if arg.endswith("]"):
        return [int(a) for a in arg[1:-1].split(",")]
    else:
        return int(arg)


if (__name__ == "__main__"):
    min_args_number = 5  # 4 + 1
    if (len(sys.argv) < min_args_number):
        print("Syntax error: python",
              os.path.basename(__file__),
              "dataset subst_model dna cores [compute=[0,1,2]] [algo=[0,1,2] [test].\n")
        sys.exit(1)

    datadir = os.path.normpath(sys.argv[1])
    subst_model = sys.argv[2]
    is_dna = bool(sys.argv[3])
    cores = int(sys.argv[4])
    additional_arguments = sys.argv[min_args_number:]
    # c++ run parameters
    compute = -1
    algo = -1
    run_test = False
    for arg in additional_arguments:
        if arg == "test":
            run_test = True
            break
        elif arg.startswith("compute="):
            compute = int(arg)
        elif arg.startswith("algo="):
            algo = int(arg)
    if run_test:
        test(datadir, subst_model, is_dna, cores)
    else:
        if (compute == -1):
            compute = 2  # FM
        elif (algo == -1):
            algo = 2  # NoTag
        run(datadir, subst_model, is_dna, cores, algo, compute)
