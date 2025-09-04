#!/usr/bin/env bash
# Usage: ./run_all.sh
OUT=results.csv
echo "case,implementation,threads,rep,time_us,seed" > $OUT

SEED=12345
N=1000
M=10000

# Cases - (case_id,mMember,mInsert,mDelete)
declare -a cases=("1:0.99:0.005:0.005" "2:0.90:0.05:0.05" "3:0.50:0.25:0.25")

for c in "${cases[@]}"; do
  IFS=":" read case_id mMember mInsert mDelete <<< "$c"
  # Serial (single-thread)
  ./serial $N $M $mMember $mInsert $mDelete $SEED $case_id >> $OUT
  # Mutex and rwlock for 1,2,4,8 threads
  for T in 1 2 4 8; do
    ./mutex $N $M $mMember $mInsert $mDelete $T $SEED $case_id >> $OUT
    ./rwlock $N $M $mMember $mInsert $mDelete $T $SEED $case_id >> $OUT
  done
done
echo "done"
