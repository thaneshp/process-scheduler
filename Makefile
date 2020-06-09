# _*_ Makefile _*_

CC = gcc
CFLAGS = -lm -Wall -Wextra -std=gnu99 -I.
DEPS = process_defs.h ff_algorithm.h linked_list_queue.h
OBJ = scheduler.o
EXE = scheduler

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	/bin/rm $(OBJ)
	/bin/rm $(EXE)