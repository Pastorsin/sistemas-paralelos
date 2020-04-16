#!/bin/bash

OPTIONS=("SECUENCIA" "PRIMOS" "PARES" "IMPARES" "UNIFORME_CRECIENTE" "UNIFORME_DECRECIENTE" "ALEATORIA") 

gcc merge_multiple.c -o merge.out

for number_of_lists in 512 1024 2048
do
	echo "--NUMBER OF LISTS : $number_of_lists"

	for option in ${OPTIONS[@]}
	do
		echo "--- option : $option"

		./merge.out $number_of_lists $option 512 1024

	done
done
