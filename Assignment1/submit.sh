#!/bin/bash
#SBATCH --job-name=a1_dlowrey
#SBATCH --partition=Centaurus
#SBATCH --time=00:10:00
#SBATCH --mem=32G
make
./ArrayMaximum
./ExclusivePrefixSum
./MatrixMultiplication
