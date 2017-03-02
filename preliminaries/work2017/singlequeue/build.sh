#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
DEVINCS="-I ./"
ELDF=${ESDK}/bsps/current/internal.ldf

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
e-gcc -O0 -g -T ${ELDF} e_SU.c -o bin/e_SU.elf ${DEVINCS} -le-lib -lm -ffast-math
e-gcc -O0 -g -T ${ELDF} e_CU.c -o bin/e_CU.elf ${DEVINCS} -le-lib -lm -ffast-math

# Convert ebinary to SREC file
#e-objcopy --srec-forceS3 --output-target srec bin/e_CU.elf bin/e_CU.srec
#e-objcopy --srec-forceS3 --output-target srec bin/e_SU.elf bin/e_SU.srec
