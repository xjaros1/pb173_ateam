#include "client.h"
#include <string>
#include <iostream>
#include <sstream>

Client::Client(string login){// klient sa pokusa spojit so serverom hned ako sa vytvori
	this->login = login;
	stop = false;
	incomingConnection = false;
	activePartnerSocket = NULL;
	try{
		activeServerSocket = new SocketClient("127.0.0.1" , SERVER_DEFAULT_PORT);
	}catch(...)
	{
		std::cout<<"Connection error.\n";
		activeServerSocket = NULL; // premenna sluzi na to aby som mohls v lubovolnej metode ked uz som sa rraz na server nepojila mohla s nim komunikovat
	}
}

int Client::registrationRequest(){// v tych jednotlivych requestochvacsinou nie je nic zlozite
	std::string toSend;
	toSend = "REG";
	toSend += ":";
	toSend += login;
	activeServerSocket->SendLine(toSend);
	Sleep(100);
	std::string r = activeServerSocket->ReceiveLine();
	
	r = r.substr(0,r.size()-1);
	vector<string> message = split(r,":");
	if((message[0]=="OK")){
		std::cout<<"Registration succesfull.\n";
		std::cout<<"Your password is: "<<message[1]<<std::endl;
		return 0;
	}else{
		std::cout<<"Registration failed.\n";
		return -1;
	}
}
int Client::listRequest()
{
	std::string toSend;
	toSend = "LIST";
	toSend += ":";
	toSend += login;
	activeServerSocket->SendLine(toSend);
	Sleep(100);
	std::string r = activeServerSocket->ReceiveLine();
	r = r.substr(0,r.size()-1);
	vector<string> message = split(r,":");
	if (message[0]=="NOK") cout <<message[1]<<std::endl;
	else
	{
		std::cout<<"Following users are online:\n";
		for(int i =0;i<message.size();i++)
		{
			cout << message[i]<<endl;
		}
	}

	return 0;
}
int Client::loginRequest(string password)
{
	std::stringstream toSend;
	toSend << "LOGIN";
	toSend << ":";
	toSend << login;
	toSend << ":";
	toSend << password;
	cout << "Specify the port you will be available for incoming connections: ";
	int p;
	cin>> p;
	port = p;
	toSend << ":";
	toSend << p;
	activeServerSocket->SendLine(toSend.str());
	Sleep(100);
	std::string r = activeServerSocket->ReceiveLine();
	
	r = r.substr(0,r.size()-1);
	vector<string> message = split(r,":");
	if((message[0]=="OK")){
		std::cout<<"Login succesful.\n";
		SocketServer* server = new SocketServer(p , 1);		
		CWinThread* m_WaitingClient = new CWinThread();
		m_WaitingClient = AfxBeginThread(clientWaiting , (LPVOID) server);
		return 0;
	}else{
		std::cout<<"Login failed.\n";
		return -1;
	}
	return 0;
}

int Client::communicationRequest(string partnerLogin)
{
	string toSend = "";
	toSend += "COMM";
	toSend += ":";
	toSend += login;
	toSend += ":";
	toSend += partnerLogin;
	activeServerSocket->SendLine(toSend);
	Sleep(100);
	std::string r = activeServerSocket->ReceiveLine();
	r = r.substr(0,r.size()-1);
	vector<string> message = split(r,":");
	if((message[0]=="OK")) cout << "Invitation send, wait for answer.\n";
	else cout << "An error occured, communication won't be established.\n";
	return 0;
}
/*int Client::sendData(int port)
{
	cout << "Trying to establish connection on port "<<port<<endl;
	activePartnerSocket = new SocketClient("127.0.0.1" , port);
	stringstream toSend;
	toSend<<"YES";
	activePartnerSocket->SendLine(toSend.str());
	return 0;
}*/

int Client::logoutRequest(){
	std::stringstream buff;
	buff<<"LOGOUT:"<<login;
	activeServerSocket->SendLine(buff.str());
	Sleep(100);
	std::string r = activeServerSocket->ReceiveLine();
	r = r.substr(0,r.size()-1);
	vector<string> message = split(r,":");
	if((message[0]=="OK")) cout << "You have been disconnected.\n";
	else cout << "An error occured, you are still connected.\n";
	delete activeServerSocket;
	activeServerSocket = NULL;
	return 0;
}


int Client::cryptoSym(std::string key, unsigned char iv[16], std::string data, std::string& outData, int mode)
{
	if(key.length() < 32)
		key.append(32 - key.length(), '\0');
	if(data.length() % 16 != 0)
		data.append( 16 - (data.length() %16), '\0');
	aes_context ctx;
	outData = std::string("");
	if(mode == AES_ENCRYPT)
	{
		aes_setkey_enc(&ctx, (unsigned char*)key.c_str(), 256);
		for(int i = 0; i < 16; i++)
			iv[i] = (unsigned char) rand();
	}
	else
		aes_setkey_dec(&ctx, (unsigned char*)key.c_str(), 256);

	std::string strIv((char*) iv, 16);
	size_t length = ((data.length() / 16) + 1) * 16;
	unsigned char *out = new unsigned char[length];

	aes_crypt_cbc(&ctx, mode, data.size(), iv, (unsigned char*) data.c_str(), out);

	outData = std::string((char*) out, length);
	strcpy((char*) iv, strIv.c_str());
	return true;
}
