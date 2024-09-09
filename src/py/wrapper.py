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

infer_string = ["NJ", "NJ", "FastME"]
infer_string_short = ["nj", "nj", "fm"]
tag_string = ["APro", "MAD", "NoTag"]


def convert_inputs(datadir, subst_model, is_dna, cores):
    # ========= Step 1: Convert to valid input data =========
    # Convert species tree and alignments to distance matrix
    utils.printFlush(f"Convert to valid input data")
    start = time.time()
    # ===
    dist_matrix_converter.convert_input(datadir, subst_model, is_dna, cores)
    # ===
    elapsed = time.time() - start
    print(f"Completed conversion. Elapsed time: {elapsed}s")
    return elapsed


def infer_trees(datadir, subst_model, is_dna, infer_algo=2, tag_algo=2, cores=16):
    infer_str = infer_string[infer_algo]
    # ========= Step 2: Infer trees =========
    # Combine Matrix and compute trees
    utils.printFlush(f"Run Spearfish with {infer_str}...\n****************************")
    start = time.time()
    try:
        # ===
        species_matrix = fam.get_true_species_tree_matrix(datadir)
        inferred_trees = launch_spearfish.run_spearfish_on_families(
            datadir, subst_model, is_dna, species_matrix, algo=tag_algo,
            mat_out=infer_algo, cores=cores)
        if infer_algo == 2:
            launch_fastme.run_fastme_on_families_matrices(datadir, "spearfish." + subst_model,
                                                          algo="B",
                                                          use_spr=True, cores=cores)
        # ===
        elapsed = time.time() - start
        print(f"Completed {infer_str} (tag={tag_string[tag_algo]}). Elapsed time: {elapsed}s")
        # print("=#=#= Took {}s per tree =#=#=".format(elapsed / ( int(
        # simphy.get_param_from_dataset_name("families", datadir)) * inferred_trees)))
        metrics.save_metrics(datadir,
            f"spearfish+{infer_string_short[compute]}+{tag_string[tag_algo].lower()}_trees",
            elapsed, "runtimes")
    except Exception as exc:
        utils.printFlush(f"Failed running Spearfish with {infer_str} "
            f"(tag={tag_string[tag_algo]})\n{exc}")
        return -1
    return elapsed


def pick_trees(datadir, subst_model, run_name, cores):
    # ========= Step 3: Pick best tree with GeneRax evaluation =========
    utils.printFlush("Picking best tree...\n**********")
    start = time.time()
    try:
        # ===
        species_tree = fam.get_species_tree(datadir)
        resultsdir = fam.get_run_dir(datadir, subst_model, "generax_eval_run")
        launch_generax.run(datadir, subst_model, "EVAL", species_tree, "spearfish", cores,
                           ["--rec-model", "UndatedDL", "--per-family-rates"], resultsdir, False)
        # ===
        elapsed = time.time() - start
        print(f"Completed pick. Elapsed time: {elapsed}s")
        metrics.save_metrics(datadir, f"{run_name}_pick", elapsed, "runtimes")
    except Exception as exc:
        utils.printFlush("Failed running pick\n" + str(exc))
        return -1
    return elapsed


def test_optimized(datadir, subst_model, is_dna, cores=16):
    # === Figure out subst_models ===
    # For testing purposes, subst_model should be same later
    sp = subst_model.split("/")
    fm_subst_model = sp[0]
    if (len(sp) == 2):
        generax_subst_model = sp[1]
    else:
        generax_subst_model = fm_subst_model

    # === Convert only once ===
    time_convert = convert_inputs(datadir, fm_subst_model, is_dna, cores)
#    # === Run all combinations ===
    time_inference = 0
    for infer_algo in [0, 2]:
        for tag_algo in range(3):
            time_inference += infer_trees(datadir, fm_subst_model, is_dna,
                                          infer_algo, tag_algo, cores)
    # === Pick best tree in every family-tag combination ===
    time_pick = pick_trees(datadir, generax_subst_model, "spearfish-all", cores)

    print(f"End of Spearfish. Elapsed total time: {time_inference + time_pick}s")


def run(datadir, subst_model, is_dna, cores, algo, compute):
    # For testing purposes, subst_model should be same later
    sp = subst_model.split("/")
    fm_subst_model = sp[0]
    if (len(sp) == 2):
        generax_subst_model = sp[1]
    else:
        generax_subst_model = fm_subst_model
    algo_str = tag_string[algo]
    compute_str = infer_string[compute]
    utils.printFlush(
        f"Run Spearfish with {compute_str}...\n****************************")
    try:
        start = time.time()
        # ========= Step 1: Convert to valid input data =========
        # Convert species tree and alignments to distance matrix
        dist_matrix_converter.convert_input(datadir, fm_subst_model, is_dna, cores)
        # ========= Step 2: Infer trees =========
        # Combine Matrix and compute trees
        species_matrix = fam.get_true_species_tree_matrix(datadir)
        inferred_trees = launch_spearfish.run_spearfish_on_families(datadir, fm_subst_model, is_dna,
                                                                    species_matrix,
                                                                    algo=algo, mat_out=compute,
                                                                    cores=cores)
        if (compute == 2):
            launch_fastme.run_fastme_on_families_matrices(datadir, "spearfish." + fm_subst_model,
                                                          algo="B",
                                                          use_spr=True, cores=cores)
        elapsed = time.time() - start
        utils.printFlush(f"Completed {compute_str} (tag={algo_str}). Elapsed time: {elapsed}s")
        # print("=#=#= Took {}s per tree =#=#=".format(elapsed / ( int(
        # simphy.get_param_from_dataset_name("families", datadir)) * inferred_trees)))
        metrics.save_metrics(datadir,
                             f"spearfish+{infer_string_short[compute]}+{algo_str.lower()}_trees",
                             elapsed, "runtimes")
    except Exception as exc:
        utils.printFlush(
            f"Failed running Spearfish with {compute_str} (tag={algo_str})\n{exc}")
        return
    # ========= Step 3: Pick best tree with GeneRax evaluation =========
    utils.printFlush("Picking best tree...\n**********")
    try:
        species_tree = fam.get_species_tree(datadir)
        resultsdir = fam.get_run_dir(datadir, generax_subst_model, "generax_eval_run")
        launch_generax.run(datadir, generax_subst_model, "EVAL", species_tree, "spearfish", cores,
                           ["--rec-model", "UndatedDL", "--per-family-rates"], resultsdir, False)
        elapsed2 = time.time() - start
        elapsed = elapsed2 - elapsed
        print(f"Completed pick. Elapsed time: {elapsed}s")
        print(
            f"End of Spearfish with {compute_str} (tag={algo_str})). Elapsed time: {elapsed2}s")
        metrics.save_metrics(datadir,
                             f"spearfish+{infer_string_short[compute]}+{algo_str.lower()}_pick",
                             elapsed, "runtimes")
        metrics.save_metrics(datadir,
                             f"spearfish+{compute_str}+{algo_str.lower()}_full",
                             elapsed2, "runtimes")
    except Exception as exc:
        utils.printFlush("Failed running pick\n" + str(exc))


if (__name__ == "__main__"):
    min_args_number = 5  # 4 + 1
    if (len(sys.argv) < min_args_number):
        print("Syntax error: python",
              os.path.basename(__file__),
              "dataset subst_model dna cores [compute=[0,1,2]] [algo=[0,1,2] [test].\n")
        sys.exit(1)

    datadir = os.path.normpath(sys.argv[1])
    subst_model = sys.argv[2]
    is_dna = bool(int(sys.argv[3])) # Cast to int is important!!!
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
        test_optimized(datadir, subst_model, is_dna, cores)
    else:
        if (compute == -1):
            compute = 2  # FM
        elif (algo == -1):
            algo = 2  # NoTag
        run(datadir, subst_model, is_dna, cores, algo, compute)
