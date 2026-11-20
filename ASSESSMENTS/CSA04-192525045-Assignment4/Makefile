# Global Makefile for CSA04 Assessment 4 (CO4)
# Student: S. Sachin (192525045)

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

.PHONY: all clean test task02 task04 task06

all: task02 task04 task06

task02:
	@echo "=== Building Task 02 Programs ==="
	$(MAKE) -C Task02 all

task04:
	@echo "=== Building Task 04 Programs ==="
	$(MAKE) -C Task04 all

task06:
	@echo "=== Building Task 06 Programs ==="
	$(MAKE) -C Task06 all

test: all
	@echo "=== Testing Task 02 ==="
	@cd Task02 && ./file_allocation
	@echo "=== Testing Task 04 ==="
	@cd Task04 && ./disk_scheduling
	@echo "=== Testing Task 06 ==="
	@cd Task06 && ./storage_simulator

clean:
	@echo "=== Cleaning All Tasks ==="
	$(MAKE) -C Task02 clean
	$(MAKE) -C Task04 clean
	$(MAKE) -C Task06 clean
