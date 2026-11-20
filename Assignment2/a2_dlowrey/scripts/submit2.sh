#!/bin/bash
#SBATCH --job-name=a2_801352577
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=36
#SBATCH --exclusive
#SBATCH --time=16:00:00
#SBATCH --output=data/run_%j.out
#SBATCH --partition=Centaurus

set -e                     
cd "$SLURM_SUBMIT_DIR" 

hostname
lscpu
gcc --version
mkdir -p data
rm -f data/pth_*.csv data/omp_*.csv data/b_star_holder.txt
make clean
make

SIZES="1024 2048"
PS="1 2 4 8 12 18 36"
GRIDS="1x2 2x2 2x4 3x4 3x6 6x6"     # pr x pc for the square grids

./partA 0 1024 1024 1024
./partA 0 2048 2048 2048

BSTAR=$(cat data/b_star_holder.txt)
if [ "$BSTAR" -eq 0 ]; then
    TILES="0"
else
    TILES="0 $BSTAR"
fi

for s in $SIZES; do
    for b in $TILES; do
        for p in $PS; do
            ./partA 1 $s $s $s $b $p
        done
    done
done

for s in $SIZES; do
    for g in $GRIDS; do
        pr=${g%x*}         
        pc=${g#*x}          
        ./partA 2 $s $s $s $BSTAR $pr $pc
    done
done
./partA 2 2048 2048 2048 $BSTAR 1 36
./partA 2 1000 1024 1536 $BSTAR 6 6


export OMP_PROC_BIND=true
export OMP_PLACES=cores
echo "OMP_PROC_BIND=$OMP_PROC_BIND OMP_PLACES=$OMP_PLACES"
for s in $SIZES; do
    for b in $TILES; do
        ./partB 0 $s $s $s $b
    done
done
for s in $SIZES; do
    for b in $TILES; do
        for p in $PS; do
            ./partB 1 $s $s $s $b $p
        done
    done
done
for s in $SIZES; do
    for g in $GRIDS; do
        pr=${g%x*}
        pc=${g#*x}
        ./partB 2 $s $s $s $BSTAR $pr $pc
    done
done
./partB 2 2048 2048 2048 $BSTAR 1 36
./partB 2 1000 1024 1536 $BSTAR 6 6
python3 scripts/plot.py || echo "Plotting failed"

