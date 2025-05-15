.PHONY: all
all: server client

server: server.o helper.o
	gcc server.o helper.o -o server

client: client.o client_screen.o helper.o
	gcc client.o client_screen.o helper.o -lncurses -o client


	
server.o: server.c protocol.h user.h helper.h
	gcc -c server.c -o server.o

client.o: client.c screen.h user.h helper.h
	gcc -c client.c -o client.o

client_screen.o: client_screen.c screen.h
	gcc -c client_screen.c -o client_screen.o

helper.o: helper.c helper.h
	gcc -c helper.c -o helper.o

.PHONY: clean
clean:
	rm server client server.o client.o helper.o client_screen.o