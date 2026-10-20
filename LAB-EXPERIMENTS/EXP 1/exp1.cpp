#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int main()
{
#ifdef _WIN32
    printf("Process ID: %lu\n", (unsigned long)GetCurrentProcessId());
    printf("Parent Process ID: %lu\n",
           (unsigned long)GetCurrentProcessId());
#else
    printf("Process ID: %d\n", getpid());
    printf("Parent Process ID: %d\n", getppid());
#endif

    return 0;
}
