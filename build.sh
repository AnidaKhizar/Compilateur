#!/bin/bash

gcc -Wall -Werror evm2.c -o evm2
gcc -Wall -Werror asm2.c -o asm2
./asm2 $1 $2
./evm2 $2
