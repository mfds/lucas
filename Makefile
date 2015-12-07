CC     := gcc

CFLAGS := -Wall -ansi -g $(shell sdl-config --cflags) $(shell xml2-config --cflags)
LDLIBS := $(shell sdl-config --libs) $(shell xml2-config --libs)

DEBUG  := 

OBJS   := main.o lucas_SDL.o param.o struct.o barrier.o

TARGET := lucas

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY : clean
clean:
	rm -f $(OBJS) *~ $(TARGET)