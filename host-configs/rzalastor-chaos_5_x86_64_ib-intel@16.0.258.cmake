##################################
# uberenv host-config
#
# This is a generated file, edit at own risk.
##################################
# chaos_5_x86_64_ib-intel@16.0.258
##################################

# cmake from uberenv
# cmake executable path: /usr/workspace/wsrzc/axom/thirdparty_libs/builds/2018_08_07_19_23_24/spack/opt/spack/chaos_5_x86_64_ib/intel-16.0.258/cmake-3.9.6-j24h3r5gfpujuinjkyb7kc5rc6hrj3rf/bin/cmake

#######
# using intel@16.0.258 compiler spec
#######

# c compiler used by spack
set(CMAKE_C_COMPILER "/usr/local/tools/ic-16.0.258/bin/icc" CACHE PATH "")

# cpp compiler used by spack
set(CMAKE_CXX_COMPILER "/usr/local/tools/ic-16.0.258/bin/icpc" CACHE PATH "")

# fortran compiler used by spack
set(ENABLE_FORTRAN ON CACHE BOOL "")

set(CMAKE_Fortran_COMPILER "/usr/local/tools/ic-16.0.258/bin/ifort" CACHE PATH "")

# Root directory for generated TPLs
set(TPL_ROOT "/usr/workspace/wsrzc/axom/thirdparty_libs/builds/2018_08_07_19_23_24/spack/opt/spack/chaos_5_x86_64_ib/intel-16.0.258" CACHE PATH "")

# hdf5 from uberenv
set(HDF5_DIR "${TPL_ROOT}/hdf5-1.8.16-zvfopdsqd7w4vjwfswxp5utura6sr3cv" CACHE PATH "")

# scr not built by uberenv

# conduit from uberenv
set(CONDUIT_DIR "${TPL_ROOT}/conduit-0.3.1-dwjqno7rpgotp6bsp7s5d2tzetve7wrw" CACHE PATH "")

# mfem from uberenv
set(MFEM_DIR "${TPL_ROOT}/mfem-3.3.2-xbfzoiwzyh5kxekrxd2r4mmcimhg245t" CACHE PATH "")

# python from uberenv
set(PYTHON_EXECUTABLE "${TPL_ROOT}/python-2.7.15-4hsn5fph5zyhm4g2wxhs7u3fzufhuxem/bin/python" CACHE PATH "")

# doxygen from uberenv
set(DOXYGEN_EXECUTABLE "${TPL_ROOT}/doxygen-1.8.11-o5rgxsbcgi4q5iowclkvnl2tvdcqlho4/bin/doxygen" CACHE PATH "")

# sphinx 1.4.5 from uberenv
set(SPHINX_EXECUTABLE "${TPL_ROOT}/python-2.7.15-4hsn5fph5zyhm4g2wxhs7u3fzufhuxem/bin/sphinx-build" CACHE PATH "")

# shroud 0.10.1 from uberenv
set(SHROUD_EXECUTABLE "${TPL_ROOT}/python-2.7.15-4hsn5fph5zyhm4g2wxhs7u3fzufhuxem/bin/shroud" CACHE PATH "")

# uncrustify from uberenv
set(UNCRUSTIFY_EXECUTABLE "${TPL_ROOT}/uncrustify-0.61-53naixuibvacrffbdkwexmef7wgj5fyf/bin/uncrustify" CACHE PATH "")

# lcov and genhtml from uberenv
set(LCOV_PATH "${TPL_ROOT}/lcov-1.11-hwb6f57wogxsdgial7urrqaijbh6ufjz/usr/bin/lcov" CACHE PATH "")

set(GENHTML_PATH "${TPL_ROOT}/lcov-1.11-hwb6f57wogxsdgial7urrqaijbh6ufjz/usr/bin/genhtml" CACHE PATH "")

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
# lc chaos5 intel@16.0.258 host configs
##############################################################################

set(ENABLE_GTEST_DEATH_TESTS ON CACHE BOOL "")

# Set flags for intel to use a gcc standard library with C++11
set(GNU_PREFIX           "/usr/apps/gnu/4.9.3")
set(BLT_C_FLAGS          "-gnu-prefix=${GNU_PREFIX}/bin/" CACHE STRING "")
set(BLT_CXX_FLAGS        "-gnu-prefix=${GNU_PREFIX}/bin/" CACHE STRING "")
set(BLT_FORTRAN_FLAGS    "-gnu-prefix=${GNU_PREFIX}/bin/" CACHE STRING "")
set(BLT_EXE_LINKER_FLAGS "-Wl,-rpath,${GNU_PREFIX}/lib64" CACHE STRING "")

##############################################################################
# MPI - manually added for now
##############################################################################
set(ENABLE_MPI ON CACHE BOOL "")

set(MPI_HOME             "/usr/local/tools/mvapich2-intel-2.0" CACHE PATH "")
set(MPI_C_COMPILER       "${MPI_HOME}/bin/mpicc" CACHE PATH "")
set(MPI_CXX_COMPILER     "${MPI_HOME}/bin/mpicxx" CACHE PATH "")
set(MPI_Fortran_COMPILER "${MPI_HOME}/bin/mpif90" CACHE PATH "")

set(MPIEXEC              "/usr/bin/srun" CACHE PATH "")
set(MPIEXEC_NUMPROC_FLAG "-n" CACHE PATH "")

##############################################################################
# !---------------------------------------------------------------------------
##############################################################################

