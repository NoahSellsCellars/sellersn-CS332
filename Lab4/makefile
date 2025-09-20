TARGET = FileConcatenate

SRC = $(TARGET).c

CC = gcc

CFLAGS = -Wall -g

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -0 $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) *.o
