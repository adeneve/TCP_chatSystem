CC=gcc -std=c99
CFLAGS=-I.
HEADERS = chat_server.h chat_client.h

%.o : %.c
	$(CC) -c  $< $(CFLAGS) 

servermake: chat_server.o chat_client.o
	$(CC) -o chat_server chat_server.o -lpthread
	$(CC) -o chat_client chat_client.o -lpthread

all: clientmake servermake

clean:
	rm *.o chat_server chat_client
