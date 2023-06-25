#!/bin/bash

TESTBINS=test/bin
TESTOBJS=test/obj

GCCOPTS="-pedantic -Wall -g -O2"

echo "" >test/logs

# build obj files
make obj/*.o &>>test/logs

mkdir ${TESTBINS} &>>test/logs
mkdir ${TESTOBJS} &>>test/logs

# mem.c
gcc $GCCOPTS -c test/mem.c -o ${TESTOBJS}/mem.o
gcc $GCCOPTS ${TESTOBJS}/mem.o obj/mem.o -o ${TESTBINS}/mem
./${TESTBINS}/mem

# read.c
gcc $GCCOPTS -c test/read.c -o ${TESTOBJS}/read.o
gcc $GCCOPTS ${TESTOBJS}/read.o obj/mem.o obj/read.o -o ${TESTBINS}/read
./${TESTBINS}/read
