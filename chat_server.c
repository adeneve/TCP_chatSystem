#include <stdlib.h>
#include <stdio.h>
#include "chat_server.h"

#define MAXQUEUE 50

void *handleConnection( void* clientSock);
int clients[10];
int clientCount;

int main(int argc, char* argv[]){

	memset(clients, 0 ,10);

	int servSock;
	if((servSock = socket(PF_INET, SOCK_STREAM, 0)) < 0 ){
		printf("failed to create server socket");
	}
	
	if(bind_TCPserver_sock(servSock, 1234, AF_INET) == -1){
		printf("failed to bind sock");
	}

	if((listen(servSock, MAXQUEUE)) < 0){
		printf("listen failed");
	}
	fprintf(stdout, "server startup complete. \n");
	fflush(stdout);
	while(1){
		//listen for a connection
		struct sockaddr clientAddr;
		int clntLen = sizeof(clientAddr);
		int clientSock;
		if((clientSock=accept(servSock, (struct sockaddr*)&clientAddr,&clntLen)) < 0){
			printf("problem with accept()");
			fflush(stdout);
			return 1;
		}
		pthread_t connectionThread;
		printf("starting new thread");
		pthread_create(&connectionThread, NULL, handleConnection, (void*) &clientSock);
		pthread_detach(connectionThread);
		fflush(stdout);
		saveClient(clientSock);

	}

	return 1;

}

int bind_TCPserver_sock(int sockid, int port, int family){

	struct sockaddr_in addrport;
	addrport.sin_family = family;
	addrport.sin_port = htons(port);
	addrport.sin_addr.s_addr = htonl(INADDR_ANY);

	return bind(sockid, (struct sockaddr*) &addrport, sizeof(addrport));
}

void *handleConnection(void* clientSocket)
{
	printf("new connection");
	fflush(stdout);
	int* clientSock = (int*) clientSocket;
	char buffer [50];
	while(1){
		int recvsize = recv(*clientSock, buffer, 50, 0);
		if(recvsize == 0) break;
		printf("message recieved: %s \n", buffer);
		fflush(stdout);
		//broadcast to other connection threads
		memset(buffer, 0, 50);
		buffer[0] = 'm'; buffer[1] = 's'; buffer[2] = '\0';
		if(send(*clientSock, buffer, 3, 0) < 0){
			printf("send error");
			return 1;
		}
	}
	printf("connection closed\n");
 //handle client connection with this thread function
}

int broadcast(char *buffer, char* name){
 return 0;
}

void saveClient( int clientSock ){

	int i;
	for(i = 0; i < 10; i++){
		if(clients[i] == 0) break;
	}
	clients[i] = clientSock;
}

void removeClient(int clientSock){
	int i;
	for(i = 0; i < 10; i++){
		if(clients[i] == clientSock) break;
	}
	clients[i] = 0;
}
