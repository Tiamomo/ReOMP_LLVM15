#!/bin/sh
####################################################################################
# Copyright (c) 2016, Lawrence Livermore National Security, LLC. 
# Produced at the Lawrence Livermore National Laboratory. 
#
# Written by Kento Sato, kento@llnl.gov. LLNL-CODE-711357. 
# All rights reserved.
#
# This file is part of ReMPI. For details, see https://github.com/PRUNER/ReMPI
# Please also see the LICENSE file for our notice and the LGPL.
#
# This program is free software; you can redistribute it and/or modify it under the 
# terms of the GNU General Public License (as published by the Free Software 
# Foundation) version 2.1 dated February 1999.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY 
# WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU 
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License along 
# with this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
# Place, Suite 330, Boston, MA 02111-1307 USA
####################################################################################



num_procs=$1
dir=.rempi


librempi=/home/msm924/reomp/lib/librempi.so

cd ../test
mkdir ${dir}
# ===== Unit test ======== 
bin=./rempi_test_units
REMPI_MODE=0 REMPI_DIR=${dir} REMPI_ENCODE=0 REMPI_GZIP=0 REMPI_TEST_ID=0 LD_PRELOAD=${librempi} srun -n ${num_procs} ${bin}
REMPI_MODE=1 REMPI_DIR=${dir} REMPI_ENCODE=0 REMPI_GZIP=0 REMPI_TEST_ID=0 LD_PRELOAD=${librempi} srun -n ${num_procs} ${bin}
srun rm ${dir}/* 2> /dev/null


# ===== Hang + io-watchdog ======== 
bin="./rempi_test_msg_race 0 1 10000 2 0"
REMPI_MODE=0 REMPI_DIR=${dir} REMPI_ENCODE=0 REMPI_GZIP=1 REMPI_TEST_ID=0 LD_PRELOAD=${librempi} srun --io-watchdog=conf=.io-watchdogrc -n ${num_procs} ${bin}
REMPI_MODE=1 REMPI_DIR=${dir} REMPI_ENCODE=0 REMPI_GZIP=1 REMPI_TEST_ID=0 LD_PRELOAD=${librempi} srun --io-watchdog=conf=.io-watchdogrc -n ${num_procs} ${bin}
srun rm ${dir}/* 2> /dev/null

# ===== MPI_Abort() + io-watchdog ======== 
bin="./rempi_test_msg_race 0 2 10000 2 0"
REMPI_MODE=0 REMPI_DIR=${dir} REMPI_ENCODE=0 REMPI_GZIP=1 REMPI_TEST_ID=0 LD_PRELOAD=${librempi} srun --io-watchdog=conf=.io-watchdogrc -n ${num_procs} ${bin}
REMPI_MODE=1 REMPI_DIR=${dir} REMPI_ENCODE=0 REMPI_GZIP=1 REMPI_TEST_ID=0 LD_PRELOAD=${librempi} srun --io-watchdog=conf=.io-watchdogrc -n ${num_procs} ${bin}
srun rm ${dir}/* 2> /dev/null

# ===== Segmentation Fault + io-watchdog ======== 
bin="./rempi_test_msg_race 0 3 10000 2 0"
REMPI_MODE=0 REMPI_DIR=${dir} REMPI_ENCODE=0 REMPI_GZIP=1 REMPI_TEST_ID=0 LD_PRELOAD=${librempi} srun --io-watchdog=conf=.io-watchdogrc -n ${num_procs} ${bin}
REMPI_MODE=1 REMPI_DIR=${dir} REMPI_ENCODE=0 REMPI_GZIP=1 REMPI_TEST_ID=0 LD_PRELOAD=${librempi} srun --io-watchdog=conf=.io-watchdogrc -n ${num_procs} ${bin}
srun rm ${dir}/* 2> /dev/null
