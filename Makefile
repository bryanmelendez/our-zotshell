CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = zotshell 
SRC = zotshell.c

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean build files
clean:
	rm -f $(TARGET)

debug:
	make
	gdb ./zotshell
