CC=gcc
CLFAGS=-I.
OBJ=a2-ece650.o
all:a2-ece650
%.o:%.c
	$(CC) -c -o $@ $< $(CLFAGS)

a2-ece650:$(OBJ)
	$(CC) -o $@ $^ $(CLFAGS)

clean:
	rm -f *.o a2-ece650 
