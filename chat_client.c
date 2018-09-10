#include <stdlib.h>
#include <stdio.h>
#include "chat_server.h"
#include <arpa/inet.h>

pthread_t receiverThread;
void* receiverFun(void* clientSock);

int main(int argc, char* argv []){

	int clientSock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	const char* serverIP = "127.0.0.1";
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	serverAddr.sin_port = htons(1234);

	int resolve = connect(clientSock, (struct sockaddr*) &serverAddr,
			sizeof(serverAddr));
	if(resolve < 0){
		printf("problem connecting to server: %d", resolve);
		return 1;
	}

	//start a reciever thread
	printf("starting receiver thread\n");
	pthread_create(&receiverThread, NULL, receiverFun, (void*)&clientSock);
        pthread_detach(receiverThread);

	char buffer[50];
	char name[20];
	printf("enter your name: " );
	fflush(stdout);
	scanf("%s", name);
	int counter = 0;
	char nextC;
	while(1){

		//scanf("%s", buffer);
		//use a semaphore here for nicer printing
		printf("> ");
		fflush(stdout);
		while(1){
			char returnChar = '\n';
			read(STDIN_FILENO, &nextC, 1);
			if(returnChar == nextC) break;
			//printf("next char..");
			fflush(stdout);
			buffer[counter] = nextC;
			counter++;
		}
		buffer[counter] = '\0';
		counter=0;
		int strlength = strlen(buffer);
		if(send(clientSock, name, strlen(name), 0) < 0){
			printf("send error");
			return 1;
		}
		if(send(clientSock, buffer, strlength, 0) < 0){
			printf("send error");
			return 1;
		}
		//recv(clientSock, buffer, 50, 0);
		printf("message sent");
		fflush(stdout);
	}
	return 0;

}

void* receiverFun(void* clientSock){

	char buffer [50];
	while(1){

	  int recvsize = recv(*(int*)clientSock, buffer, 20, 0);
	  if(recvsize == 0) break;
	  printf("message from server - %s : ", buffer);
	  memset(buffer, 0 ,50);
	  recvsize = recv(*(int*)clientSock, buffer, 50, 0);
	  if(recvsize == 0) break;
	  printf(" %s \n", buffer);
	  memset(buffer,0,50);

	}

}
