CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

all: storage_simulator disk_workload_simulator

storage_simulator: storage_simulator.c
	$(CC) $(CFLAGS) storage_simulator.c -o storage_simulator -lm

disk_workload_simulator: disk_workload_simulator.c
	$(CC) $(CFLAGS) disk_workload_simulator.c -o disk_workload_simulator -lm

clean:
	rm -f storage_simulator disk_workload_simulator *.o
