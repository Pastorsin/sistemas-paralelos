#!/bin/bash

gcc -o multBloques multBloques.c
if [ ! -d "outputs" ]; then
  mkdir outputs
fi
cp multBloques outputs

cd outputs
echo " ================= Algoritmo con Bloques =================== "
for matriz in 512 1024
do
  echo " ==================================================== Matriz $((matriz)) "
  tamBloque=$matriz
  for (( cantBloques=1; cantBloques<=matriz; cantBloques=cantBloques*2 ))
  do
    echo " -------------------------- Cantidad bloques -> $cantBloques Tamano bloque -> $tamBloque"
    ./multBloques $cantBloques $tamBloque 0
    tamBloque=$((tamBloque/2))
  done
done
