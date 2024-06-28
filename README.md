# Spearfish

<p align="center">
  <img src="resources/spearfish.png" alt="Spearfish logo"/>
</p>
Spearfish (<b>SPE</b>cies tree <b>A</b>ware gene t<b>R</b>ee in<b>F</b>erence
with d<b>IS</b>tance met<b>H</b>ods) is a distance-based tree inference
algorithm that respects the species tree and can infer gene family trees.


## TODO

- [ ] src/
  - [ ] Clean up, better tree
    - [ ] Use less 3rd party
  - [ ] precomputations in cpp?
- [ ] License


### Usage

Clone and build the repository.
```
git clone https://github.com/knirschl/Spearfish.git
cd Spearfish
mkdir build
cd build
cmake ..
make
```

Spearfish supports multiple runmodi which can be selected by calling it with
different parameters:
  - Required
    - `s`, `--species-mat`: specify the species tree distance matrix in PHYLIP format
    - `-a`, `--align-mat`: specify the gene alignment distance matrix in PHYLIP format
  - OPTIONAL
    - -`t`, `--starting-tree`: specify the gene tree to compute dup/loss on
    (NEWICK format)
    - -`r`, `--tag-algo`: tag and reroot algorithm
      - `0`: APro (default)
      - `1`: MAD
      - `2`: None (everything gets corrected)
    - `-c`, `--compute-tree`: compute selection
      - `0`: only tree
      - `1`: tree and matrix
      - `2`: only matrix (default)
    - `p`, `--prefix`: specify the output prefix relative to working directory
    (default: `/`)
    - `-m`, `--map`: supply mapping file between species and locus names
    (default: "")
    - `l`: if set, mapping file maps locus names to species names, else other
    way around (default: unset)
    - `-d`, `--delimiter`: mapping separator


### Dependencies

Spearfish uses multiple Python packages to pre-process the input data, as well
as some phylogenetic tools that need to be build in order to run Spearfish with
optimal settings.

#### Phylogenetic tools

  - [FastME](http://www.atgc-montpellier.fr/fastme)
  - [MADroot](https://github.com/davidjamesbryant/MADroot)
  - [GeneRax](https://github.com/BenoitMorel/GeneRax)

#### Others

  - [argparse](https://github.com/p-ranav/argparse)
