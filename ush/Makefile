CC = gcc
CFLAGS = -g -Wall
TARGET = ush
#only this line needs to be changed with the .c file added
SRC = ush.c expand.c builtin.c
OBJS = $(SRC:.c=.o)
DEPS = builtin.h

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean

clean:
	rm -f $(TARGET) *.o
