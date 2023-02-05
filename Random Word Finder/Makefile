CC=gcc
CFLAGS=-I. -w -pthread
OBJ = wordpuzzle.o randomgen.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chess: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
