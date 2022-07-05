EXEC1	= encode
EXEC2	= decode
EXEC3	= entropy

CC	= clang
CFLAGS	= -Wall -Werror -Wpedantic -Wextra
LFLAGS	= -lm
SRC	= io.c node.c pq.c code.c stack.c huffman.c
OBJ1	= $(EXEC1).o $(SRC:%.c=%.o)
OBJ2	= $(EXEC2).o $(SRC:%.c=%.o)
OBJ3	= $(EXEC3).o

.PHONY: all format clean

all: $(EXEC1) $(EXEC2) $(EXEC3)

$(EXEC1):  $(OBJ1)
	$(CC) -o $@ $^

$(EXEC2): $(OBJ2)
	$(CC) -o $@ $^

$(EXEC3): $(OBJ3)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

format:
	clang-format -i -style=file *.[ch]

clean:
	rm -rf $(EXEC1) $(EXEC2) $(EXEC3) $(wildcard *.o)
