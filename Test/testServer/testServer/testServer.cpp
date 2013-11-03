// testServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <list>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")
enum requestType{
	HELLO, SHOW, CHOOSE, LOGOUT
};
enum eNetworkCommand
{
	CONNECTION_ACCEPTED,
	SERVER_FULL,
	CONNECTION_BANNED,
	RECEIVE_DATA
};
struct myPacket {
	int mylong;
	requestType req;
	eNetworkCommand netCmd;
	char mystring[256];
	myPacket() {}
	myPacket(int x, char myString[256], eNetworkCommand net) : mylong(x)
	{ strcpy_s(mystring, myString);
	netCmd = net;}   
};

SOCKET sock;
SOCKET sock2[200];
SOCKADDR_IN i_sock2[4];
SOCKADDR_IN i_sock;
WSADATA Data;
int clients = 0;
struct User
	{
		//std::string login;
		//std::string password;
		//cert certificate;
		//User(std::string login,std::string password):login(login),password(password){}
		ULONG IP;
		u_short port;
		int number;
		User(ULONG IP, u_short port, int number) : IP(IP), port(port), number(number) {}
	};

	struct cert
	{

	};

	std::thread threads[5];
class Server{
	
private:
	std::list<User*> registeredUsers;
	std::list<User*> onlineUsers;
	cert certificate;
	unsigned char publicKey[128];
	unsigned char privateKey[128];
public:
	/**
	* Checks authenticity of given certificate.
	*
	* @param userCert certficate to be checked
	* @param CApublicKey public key of certification authority
	*
	* @return true when check succesful, false otherwise 
	*/
	bool checkCert(cert userCert , unsigned char* CApublicKey);

	/**
	* Register user into database.
	*
	* @param registeredUsers list with registered users
	* @param login user's login
	* @param pwd user's password
	* @param userCert user's certificate
	*
	* @return zero when succesful, nonzero value when error occurs
	*/
	int registration(std::string login , std::string pwd , cert userCert);

	/**
	* Generates random AES key.
	*
	* @param key generated key
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int randGenAES(unsigned char* key);

	/**
	* Generates random pair of RSA keys.
	*
	* @param publicKey generated public key
	* @param privateKey generated private key
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int randGenRSA(unsigned char* publicKey , unsigned char* privateKey);
		/**
	* Encrypts/decrypts given data with AES - 128.
	* 
	* @param key 128 bit long key
	* @param iv initialisation vector
	* @param data input data
	* @param outData output data
	* @param mode 0 - encryption ; 1 - decryption 
	* 
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int cryptoSym(unsigned char* key , unsigned char* iv , unsigned char* data , unsigned char* outData  , int mode);

	/**
	* Encrypts/decrypts given data with RSA - 1024.
	*
	* @param key 1024 bit long key
	* @param data input data
	* @param outData output data
	* @param mode 0 - encryption ; 1 - decryption
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int cryptoAsym(unsigned char* key , unsigned char* data , unsigned char* outData , int mode);
	
	/**
	* Creates a hash of given data.
	*
	* @param data data to be hashed
	* @param output output of hash
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int hash(unsigned char* data , unsigned char* output);

	/**
	* Sends data to client.
	*
	* @param clientAdress client's adress
	* @param data data to be send
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int sendData(char *Buf, int len, int Client);

	/**
	* Adds user into list of online users.
	*
	* @param onlineUsers list of online users
	* @param login user's login
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int login(std::string login,std::string password);

	/**
	* Removes user from list of online users.
	*
	* @param onlineUsers list of online users
	* @param login user's login
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int logout(std::string login);

	/**
	* Accepts user's request, takes action based on request type.
	*
	* @param requestType type of request
	* @return returns zero when succesful, nonzero value otherwise
	*/
	
	
	int requestAccept(requestType rT);
	User* getUser(std::string login);
	User* getOnlineUser(std::string login);
	int startServer(int port);
	int receiveData(char *Buf, int len, int Client);
	int endSocket();
	int AcceptClients();
	void CloseConnection(int iClientID);
	int service(int client);
	bool Accept(int);
private:
	void save(myPacket *packet);
	User find(myPacket packet);
	void deleteUser(int client);
	int findFreeInArray();
	void acceptSend();
};
int Server::sendData(char *Buf, int len, int Client)
{
	int slen = send(sock2[Client], Buf, len, 0);
	if(slen < 0)
	{
		std::cout<< "Cannot send data"<<std::endl;
		return -1;
	}
	return slen;
}
void Server::CloseConnection(int iClientID)
{
	closesocket(sock2[iClientID]);
	sock2[iClientID] = NULL; 
}
bool Server::Accept(int client)
{
	int iSockLen = sizeof(i_sock2);
	sock2[client] = accept(sock, (sockaddr *)&i_sock2[client], &iSockLen);
	if(sock2[client] == INVALID_SOCKET)
		return false;
	else
	{
		std::cout<< "Connecting from "<< i_sock2[client].sin_addr.s_addr << " "  << i_sock2[client].sin_port << std::endl;
		clients++;
	}
	return true;
}
int Server::findFreeInArray()
{
	for(int i = 0; i < 4; i++)
		if(sock2[i] == NULL)
			return i;
	return 4;
}
char znak = 'a';
int Server::AcceptClients()
{
	myPacket packet;
	while(znak != 'q')
	{
		clients = findFreeInArray();
		if(clients < 4)
		{
			acceptSend();
		}
		else
		{
			if(onlineUsers.size() < 4)
				{
					acceptSend();
				}
				else
				{
					packet = myPacket(256,"", SERVER_FULL);
					sendData((char *)&packet, sizeof(packet), clients-1);
					CloseConnection(clients-1);
				}
		}
	}
	for(int i = 0; i < 4; i ++)
		CloseConnection(i);
	return 0;
}
void Server::acceptSend()
{
	if(Accept(clients) == true)
		{
			std::string s = std::to_string(i_sock2[clients-1].sin_addr.s_addr) + 
				" " + std::to_string(i_sock2[clients-1].sin_port);
			myPacket packet = myPacket(256, (char*)s.c_str(), CONNECTION_ACCEPTED);
			if( sendData(  (char*) &packet, sizeof (packet), clients-1) != -1)
			{
				std::cout << "Client has successfully joined the server" << std::endl;
				threads[clients] = std::thread(&Server::service, this, clients-1);
				//threads[clients].join();
			}
		}
}
User Server::find(myPacket packet)
{
	std::list<User*>::iterator it;
	int nr = atoi(packet.mystring);
	for(it = onlineUsers.begin(); it!= onlineUsers.end(); it++)
		if( (*it)->number == nr)
			return *(*it);
}
void Server::deleteUser(int client)
{
	std::list<User*>::iterator it;
	for(it = onlineUsers.begin(); it!= onlineUsers.end(); it++)
		if( (*it)->number == client)
		{
			onlineUsers.erase(it);
			return;
		}
}
int Server::service(int client)
{
	bool stop = false;
	while(!stop)
	{
		myPacket packet;
		if( receiveData( (char*) &packet, 256, client) != -1)
		{
			switch(packet.req)
			{
			case requestType::HELLO: 
				{User *newUser = new User(i_sock2[client].sin_addr.s_addr, i_sock2[client].sin_port, client);
				onlineUsers.push_back(newUser); 
				break;}
			case requestType::SHOW: 
				{
					myPacket pack;
					save(&pack);
					pack.req = SHOW;
				sendData((char*) &pack, 256, client);
				break;}
			case requestType::CHOOSE: 
				{User us = find(packet);
				std::string s = std::to_string(us.IP) + " " + std::to_string(us.port);
				strcpy_s(packet.mystring, s.c_str());
				packet.netCmd = RECEIVE_DATA;
				packet.req = CHOOSE;
				sendData((char*)&packet, 256, client);
				break;}
			case requestType::LOGOUT:

				packet = myPacket(256, "", CONNECTION_BANNED);
				sendData((char*) &packet, 256, client);
				CloseConnection(client);
				deleteUser(client);
				stop = true;
				std::cout<<"logout"<<std::endl; break;
			}

		}
		else
		{
			std::cout << "User logout" << std::endl;
			stop = true;
		}
	}
	return 0;
}
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
	threads[0] = std::thread(&Server::AcceptClients, this);
}


int Server::receiveData(char *Buf, int len, int Client)
{
	int slen = recv(sock2[Client], Buf, len, 0);
	if(slen < 0)
	{
		std::cout<< "Cannot recive data"<<std::endl;
		return -1;
	}
	return slen;
}

int Server::endSocket()
{
	closesocket(sock);
	WSACleanup();
	return 1;
}
void Server::save(myPacket* packet)
{
	std::list<User*>::iterator it;
	std::string infos("");
	for(it = onlineUsers.begin(); it!= onlineUsers.end(); it++)
	{
		std::string s = std::to_string( (*it)->number);
		infos += s; 
		infos.push_back('\n');
		
	}
	memset(packet->mystring, 0, 256);
	strcpy_s(packet->mystring, infos.c_str() );
	packet->netCmd = RECEIVE_DATA; 
}
int _tmain(int argc, _TCHAR* argv[])
{
	Server serv = Server();

	serv.startServer(5000);

	while(znak != 'q')
	{ std::cin >> znak; }
	serv.endSocket();
	threads[0].join();
	return 0;
}

