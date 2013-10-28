#include "server.h"
#include <string>
#include <iostream>
#include <Windows.h>
#pragma comment(lib, "Ws2_32.lib")

SOCKET sock;
SOCKET sock2[200];
SOCKADDR_IN i_sock2;
SOCKADDR_IN i_sock;
WSADATA Data;
int clients = 0;

int Server::startServer(int port)
{
	int err;
	WSAStartup(MAKEWORD(2,2), &Data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
		Sleep(4000);
		exit(0);
		return 0;
	}
	i_sock.sin_family = AF_INET;
	i_sock.sin_addr.s_addr = htonl(INADDR_ANY);
	i_sock.sin_port = htons(port);
	err = bind(sock, (LPSOCKADDR)&i_sock, sizeof(i_sock));
	if(err != 0)
		return 0;

	err = listen(sock, 2);
	if( err == SOCKET_ERROR)
		return 0;
	while(1)
	{
		// max count of clients is 1, can be changed
		for(int i = 0; i < 1; i++)
		{
			if( clients < 1)
			{
				int so2len = sizeof(i_sock2);
				sock2[clients] = accept(sock, (sockaddr*) &i_sock2, &so2len);
				if(sock2[clients] == INVALID_SOCKET)
					return 0;
				std::cout << "A client has joined the server IP: "<< i_sock2.sin_addr.s_addr << std::endl;
				clients++;
				continue;
			}
			else
				return 1;
		}
		
	}
 	return 1;
}
int Server::sendData(char *Buf, int len, int Client)
{
	int slen = send(sock2[Client], Buf, len, 0);
	if(slen < 0)
	{
		std::cout<< "Cannot send data"<<std::endl;
		return 1;
	}
	return slen;
}
int Server::receiveData(char *Buf, int len, int Client)
{
	int slen = recv(sock2[Client], Buf, len, 0);
	if(slen < 0)
	{
		std::cout<< "Cannot recive data"<<std::endl;
		return 1;
	}
	return slen;
}
int Server::endSocket()
{
	closesocket(sock);
	WSACleanup();
	return 1;
}