FLAGS = -Wall -g -D_REENTRANT
LINKER_FLAGS = -lpthread

all: clean worker

lasttweet.o: lasttweet.c
	gcc ${FLAGS} -c lasttweet.c

worker: worker.c lasttweet.o
	gcc ${LINKER_FLAGS} ${FLAGS} lasttweet.o worker.c -o worker

clean:
	rm -f lasttweet.o worker
