#!/bin/sh
#PBS -q plgrid-testing
#PBS -l walltime=0:30:00
#PBS -l nodes=2:ppn=12
#PBS -A plgkazala2017a

cd ~/my_tests

module load plgrid/libs/boost/1.59.0

#kompilacja
mpicc -o ssend ssend.c
mpicc -o bsend bsend.c


#obliczenia
mpiexec -n 2 ./ssend > ssend_results.out
mpiexec -n 2 ./bsend > bsend_results.out


#kompilacja
mpicc -o ssend_lat ssend_lat.c
mpicc -o bsend_lat bsend_lat.c


#obliczenia
mpiexec -n 2 ./ssend_lat > ssend_lat_results.out
mpiexec -n 2 ./bsend_lat > bsend_lat_results.out