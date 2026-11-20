# Makefile for Task 02: File Allocation and Sizing Calculators
# Course: CSA04 OS | Student: S. Sachin (192525045) | SIMATS Engineering

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

all: file_allocation inode_calculator free_space_calculator

file_allocation: file_allocation.c
	$(CC) $(CFLAGS) file_allocation.c -o file_allocation

inode_calculator: inode_calculator.c
	$(CC) $(CFLAGS) inode_calculator.c -o inode_calculator

free_space_calculator: free_space_calculator.c
	$(CC) $(CFLAGS) free_space_calculator.c -o free_space_calculator

clean:
	rm -f file_allocation inode_calculator free_space_calculator *.o

.PHONY: all clean
