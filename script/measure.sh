#!/bin/bash

echo "$1 $2" > times

for i in $(seq 1 $1)
do
  export OMP_NUM_THREADS=$i
  echo "Num. of Threads: $i"

  for j in $(seq 1 $2)
  do
    #times=`./knn data/training_set data/testing_set 1 | awk -F ' ' '{ print $3 }' | tail -n 3 | sed 's| ms||g'`
    times=`./knn data/training_set data/testing_set 1 | awk -F ' ' '{ print $3 }' | tail -n 7 | head -n 3 | sed 's| ms||g'`
    echo $times >> times
    echo "  " $times
  done
done

#rm -rf graphs
mkdir -p graphs
mv times graphs
python script/gen_graph.py graphs/times
mv *.png graphs
