#include "stdio.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "queue.h"

#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define MAX_PROTOCOL_LENGTH 12

union message
{
	uint8_t data[MAX_PROTOCOL_LENGTH];
	struct
	{
		byte x3f;
		byte len;
		byte ip[4];
		byte time[3];
		byte b[2];
	};
} mes;

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char server_reply[MAX_PROTOCOL_LENGTH];
	int recv_size;
	queue_t* queue = create_queue(MAX_PROTOCOL_LENGTH * 3);
	uint8_t value;

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
		while (queue_watch(queue, &value, 0) && value != 0x3f)
		{
			queue_dequeue(queue, &value);
		}
		
		value = 0;
		if (queue_watch(queue, &value, 1) && value <= queue->length)
		{
			for (int i = 0; i < MAX_PROTOCOL_LENGTH; i++)
			{
				queue_dequeue(queue, &mes.data[i]);
			}

			uint8_t crc = 0;
			for (uint8_t i = 0; i < mes.len - 1; ++i)
			{
				crc ^= mes.data[i];
			}
			if (crc != mes.data[mes.len - 1])
			{
				printf("Invalid message\n");
				continue;
			}

			printf("IP: %d.%d.%d.%d\tTIME: %02d:%02d:%02d",
				mes.ip[0], mes.ip[1], mes.ip[2], mes.ip[3],
				mes.time[0], mes.time[1], mes.time[2]);
			size_t payload_size = mes.len - 10;
			for (size_t i = 0; i < payload_size; ++i)
			{
				printf("\tV%d: %d", i + 1, mes.b[i]);
			}
			printf("\n");
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}