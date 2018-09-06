#include <stdlib.h>
#include <stdio.h>
#include "chat_server.h"
#include <arpa/inet.h>

int main(int argc, char* argv []){

	int clientSock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	const char* serverIP = "127.0.0.1";
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	serverAddr.sin_port = htons(1234);

	int resolve = connect(clientSock, (struct sockaddr*) &serverAddr,
			sizeof(serverAddr));
	if(resolve < 0){
		printf("problem connecting to server");
		return 1;
	}

	char buffer[50];
	int counter = 0;
	char nextC;
	while(1){
		
		//scanf("%s", buffer);
		while(1){
			char returnChar = '\n';
			read(STDIN_FILENO, &nextC, 1);
			if(returnChar == nextC) break;
			printf("next char..");
			fflush(stdout);
			buffer[counter] = nextC;
			counter++;
		}
		buffer[counter] = '\0';
		counter=0;
		printf("message complete");
		fflush(stdout);
		int strlength = strlen(buffer);
		if(send(clientSock, buffer, strlength, 0) < 0){
			printf("send error");
			return 1;
		}
		memset(buffer, 0, 50);
		recv(clientSock, buffer, 50, 0);
		printf("%s \n", buffer);
		fflush(stdout);

	}
	return 0;

}
