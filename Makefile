#
# Copyright (c) 2015 Sergi Granell (xerpi)
#

TARGET = vitareadelf
OBJS = main.o elf.o

CFLAGS = -Wall
LIBS =

all: $(TARGET)

$(TARGET): $(OBJS)
	gcc -o $@ $^ $(LIBS)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

clean:
	@rm -f $(OBJS) $(TARGET)
