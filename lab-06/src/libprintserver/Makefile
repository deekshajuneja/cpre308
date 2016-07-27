

all:
	gcc -olibprintserver.o -Wall -g -fPIC -c print_server_client.c
	gcc -shared -Wl,-soname,libprintserver.so -o libprintserver.so libprintserver.o

