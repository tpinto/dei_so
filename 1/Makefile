all : clean client daemon

client : client.c
	gcc -Wall client.c -o client

daemon : daemon.c
	gcc -Wall daemon.c -o daemon

clean :
	rm -f client daemon