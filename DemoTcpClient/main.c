#include "stdio.h"
#include "winsock2.h"
#include "ws2tcpip.h"

#pragma comment(lib, "ws2_32.lib") //Winsock Library

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char server_reply[32];
	int recv_size;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code: %d\n", WSAGetLastError());
		return 1;
	}

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket: %d\n", WSAGetLastError());
		return 1;
	}

	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(65454);

	//Connect to remote server
	if (connect(s, (struct sockaddr*) &server, sizeof(server)) < 0)
	{
		printf("Could not connect: %d\n", WSAGetLastError());
		return 1;
	}

	while (1)
	{
		//Receive a reply from the server
		if ((recv_size = recv(s, server_reply, 30, 0)) == SOCKET_ERROR)
		{
			printf("Could not receive data\n");
		}

		printf("Got %d bytes:", recv_size);
		for (int i = 0; i < recv_size; ++i)
		{
			printf(" %d", server_reply[i]);
		}
		printf("\n");
	}

	closesocket(s);
	WSACleanup();

	return 0;
}