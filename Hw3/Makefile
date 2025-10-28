# Compiler to use
CC = gcc

# Compiler flags:
# -Wall = Show all warnings
# -g    = Include debug information (for GDB)
CFLAGS = -Wall -g

# Default target: 'make' or 'make all' will build both
all: PrintEntry HW3

# Rule to build the 'PrintEntry' executable
PrintEntry: PrintEntry.c
	$(CC) $(CFLAGS) -o PrintEntry PrintEntry.c

# Rule to build the 'HW3' executable
HW3: HW3.c
	$(CC) $(CFLAGS) -o HW3 HW3.c

# Rule to clean up the compiled executables
clean:
	rm -f PrintEntry HW3