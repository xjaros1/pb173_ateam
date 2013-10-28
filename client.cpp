#include "client.h"
#include <Windows.h>
#include <string>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

SOCKET sock;
SOCKADDR_IN i_sock;
WSADATA Data;

int Client::connectTo(std::string IP, int port)
{
	WSAStartup(MAKEWORD(2,2), &Data);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET)
	{
		return 1;
	}
	i_sock.sin_family = AF_INET;
	i_sock.sin_addr.s_addr = inet_addr(IP.c_str());
	i_sock.sin_port = htons(port);
	int ss = connect(sock, (struct sockaddr *)&i_sock, sizeof(i_sock));
	if(ss != 0)
	{
		std::cout << "Cannot connect" << std::endl;
		return 0;
	}
	std::cout << "Successfully connected" << std::endl;
	return 1;
}
int Client::sendData(char *buff, int len)
{
	int slen;
	slen = send(sock, buff, len, 0);
	if(slen < 0)
	{
		std::cout << "Cannot send data" << std::endl;
		return 1;
	}
	return slen;
}
int Client::receiveData(char* buff, int len)
{
	int slen;
	slen = recv(sock, buff, len, 0);
	if( slen < 0 )
	{
		std::cout<< "Cannot recive data" << std::endl;
		return 1;
	}
	return slen;
}
int Client::endSocket()
{
	closesocket(sock);
	WSACleanup();
	return 1;
}