#include "stdio.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "queue.h"

#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define MAX_PROTOCOL_LENGTH 10

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char server_reply[MAX_PROTOCOL_LENGTH];
	int recv_size;
	queue_t* queue = create_queue(MAX_PROTOCOL_LENGTH * 3);
	uint8_t message[MAX_PROTOCOL_LENGTH];

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
	server.sin_port = htons(8080);

	//Connect to remote server
	if (connect(s, (struct sockaddr*) &server, sizeof(server)) < 0)
	{
		printf("Could not connect: %d\n", WSAGetLastError());
		return 1;
	}

	while (1)
	{
		//Receive a reply from the server
		if ((recv_size = recv(s, server_reply, MAX_PROTOCOL_LENGTH, 0)) == SOCKET_ERROR)
		{
			printf("Could not receive data\n");
		}

		for (int i = 0; i < recv_size; ++i)
		{
			queue_enqueue(queue, server_reply[i]);
		}

		// message starts with 0x3f!
		while (queue_watch(queue, &message[0]) && message[0] != 0x3f)
		{
			queue_dequeue(queue, &message[0]);
		}
		
		if (queue->length >= MAX_PROTOCOL_LENGTH)
		{
			for (int i = 0; i < MAX_PROTOCOL_LENGTH; i++)
			{
				queue_dequeue(queue, &message[i]);
			}

			printf("IP: %d.%d.%d.%d\tTIME: %02d:%02d:%02d",
				message[2], message[3], message[4], message[5],
				message[6], message[7], message[8]);
			if (message[0] != 0x3f || message[1] != 0xe4 || message[9] != 0xf3)
			{
				printf("\tInvalid data");
			}
			printf("\n");
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}