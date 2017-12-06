#!/bin/sh

for((i = 1; i <= 9; ++i))
do
  echo "set$i"
  mkdir run000$i
done

for((i= 10; i <= 49; ++i))
do
  echo "set$i"
  mkdir run00$i
done 

