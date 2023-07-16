#!/bin/bash

TESTBINS=test/bin
TESTOBJS=test/obj

GCCOPTS="-pedantic -Wall -g -O2"

echo "" >test/logs

mkdir ${TESTBINS} &>>test/logs
mkdir ${TESTOBJS} &>>test/logs

# mem.c
make clean
MEMDEPS="obj/mem.o obj/constants.o"
make ${MEMDEPS}
gcc $GCCOPTS -c test/mem.c -o ${TESTOBJS}/mem.o                     \
    && gcc $GCCOPTS ${TESTOBJS}/mem.o ${MEMDEPS} -o ${TESTBINS}/mem \
    && ./${TESTBINS}/mem                                            \
    || echo "mem.c test crashed or didn't build"

## read.c
make clean
READDEPS="obj/constants.o obj/mem.o obj/read.o"
make EXTRAFLAGS="-DALWAYS_GC" ${READDEPS}
gcc $GCCOPTS -c test/read.c -o ${TESTOBJS}/read.o                      \
    && gcc $GCCOPTS ${TESTOBJS}/read.o ${READDEPS} -o ${TESTBINS}/read \
    && ./${TESTBINS}/read                                              \
    || echo "read.c test crashed or didn't build"

# env.c
make clean
ENVDEPS="obj/constants.o obj/mem.o obj/env.o"
make EXTRAFLAGS="-DALWAYS_GC" ${ENVDEPS}
gcc $GCCOPTS -c test/env.c -o ${TESTOBJS}/env.o                     \
    && gcc $GCCOPTS ${TESTOBJS}/env.o ${ENVDEPS} -o ${TESTBINS}/env \
    && ./${TESTBINS}/env                                            \
    || echo "env.c test crashed or didn't build"

# apply.c
make clean
ENVDEPS="obj/constants.o obj/mem.o obj/env.o obj/apply.o"
make EXTRAFLAGS="-DALWAYS_GC" ${ENVDEPS}
gcc $GCCOPTS -c test/apply.c -o ${TESTOBJS}/apply.o                     \
    && gcc $GCCOPTS ${TESTOBJS}/apply.o ${ENVDEPS} -o ${TESTBINS}/apply \
    && ./${TESTBINS}/apply                                              \
    || echo "apply.c test crashed or didn't build"

## eval.c
#gcc $GCCOPTS -c test/eval.c -o ${TESTOBJS}/eval.o
#gcc $GCCOPTS ${TESTOBJS}/eval.o obj/error.o obj/constants.o obj/apply.o obj/mem.o obj/env.o obj/eval.o -o ${TESTBINS}/eval
#./${TESTBINS}/eval
#
## defs.c
#gcc $GCCOPTS -c test/defs.c -o ${TESTOBJS}/defs.o
#gcc $GCCOPTS ${TESTOBJS}/defs.o obj/error.o obj/apply.o obj/mem.o obj/env.o obj/eval.o obj/constants.o obj/defs.o -o ${TESTBINS}/defs
#./${TESTBINS}/defs
