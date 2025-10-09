TARGET = Lab6

SRC = $(TARGET).c

CC = gcc

CFLAGS = -Wall -g

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) *.o
