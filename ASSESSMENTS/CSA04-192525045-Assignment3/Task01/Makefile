# Makefile for Task 01: Linking, Loading and Address Binding
# Student: S. Sachin | Reg: 192525045 | CSA04 CO3

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDFLAGS_RUNTIME = -ldl
RPATH = -Wl,-rpath,.

all: libcrypto_demo.a libcrypto_demo.so crypto_static crypto_shared crypto_runtime static_dynamic_comparison

# 1. Compile Position-Independent Code (PIC) for Shared Object
libcrypto_demo.o: libcrypto_demo.c libcrypto_demo.h
	$(CC) $(CFLAGS) -fPIC -c libcrypto_demo.c -o libcrypto_demo.o

# 2. Build Shared Library (.so)
libcrypto_demo.so: libcrypto_demo.o
	$(CC) -shared -o libcrypto_demo.so libcrypto_demo.o

# 3. Build Static Library (.a)
libcrypto_demo.a: libcrypto_demo.c libcrypto_demo.h
	$(CC) $(CFLAGS) -c libcrypto_demo.c -o libcrypto_static.o
	ar rcs libcrypto_demo.a libcrypto_static.o

# 4. Build Static Executable
crypto_static: crypto_static.c libcrypto_demo.a
	$(CC) $(CFLAGS) crypto_static.c libcrypto_demo.a -o crypto_static

# 5. Build Dynamic Shared Executable
crypto_shared: crypto_shared.c libcrypto_demo.so
	$(CC) $(CFLAGS) crypto_shared.c -L. -lcrypto_demo $(RPATH) -o crypto_shared

# 6. Build Runtime Loading Executable (dlopen)
crypto_runtime: crypto_runtime.c
	$(CC) $(CFLAGS) crypto_runtime.c $(LDFLAGS_RUNTIME) -o crypto_runtime

# 7. Build Comparison Tool
static_dynamic_comparison: static_dynamic_comparison.c
	$(CC) $(CFLAGS) static_dynamic_comparison.c -o static_dynamic_comparison

# Target to demonstrate hot-patching with patched library v2.0
patch_v2:
	$(CC) $(CFLAGS) -fPIC -DCRYPTO_VERSION_STRING='\"v2.0.0-Security-Patched-AES\"' -shared -o libcrypto_demo.so libcrypto_demo.c
	@echo "Patched libcrypto_demo.so to v2.0.0 without touching executables!"

clean:
	rm -f *.o *.a *.so crypto_static crypto_shared crypto_runtime static_dynamic_comparison

.PHONY: all clean patch_v2
