#!/bin/bash

echo "$1 $2" > times

ARRAY=(1 2 4 8)

export OMP_NUM_THREADS=1
for i in $(seq 1 $1)
do
  #export OMP_NUM_THREADS=${ARRAY[$i]}

  for j in $(seq 1 $2)
  do
    #times=`./knn data/training_set data/testing_set 1 | awk -F ' ' '{ print $3 }' | tail -n 3 | sed 's| ms||g'`
    times=`./knn data/$i/train_set_$i data/$i/val_set_$i 1 | awk -F ' ' '{ print $3 }' | tail -n 7 | head -n 3 | sed 's| ms||g'`
    echo $times >> times
    echo "  " $times
  done
done

#rm -rf graphs
mkdir -p graphs
mv times graphs
python script/gen_graph.py graphs/times
mv *.png graphs
