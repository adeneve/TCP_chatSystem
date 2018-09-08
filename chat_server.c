#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "chat_server.h"

#define MAXQUEUE 50

void *handleConnection( void* clientSock);
int broadcast(char* buffer, char* name);
void saveClient(int sock);
void removeClient(int sock);

int clients[10];
int clientCount;
sem_t mutex;

int main(int argc, char* argv[]){

	memset(clients, 0 ,10);
	sem_init(&mutex, 0 ,1);

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
		//handle the connection in a separate thread
		pthread_t connectionThread;
		printf("starting new connection thread\n");
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
	printf("connection thread created\n");
	fflush(stdout);
	int* clientSock = (int*) clientSocket;
	char buffer [50];
	char name [50];
	while(1){
		int recvsize = recv(*clientSock, buffer, 50, 0);
		if(recvsize == 0) break;
		printf("message from %s :", buffer);
		memcpy(name, buffer, 50);
		memset(buffer, 0, 50);
		recvsize = recv(*clientSock, buffer, 50, 0);
		if(recvsize == 0) break;
		printf(" %s \n", buffer);
		fflush(stdout);
		broadcast(buffer, name);
		//broadcast to other connection threads
		memset(buffer, 0, 50);
	}
	printf("connection closed\n");
	close(*clientSock);
	removeClient(*clientSock);
 //handle client connection with this thread function
}

int broadcast(char *buffer, char* name){
	//iterate through the clients sending out the recieved message
	//although this is slow, it eliminates the need to store the messages
	//in a database to be queried by the clients later on.
	//in other words, this is a stateless chat system, nothing gets saved to disk
	//down semaphore - make sure broadcasts don't overlap
	//sem_wait(&mutex);
	for(int i = 0; i < 10; i++){
		if(clients[i] != 0){
			printf("name is %s \n", name);
			send(clients[i], name, strlen(name), 0);
			send(clients[i], buffer, strlen(buffer), 0);
		}
	}
	//up semaphore
	//sem_post(&mutex);
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
