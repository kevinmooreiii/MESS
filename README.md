# MESS

Master Equation System Solver

The primary purpose of the program is to calculate temperature and pressure dependent
rate coefficients for complex-forming reactions via solution of the one-dimensional master equation. Ancillary calculations of various quantities (e.g., stabilization probabilities for microcanonical initial distributions, microcanonical rate constants, partition functions, and related thermochemical information, time dependent propagation of species populations, etc.) are also available. 

### Installation using Conda

The most direct way to install the code is through the conda package manager.
If you have conda installed, simply run the following command in whichever
environment you choose:
```
conda install -c auto-mech mess
```
If you do not have conda, it can be installed using the shell script
`debug/install-conda.sh`.

### Building from source using Conda environment for dependencies

To build the code from source for development or debugging purposes, first
create a conda environment with the necessary dependencies as follows:
```
conda env create -f environment.yml
```
which will create the `mess-env` environment.
You can then activate the environment and build the code as follows:
```
conda activate mess-env
bash debug/build.sh
```
To put the MESS executables in your path, you can then run
```
. debug/fake-install.sh
```

### Building from source without Conda

This is not the advised way to install, since the user will have to deal with their specific system set-up

To build MESS, the following libraries are required:

BLAS
LAPACK
SLATEC 
MPACK

SLATEC and MPACK can be downloaded from their respective GitHub sites

SLATEC: <https://github.com/Auto-Mech/SLATEC>
MPACK: <https://github.com/Auto-Mech/MPACK>

and installed individually in a location that your system can find them. 

With SLATEC and MPACK installed, can run build.sh, which uses cmake to compile MESS
```
bash build.sh
```

## Reference

See Y. Georgievskii, J. A. Miller, M. P. Burke, and S. J. Klippenstein,
Reformulation and Solution of the Master Equation for Multiple-Well Chemical
Reactions, J. Phys. Chem. A, 117, 12146-12154 (2013).

## Acknowledgment

This work was supported by the U.S. Department of Energy, Office of Basic Energy
Sciences, Division of Chemical Sciences, Geosciences, and Biosciences under DOE
Contract Number DE-AC02-06CH11357 as well as the Exascale Computing Project
(ECP), Project Number: 17-SC-20-SC.  The ECP is a collaborative effort of two
DOE organizations, the Office of Science and the National Nuclear Security
Administration, responsible for the planning and preparation of a capable
exascale ecosystem including software, applications, hardware, advanced system
engineering, and early test bed platforms to support the nation's exascale
computing imperative. 

## Notice

Copyright (c) 2018 Yuri Georgievski (ygeorgi@anl.gov), Stephen J.
Klippenstein (sjk@anl.gov), and Argonne National Laboratory.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
