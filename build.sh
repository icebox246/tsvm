#!/bin/sh

gcc tsvm-ass.c -o tsvm-ass -ggdb &
gcc tsvm-run.c -o tsvm-run -ggdb &
gcc tsvm-gensyntax.c -o tsvm-gensyntax -ggdb &
wait

[ -d vim/syntax ] || mkdir vim/syntax
./tsvm-gensyntax
