# Assignment 2 Makefile
#
# Run from the project's top folder:
#   make          builds both programs
#   make partA    builds the pthreads program (Part A)
#   make partB    builds the OpenMP program (Part B)
#   make clean    removes the built programs
#
# The compile flags follow the assignment: gcc -O2 -pthread (Part A) and
# gcc -O2 -fopenmp (Part B). -lm links the math library, which compareCees
# needs for fmax and fabs.
#
# On macOS, Apple's "gcc" is clang and has no OpenMP support. To build Part B
# locally with Homebrew GCC, override the compiler and add the SDK path, e.g.:
#   make partB CC=gcc-16 OMPFLAGS="-O2 -fopenmp -isysroot $(xcrun --show-sdk-path)"

CC       = gcc
PTHFLAGS = -O2 -pthread
OMPFLAGS = -O2 -fopenmp
LDLIBS   = -lm

PARTA_SRC = src/partA/pthreadmatrixmult.c
PARTB_SRC = src/partB/ompmatrixmult.c

.PHONY: all clean

all: partA partB

partA: $(PARTA_SRC) src/partA/assignment2.h
	$(CC) $(PTHFLAGS) $(PARTA_SRC) -o partA $(LDLIBS)

partB: $(PARTB_SRC) src/partB/assignment2.h
	$(CC) $(OMPFLAGS) $(PARTB_SRC) -o partB $(LDLIBS)

clean:
	rm -f partA partB