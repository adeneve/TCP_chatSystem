#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "chat_server.h"

#define MAXQUEUE 10

void *handleConnection( void* clientSock);
int broadcast(char* buffer, char* name);
int saveClient(int sock);
void removeClient(int sock);

int clients[10];
int clientCount;
sem_t mutex;

int main(int argc, char* argv[]){

	memset(clients, 0 ,10);
	sem_init(&mutex, 0 ,1);
	int clientCount = 0;

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
	int clientSock;

	while(1){
		//listen for a connection
		struct sockaddr clientAddr;
		int clntLen = sizeof(clientAddr);
		if(clientCount == 10){
			printf("max connections reached");
			usleep(200000); // sleep 200ms
			continue;
		}
		if((clientSock=accept(servSock, (struct sockaddr*)&clientAddr,&clntLen)) < 0){
			printf("problem with accept()");
			fflush(stdout);
			return 1;
		}
		//find available slot in client array
		int slotNum = saveClient(clientSock);
		//handle the connection in a separate thread
		pthread_t connectionThread;
		printf("starting new connection thread\n");
		pthread_create(&connectionThread, NULL, handleConnection, (void*) &clients[slotNum]);
		pthread_detach(connectionThread);
		fflush(stdout);

		clientCount++;

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
	char name [20];
	while(1){
		printf("waiting for message from %d ...\n", *clientSock);
		int recvsize = recv(*clientSock, buffer, 20, 0);
		if(recvsize == 0) break;
		printf("message from %s :", buffer);
		fflush(stdout);
		memcpy(name, buffer, 20);
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
	sem_wait(&mutex);
	for(int i = 0; i < 10; i++){
		if(clients[i] != 0){
			printf("name is %s \n", name);
			if(send(clients[i], name, 20, 0) < 0){
				printf("send error");
			}
			if(send(clients[i], buffer, 50, 0)< 0){
				printf("send error");
			}
		}
	}
	//up semaphore
	sem_post(&mutex);
 return 0;
}

int saveClient( int clientSock ){

	int i;
	for(i = 0; i < 10; i++){
		if(clients[i] == 0) break;
	}
	clients[i] = clientSock;

	return i;
}

void removeClient(int clientSock){
	int i;
	for(i = 0; i < 10; i++){
		if(clients[i] == clientSock) break;
	}
	clients[i] = 0;
	clientCount--;
}
