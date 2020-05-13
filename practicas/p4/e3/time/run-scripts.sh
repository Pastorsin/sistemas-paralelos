#!/bin/bash

THREADS=(4 8 16)
ARRAY_SIZES=(10000000 20000000 30000000)

for size in ${ARRAY_SIZES[@]};
do
	echo "------------------------------------"
	echo "N=$size"

	for thread in ${THREADS[@]};
	do
		echo "--------------------"
		echo "-- Blocking Ring P=$thread"
		mpirun --oversubscribe -np $thread blocking.out $size | grep comunicacion

		echo "--------------------"
		echo "-- NON Blocking Ring P=$thread"
		mpirun --oversubscribe -np $thread non-blocking.out $size | grep comunicacion
	done
done
