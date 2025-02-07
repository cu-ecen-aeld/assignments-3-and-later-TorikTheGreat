# Define the compiler depending on whether the CROSS_COMPILE variable was passed
ifdef CROSS_COMPILE
	CC = $(CROSS_COMPILE)gcc
else
	CC = gcc
endif

SRC = writer.c
TARGET = writer

all:
	$(CC) $(SRC) -o $(TARGET)
clean:
	rm -f $(TARGET)
