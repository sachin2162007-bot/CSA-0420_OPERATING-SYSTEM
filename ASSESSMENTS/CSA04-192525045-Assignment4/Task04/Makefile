CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

all: disk_scheduling disk_scheduling_simulator

disk_scheduling: disk_scheduling.c
	$(CC) $(CFLAGS) disk_scheduling.c -o disk_scheduling

disk_scheduling_simulator: disk_scheduling_simulator.c
	$(CC) $(CFLAGS) disk_scheduling_simulator.c -o disk_scheduling_simulator -lm

clean:
	rm -f disk_scheduling disk_scheduling_simulator *.o
