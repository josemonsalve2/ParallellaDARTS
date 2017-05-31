#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
DEVINCS="-I ./ -L ./"
#ELDF=${ESDK}/bsps/current/internal.ldf
ELDF=fast_cacheman.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

# Create the binaries directory
mkdir -p bin/

CROSS_PREFIX=
case $(uname -p) in
	arm*)
		# Use native arm compiler (no cross prefix)
		CROSS_PREFIX=
		;;
	   *)
		# Use cross compiler
		CROSS_PREFIX="arm-linux-gnueabihf-"
		;;
esac

# Build HOST side application
${CROSS_PREFIX}gcc main.c -g -o bin/main.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program
echo "e-gcc -O0 -g -ffast-math -fpic -o bin/e_SU.elf e_SU.c -T ${ELDF} ${DEVINCS} -lm -le-lib"
echo "e-gcc -O0 -g -ffast-math -fpic -o bin/e_CU.elf e_CU.c -T ${ELDF} ${DEVINCS} -lm -le-lib"

e-gcc -O0 -g -ffast-math -fpic -o bin/e_SU.elf e_SU.c -T ${ELDF} ${DEVINCS} -lm -le-lib
e-gcc -O0 -g -ffast-math -fpic -o bin/e_CU.elf e_CU.c -T ${ELDF} ${DEVINCS} -lm -le-lib

# Convert ebinary to SREC file
#e-objcopy --srec-forceS3 --output-target srec bin/e_CU.elf bin/e_CU.srec
#e-objcopy --srec-forceS3 --output-target srec bin/e_SU.elf bin/e_SU.srec
