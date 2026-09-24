[![regression-tester/current](https://dealii.org/regression_tests/reports/current-status-badge.svg)](https://dealii.org/regression_tests/reports/render.html?#!current.md)
[![regression-tester/previous](https://dealii.org/regression_tests/reports/previous-status-badge.svg)](https://dealii.org/regression_tests/reports/render.html?#!previous.md)
[![performance-tester/current](https://dealii.org/performance_tests/reports/current-status-badge.svg)](https://dealii.org/performance_tests/reports/render.html?#!current.md)
[![jenkins/dealii-serial](https://ci.tjhei.info/job/dealii-serial/job/master/badge/icon?style=plastic&subject=jenkins-serial)](https://ci.tjhei.info/job/dealii-serial/job/master/)
[![jenkins/dealii-mpi](https://ci.tjhei.info/job/dealii-mpi/job/master/badge/icon?style=plastic&subject=jenkins-MPI)](https://ci.tjhei.info/job/dealii-mpi/job/master/)
[![jenkins/dealii-osx](https://ci.tjhei.info/job/dealii-osx/job/master/badge/icon?style=plastic&subject=jenkins-OSX)](https://ci.tjhei.info/job/dealii-osx/job/master/)
[![jenkins/dealii-ampere](https://ci.tjhei.info/job/dealii-ampere/job/master/badge/icon?style=plastic&subject=jenkins-ampere)](https://ci.tjhei.info/job/dealii-ampere/job/master/)
[![workflows/indent](https://github.com/dealii/dealii/actions/workflows/indent.yml/badge.svg?branch=master)](https://github.com/dealii/dealii/actions/workflows/indent.yml?query=branch%3Amaster)
[![workflows/tidy](https://github.com/dealii/dealii/actions/workflows/tidy.yml/badge.svg?branch=master)](https://github.com/dealii/dealii/actions/workflows/tidy.yml?query=branch%3Amaster)
[![workflows/github-linux](https://github.com/dealii/dealii/actions/workflows/linux.yml/badge.svg?branch=master)](https://github.com/dealii/dealii/actions/workflows/linux.yml?query=branch%3Amaster)
[![workflows/github-OSX](https://github.com/dealii/dealii/actions/workflows/osx.yml/badge.svg?branch=master)](https://github.com/dealii/dealii/actions/workflows/osx.yml?query=branch%3Amaster)
[![workflows/github-windows](https://github.com/dealii/dealii/actions/workflows/windows.yml/badge.svg?branch=master)](https://github.com/dealii/dealii/actions/workflows/windows.yml?query=branch%3Amaster)

> **Notice:** Below are the standard deal.II instructions. For the instructions particular to step-76, and to work with the asynchronous DG method based on CAA implementation in step-99, please [go to the bottom](#reproducing-step-76-and-step-99-asynchronous-dg-method-based-on-caa).

What is deal.II?
================

deal.II is a C++ program library targeted at the computational solution
of partial differential equations using adaptive finite elements. It uses
state-of-the-art programming techniques to offer you a modern interface
to the complex data structures and algorithms required.

For the impatient:
------------------

Let's say you've unpacked the .tar.gz file into a directory /path/to/dealii/sources.
Then configure, compile, and install the deal.II library with:

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_INSTALL_PREFIX=/path/where/dealii/should/be/installed/to /path/to/dealii/sources
    $ make install    (alternatively $ make -j<N> install)
    $ make test

To build from the repository, execute the following commands first:

    $ git clone https://github.com/dealii/dealii
    $ cd dealii

Then continue as before.

A detailed *ReadME* can be found at [./doc/readme.html](https://dealii.org/developer/readme.html),
[./doc/users/cmake_user.html](https://dealii.org/developer/users/cmake_user.html),
or at https://dealii.org/.

Getting started:
----------------

The tutorial steps are located under examples/ of the installation.
Information about the tutorial steps can be found at
[./doc/doxygen/tutorial/index.html](https://dealii.org/developer/doxygen/deal.II/Tutorial.html)
or at https://dealii.org/.

deal.II includes support for pretty-printing deal.II objects inside GDB.
See [`contrib/utilities/dotgdbinit.py`](contrib/utilities/dotgdbinit.py) or
the new documentation page (under 'information for users') for instructions
on how to set this up.

License:
--------

Please see the file [./LICENSE.md](LICENSE.md) for details

Further information:
--------------------

For further information have a look at
[./doc/index.html](https://dealii.org/developer/index.html) or at
https://dealii.org.

Docker Images:
-------------

Docker images based on the Ubuntu operating system are available on
[Docker Hub](https://hub.docker.com/r/dealii/dealii). You can
use any of the available version
([list of available tags](https://hub.docker.com/r/dealii/dealii/tags))
by running, for example:

    $ docker run --rm -t -i dealii/dealii:master-focal

The above command would drop you into an isolated environment, in which you
will find the latest version of deal.II (master development branch) installed
under `/usr/local`.

---

# Reproducing step-76 and step-99 (Asynchronous DG method based on CAA)

This section provides instructions for building deal.II and running the synchronous DG benchmark (`step-76`) as well as the asynchronous DG benchmark based on the communication-avoiding algorithm (CAA) framework implemented in `step-99`.

### 1. Prerequisites and Dependencies
In addition to general compiler tools and CMake, the following dependencies are required:
* **MPI** (`mpicc`, `mpicxx`, `mpifc`)
* **p4est** (parallel distributed mesh management, compiled with MPI)
* **BLAS / LAPACK / ScaLAPACK (optional)** (e.g., Intel oneAPI MKL via `${MKLROOT}`)

---

### 2. Clone the Repository and Switch Branch

Clone the repository and switch to the `dealii-v94-at` branch:

```bash
git clone git@github.com:gshubhamk/dealii.git
cd dealii
git checkout dealii-v94-at
```

---

### 3. Configure, Build, and Install deal.II

Create a build directory alongside the cloned `dealii` source directory, configure the library using CMake and compile:

```bash
cd ..
mkdir dealii-build
cd dealii-build

cmake \
  -DCMAKE_INSTALL_PREFIX=/path/where/dealii/should/be/installed/to \
  -DCMAKE_C_COMPILER=mpicc \
  -DCMAKE_CXX_COMPILER=mpicxx \
  -DCMAKE_Fortran_COMPILER=mpifc \
  -DMPI_C_COMPILER=mpicc \
  -DMPI_CXX_COMPILER=mpicxx \
  -DMPI_Fortran_COMPILER=mpifc \
  -DCMAKE_CXX_FLAGS="-march=native" \
  -DCMAKE_C_FLAGS="-march=native" \
  -DDEAL_II_CXX_FLAGS_RELEASE="-O2" \
  -DDEAL_II_COMPONENT_EXAMPLES=OFF \
  -DDEAL_II_WITH_MPI=ON \
  -DDEAL_II_WITH_64BIT_INDICES=ON \
  -DDEAL_II_WITH_TBB=OFF \
  -DDEAL_II_WITH_TASKFLOW=OFF \
  -DDEAL_II_WITH_P4EST=ON \
  -DP4EST_DIR=/path/to/p4est/install \
  -DDEAL_II_WITH_LAPACK=ON \
  -DLAPACK_DIR="${MKLROOT}/lib/intel64" \
  -DLAPACK_FOUND=true \
  -DLAPACK_LIBRARIES="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl" \
  -DLAPACK_INCLUDE_DIRS="${MKLROOT}/include" \
  -DDEAL_II_WITH_SCALAPACK=OFF \
  ../dealii

make -j32
make install
```

### 4. Running `step-76`

Copy `step-76` from the source repository into dealii-build/examples/, configure, build, and run:

```bash
cp -r ../dealii/examples/step-76 examples/
cd examples/step-76

cmake .
make -j32
mpirun -n 4 ./step-76
```

### 5. Running `step-99` (CAA Implementation)

The communication-avoiding algorithm (CAA) framework is implemented in `step-99`. To set up and run it:

```bash
cd ../
cp -r ../dealii/examples/step-99 examples/
cd examples/step-99

cmake .
make -j32
mpirun -n 4 ./step-99
```

#### Configurable Parameters

The solver can be configured by modifying the following parameters to reproduce the test cases reported in the paper:

* **`testcase`**: Specifies the benchmark problem to run.
* **`dimension`**: Sets the spatial dimension of the problem (e.g., `2` for 2D or `3` for 3D).
* **`n_global_refinements`**: The number of initial global mesh refinements to control mesh resolution and total degree-of-freedom (DoF) count.
* **`fe_degree`**: Degree of the discontinuous finite element polynomial space ($k$ for $\mathbb{Q}_k$ elements).
* **`cfl`**: The Courant–Friedrichs–Lewy (CFL) number used to determine the time step size $\Delta t$ for temporal stability.
* **`L`**: Maximum allowable delay.
* **`caa`**: Flag/switch to enable the communication-avoiding algorithm (CAA) (`true`/`false`).
* **`AT_flux_flag`**: Selects the asynchronous numerical flux treatment across interface boundaries (i.e., enabling the modified asynchrony-tolerant flux evaluations).

*(All other parameters remain unchanged and retain their default values as configured in the branch.)*