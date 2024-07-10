CC = gcc
CFLAGS = -Wall -g

SDL_INCLUDE = -I./SDL2/include
SDL_LIB = -L./SDL2/lib -lSDL2

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))
TARGET = mine.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(SDL_LIB)

%.o: %.c
	$(CC) $(CFLAGS) $(SDL_INCLUDE) -c $< -o $@

# Clean up
clean:
	del $(OBJS) $(TARGET)

.PHONY: all clean
