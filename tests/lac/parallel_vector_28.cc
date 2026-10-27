// -----------------------------------------------------------------------------
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception OR LGPL-2.1-or-later
// Copyright (C) 2026 by the deal.II authors
//
// This file is part of the deal.II library.
//
// Detailed license information governing the source code and contributions
// can be found in LICENSE.md and CONTRIBUTING.md at the top level directory.
//
// -----------------------------------------------------------------------------


// check that LA::Vector::compress(VectorOperation::add) gives the same results
// in MemorySpace::Default and MemorySpace::Host, bit by bit, when an owned
// entry receives contributions from several processors

#include <deal.II/base/index_set.h>
#include <deal.II/base/utilities.h>

#include <deal.II/lac/la_parallel_vector.h>

#include <iostream>
#include <vector>

#include "../tests.h"


void
test()
{
  const unsigned int myid    = Utilities::MPI::this_mpi_process(MPI_COMM_WORLD);
  const unsigned int numproc = Utilities::MPI::n_mpi_processes(MPI_COMM_WORLD);

  if (myid == 0)
    deallog << "numproc=" << numproc << std::endl;

  // each processor owns 4 indices; the first two indices of every processor
  // are ghosts on all other processors, the third one only on the next
  // processor
  const unsigned int n_local = 4;
  IndexSet           local_owned(numproc * n_local);
  local_owned.add_range(myid * n_local, (myid + 1) * n_local);
  IndexSet local_relevant = local_owned;
  for (unsigned int p = 0; p < numproc; ++p)
    if (p != myid)
      local_relevant.add_range(p * n_local, p * n_local + 2);
  local_relevant.add_index(((myid + numproc - 1) % numproc) * n_local + 2);

  LinearAlgebra::distributed::Vector<double, MemorySpace::Host> v_host(
    local_owned, local_relevant, MPI_COMM_WORLD);
  LinearAlgebra::distributed::Vector<double, MemorySpace::Default> v_device(
    local_owned, local_relevant, MPI_COMM_WORLD);
  const auto &partitioner = v_host.get_partitioner();

  // values that are not exactly representable, so that the result depends
  // on the order of the additions
  const unsigned int  n_values = local_relevant.n_elements();
  std::vector<double> values(n_values);
  for (unsigned int i = 0; i < n_values; ++i)
    values[i] =
      0.1 * (myid + 1) + 0.01 * partitioner->local_to_global(i) + 1. / 3.;

  for (unsigned int i = 0; i < n_values; ++i)
    v_host.local_element(i) = values[i];
  Kokkos::deep_copy(
    Kokkos::View<double *, MemorySpace::Default::kokkos_space>(
      v_device.get_values(), n_values),
    Kokkos::View<const double *, Kokkos::HostSpace>(values.data(), n_values));

  v_host.compress(VectorOperation::add);
  v_device.compress(VectorOperation::add);

  std::vector<double> result_device(n_local);
  Kokkos::deep_copy(
    Kokkos::View<double *, Kokkos::HostSpace>(result_device.data(), n_local),
    Kokkos::View<const double *, MemorySpace::Default::kokkos_space>(
      v_device.get_values(), n_local));

  for (unsigned int i = 0; i < n_local; ++i)
    deallog << myid << ": entry " << partitioner->local_to_global(i) << ": "
            << v_host.local_element(i) << ", identical: "
            << (result_device[i] == v_host.local_element(i) ? "yes" : "no")
            << std::endl;

  if (myid == 0)
    deallog << "OK" << std::endl;
}



int
main(int argc, char **argv)
{
  Utilities::MPI::MPI_InitFinalize mpi_initialization(
    argc, argv, testing_max_num_threads());

  MPILogInitAll log;

  test();
}
