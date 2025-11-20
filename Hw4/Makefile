CC = gcc
CFLAGS = -Wall -O2 -g
LDLIBS = -lpthread
TARGET = Hw4
SRCS = Hw4.c

.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: run
run: all
	./$(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) output.txt