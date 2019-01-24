##################################
# uberenv host-config
#
# This is a generated file, edit at own risk.
##################################
# chaos_5_x86_64_ib-clang@3.9.1
##################################

# cmake from uberenv
# cmake executable path: /usr/workspace/wsrzc/axom/thirdparty_libs/builds/2018_08_07_19_23_24/spack/opt/spack/chaos_5_x86_64_ib/clang-3.9.1/cmake-3.9.6-4nbvmm4v43k5hps7sieqofjzxfge4abg/bin/cmake

#######
# using clang@3.9.1 compiler spec
#######

# c compiler used by spack
set(CMAKE_C_COMPILER "/usr/global/tools/clang/chaos_5_x86_64_ib/clang-3.9.1/bin/clang" CACHE PATH "")

# cpp compiler used by spack
set(CMAKE_CXX_COMPILER "/usr/global/tools/clang/chaos_5_x86_64_ib/clang-3.9.1/bin/clang++" CACHE PATH "")

# fortran compiler used by spack
set(ENABLE_FORTRAN ON CACHE BOOL "")

set(CMAKE_Fortran_COMPILER "/usr/apps/gnu/4.9.3/bin/gfortran" CACHE PATH "")

# Root directory for generated TPLs
set(TPL_ROOT "/usr/workspace/wsrzc/axom/thirdparty_libs/builds/2018_08_07_19_23_24/spack/opt/spack/chaos_5_x86_64_ib/clang-3.9.1" CACHE PATH "")

# hdf5 from uberenv
set(HDF5_DIR "${TPL_ROOT}/hdf5-1.8.16-gcsba5lbtjzskhtolq7d3ciksvog5mmz" CACHE PATH "")

# scr not built by uberenv

# conduit from uberenv
set(CONDUIT_DIR "${TPL_ROOT}/conduit-0.3.1-qx6alxxlixyojppfatf3vmuqmv7nhvyz" CACHE PATH "")

# mfem from uberenv
set(MFEM_DIR "${TPL_ROOT}/mfem-3.3.2-pj7ivtzwtks2uatb72itorxbmz36c5x4" CACHE PATH "")

# python from uberenv
set(PYTHON_EXECUTABLE "${TPL_ROOT}/python-2.7.15-xa6i5xyxlh6amhwoz3qa5snnbt6xeorp/bin/python" CACHE PATH "")

# doxygen from uberenv
set(DOXYGEN_EXECUTABLE "${TPL_ROOT}/doxygen-1.8.11-3vwpxhyritdsxepkfikipivrjf67k3qf/bin/doxygen" CACHE PATH "")

# sphinx 1.4.5 from uberenv
set(SPHINX_EXECUTABLE "${TPL_ROOT}/python-2.7.15-xa6i5xyxlh6amhwoz3qa5snnbt6xeorp/bin/sphinx-build" CACHE PATH "")

# shroud 0.10.1 from uberenv
set(SHROUD_EXECUTABLE "${TPL_ROOT}/python-2.7.15-xa6i5xyxlh6amhwoz3qa5snnbt6xeorp/bin/shroud" CACHE PATH "")

# uncrustify from uberenv
set(UNCRUSTIFY_EXECUTABLE "${TPL_ROOT}/uncrustify-0.61-uiojma7mwdeu5rxko54iy7id5nghhx6q/bin/uncrustify" CACHE PATH "")

# lcov and genhtml from uberenv
set(LCOV_PATH "${TPL_ROOT}/lcov-1.11-r4v3ugaleai3raxqkrrcoyjtycy36ako/usr/bin/lcov" CACHE PATH "")

set(GENHTML_PATH "${TPL_ROOT}/lcov-1.11-r4v3ugaleai3raxqkrrcoyjtycy36ako/usr/bin/genhtml" CACHE PATH "")

##################################
# end uberenv host-config
##################################

##
## Copyright (c) 2017-2019, Lawrence Livermore National Security, LLC.
##
## Produced at the Lawrence Livermore National Laboratory.
##
## LLNL-CODE-741217
##
## All rights reserved.
##
## This file is part of Axom.
##
## For details about use and distribution, please read axom/LICENSE.
##

##############################################################################
# !---------------------------------------------------------------------------
##############################################################################
# Options added manually to
# lc chaos5 clang@3.9.1  host configs
##############################################################################

set(ENABLE_GTEST_DEATH_TESTS ON CACHE BOOL "")

##############################################################################
# MPI - manually added for now.
##############################################################################
set(ENABLE_MPI ON CACHE BOOL "")

set(MPI_HOME             "/usr/global/tools/clang/chaos_5_x86_64_ib/clang-3.9.1" CACHE PATH "")
set(MPI_C_COMPILER       "${MPI_HOME}/bin/mpiclang" CACHE PATH "")
set(MPI_CXX_COMPILER     "${MPI_HOME}/bin/mpiclang++" CACHE PATH "")

set(MPIEXEC              "/usr/bin/srun" CACHE PATH "")
set(MPIEXEC_NUMPROC_FLAG "-n" CACHE PATH "")

##############################################################################
# !---------------------------------------------------------------------------
##############################################################################

