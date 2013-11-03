// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")

struct adress{
	std::string IP;
	std::string port;
};
struct cert{

};
struct personInfo{

};
enum eNetworkCommand
{
	CONNECTION_ACCEPTED,
	SERVER_FULL,
	CONNECTION_BANNED,
	RECEIVE_DATA
};
enum requestType{
	HELLO, SHOW, CHOOSE, LOGOUT
};
std::thread thr, thr2, thr3;
struct myPacket {
	int mylong;
	requestType req;
	eNetworkCommand netCmd;
	char mystring[256];
	myPacket() {}
	myPacket(int x, char myString[256], requestType request) : mylong(x)
	{ strcpy_s(mystring, myString);
	req = request;}   
	myPacket(int x, char myString[256], eNetworkCommand enet) : mylong(x)
	{ strcpy_s(mystring, myString);
	netCmd = enet;}   
};
class Client{
private:
	unsigned char publicKey[128];
	unsigned char privateKey[128];
	unsigned char symKey[32];
	unsigned char iv[32];	// inicializacny vektor
	personInfo PI;
	cert myCert;
	cert partnerCert;
	adress myAdress;
	adress partnerAdress;
	bool connected, connectClient;
public:
	Client() { connectClient = false;}
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
	int cryptoSym(unsigned char key[32] , unsigned char iv[32] , unsigned char* data , unsigned char* outData  , int mode);

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
	int cryptoAsym(unsigned char key[128] , unsigned char* data , unsigned char* outData , int mode);

	/**
	* Sends data to other client.
	*
	* @param partnerAdress other client's adress
	* @param data data to be send
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int sendData(char *buff, int len, int sending);

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
	* Requests login to server.
	*
	* @param requestType type of request
	* @param login user's login
	* @param password user's password
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int loginRequest(requestType rT , unsigned char* login , unsigned char* password);

	/**
	* Requests logout from server.
	*
	* @param requestType type of request
	* @param login user's login
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int logoutRequest(requestType rT , unsigned char* login);

	/**
	* Requests registration on server.
	*
	* @param requestType type of request
	* @param myCert user's certificate
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int registrationRequest(requestType rT , cert myCert);

	/**
	* Requests partner's adress from server, based on partner's login.
	*
	* @param requestType type of request
	* @param partnerLogin partner's login
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int adressRequest(requestType rt , unsigned char* partnerLogin);

	/**
	* Request signed certificate from CA.
	*
	* @param requestType type of request
	* @param PI user's personal info
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int certificateRequest(requestType rt , personInfo PI);

	int connectTo(std::string IP, int port);
	int receiveData(char* buff, int len);
	int endSocket();
	int logout();
	void setPartnerAdress(std::string);
	int connectingServer();
	adress getAdress() {return this->partnerAdress;}
	bool getConnected() {return this->connected;}
	void setConnected(bool value) { this->connected = value;}
	int startOnline(myPacket);
	int closeListening();
	int connectingClient();
	bool acceptC();
};
void Client::setPartnerAdress(std::string adress)
{
	std::istringstream iss(adress);
	iss >> this->partnerAdress.IP;
	iss >> this->partnerAdress.port;
	std::cout << this->partnerAdress.IP << " "<< this->partnerAdress.port << std::endl;
}

SOCKET sock;
SOCKADDR_IN i_sock;
WSADATA Data;
SOCKET sock2;
SOCKADDR_IN i_sock2;
SOCKET sock3;
SOCKADDR_IN i_sock3;
WSADATA Data2;
bool Client::acceptC()
{
	int iSockLen = sizeof(i_sock3);
	sock3 = accept(sock2, (sockaddr *)&i_sock3, &iSockLen);
	if( sock3  == INVALID_SOCKET)
		return false;
	else
	{
		std::cout << "connecting "<< std::endl;
		connectClient = true;
	}
	return true;
}
int Client::connectingClient()
{
	myPacket packet;
	while(!connectClient)
	{};
	while(1)
		if(recv(sock3, (char*)&packet, sizeof(packet), 0)!= -1)
		{
			std::cout << packet.mystring << std::endl;
		}
		else
			break;
	return 0;
}
int Client::startOnline(myPacket p)
{
	std::istringstream iss(p.mystring);
	int port;
	u_long ip; 
	iss >> ip >> port;
	int err;
	WSAStartup(MAKEWORD(2,2), &Data2);
	sock2 = socket(AF_INET, SOCK_STREAM, 0);
	if( sock2 == INVALID_SOCKET)
	{
		Sleep(4000);
		exit(0);
		return 0;
	}
	i_sock2.sin_family = AF_INET;
	i_sock2.sin_addr.s_addr = htonl(INADDR_ANY);
	i_sock2.sin_port = htons(port);
	err = bind(sock2, (LPSOCKADDR)&i_sock2, sizeof(i_sock2));
	if( err != 0)
		return 0;

	err = listen(sock2, 2);
	if( err== SOCKET_ERROR)
		return 0;
	thr2 = std::thread(&Client::acceptC, this);
	thr3 = std::thread(&Client::connectingClient, this);
}

int Client::connectingServer()
{
	myPacket packet;
	bool stop = false;
	while(!stop)
	{
		if(recv(sock, (char*)&packet, sizeof(packet), 0)!= -1)
		{
			switch(packet.netCmd)
			{
			case CONNECTION_ACCEPTED:
				std::cout<<"success conected" << std::endl;
				startOnline(packet);
				break;
			case SERVER_FULL:
				std::cout<<"server is full."<<std::endl;
				break;
			case CONNECTION_BANNED:
				std::cout<<"connection banned."<<std::endl;
				stop = true;
				break;
			case RECEIVE_DATA:
				switch(packet.req)
				{
				case SHOW:	
					std::cout << packet.mystring << std::endl;
					break;
				case CHOOSE:
					this->setPartnerAdress(packet.mystring);
					break;
				default: std::cout << packet.mystring << std::endl;
				}
				break;
			}
		}
		else stop = true;
	}
	return 0;
}
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
	connected = true;
	myPacket packet = myPacket(256, "", requestType::HELLO);
	sendData((char*) &packet, 256, 0);
	
	thr = std::thread(&Client::connectingServer, this);
	return 1;
}

int Client::sendData(char *buff, int len, int sending)
{
	int slen;
	if(sending == 0)
		slen = send(sock, buff, len, 0);
	else
		slen = send(sock3, buff, len, 0);
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
int Client::logout()
{
	if(getConnected())
	{
		myPacket packet = myPacket(256, "", requestType::LOGOUT);
		sendData((char*)& packet, 256, 0);
		thr.join();
		endSocket();
		closeListening();
		setConnected(false);
	}
	return 0;
}
int Client::closeListening()
{
	closesocket(sock2);
	thr2.join();
	return 0;
}
int getInput()
{
	Client cl = Client();

	std::string m;
	myPacket packet;
	adress partnerAdress;
	bool stop = false;
	std::istringstream s;
	std::getline(std::cin, m);
	while(!stop)
	{
		switch(m[0])
		{
		case 'h':
				cl.connectTo("127.0.0.1", 5000);
				break;

		case 's':
				packet = myPacket(256, "", requestType::SHOW);
				cl.sendData((char*) &packet, 256, 0);
				break;

		case 'c':
				memset(packet.mystring, 0, 256);
				packet = myPacket(256,(char*) m.substr(2,1).c_str(),  requestType::CHOOSE);
				cl.sendData((char*)& packet, 256, 0);
				cl.setPartnerAdress(packet.mystring);
				break;
		case 'l':
				cl.logout();
				break;
		case 'q':
				cl.logout();
				stop = true;
				break;
		case 'u':
				cl.logout();
				partnerAdress = cl.getAdress();
				s = std::istringstream( partnerAdress.port);
				int port;
				s >> port;
				std::cout << port << " " << partnerAdress.IP << std::endl;
				cl.connectTo("127.0.0.1", port);
				break;
		case 'd':
				std::string str ("HELLO CLIENT, HOW ARE YOU?");
				packet = myPacket(256, (char*)str.c_str(), eNetworkCommand::RECEIVE_DATA);
				cl.sendData((char*)& packet, 256, 0);
				break;
		}
		if(!stop)
			std::getline(std::cin, m);
	}
	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "Klavesy pro akce: "<< std::endl;
	std::cout << "h pro pripojeni k serveru" << std::endl;
	std::cout << "s pro zobrazeni seznamu pripojenych uzivatelu k serveru" << std::endl;
	std::cout << "c cislo  vyber uzivatele ze seznamu (napr. c 5)" << std::endl;
	std::cout << "l pro odpojeni ze serveru/uzivatele" << std::endl;
	std::cout << "q pro ukonceni aplikace" << std::endl;
	std::cout << "u pro pripojeni k uzivateli" << std::endl;
	std::cout << "d pro odeslani dat k uzivateli" << std::endl;
	getInput();
	return 0;
}

