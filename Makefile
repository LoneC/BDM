CC = gcc
CFLAGS = -I/usr/lib/ -Wall -Wpedantic -Wno-unused -O3
LFLAGS = -lncurses -lpam 
TARGET = bdm

SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

all: $(OBJECTS) $(TARGET) clean

$(OBJECTS): $(SOURCES)
	$(CC) -c $(SOURCES) $(CFLAGS)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LFLAGS)

clean:
	rm *.o
