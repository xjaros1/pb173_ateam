#include "client.h"
#include <string>
#include <iostream>
#include <sstream>

Client::Client(string login){// klient sa pokusa spojit so serverom hned ako sa vytvori
	CWinThread* crypt = new CWinThread();
	crypt = AfxBeginThread(pre_generatingKeyEnc , (LPVOID) this);
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



string Client::encipher(string text)
{
	string cipherText;
	cipherText.resize(text.size());
	int i = 0;
	bool done = false;
	while(!done)
	{
		while (this->getPointerEnc != this->putPointerEnc)
		{
			if (i == text.size()) {done = true;break;}
			cipherText[i] = text[i] ^ this->encBuffer[getPointerEnc];
			i++;
			getPointerEnc++;
		}
	}
	return cipherText;
}
string Client::decipher(string text)
{
	string plainText;
	plainText.resize(text.size());
	int i = 0;
	bool done = false;
	while(!done)
	{
		while (this->getPointerDec != this->putPointerDec)
		{
			if (i == text.size()) {done = true;break;}
			plainText[i] = text[i] ^ this->decBuffer[getPointerDec];
			i++;
			getPointerDec++;
		}
	}
	return plainText;
}

int Client::connectToPartner(std::string value){
	if(!activePartnerSocket){
		partnerName = value;
		communicationRequest(value);
		return 0;
	}else{
		std::cout<<"  You already have ongoing communication.\n";
		return -1;
	}
}

int Client::acceptComm(){
	if(incomingConnection){
		std::stringstream buff;
		buff<<"YES:"<<port;
		Sleep(100);
		activePartnerSocket->SendLine(buff.str());
		std::cout<<"You are now connected to "<<partnerName<<std::endl;
		return 0;
	}else{
		std::cout<<"  You have currently no incoming connections.\n";
		return -1;
	}
}

int Client::declineComm(){
	if(incomingConnection){
		std::string buff;
		buff = "NO";
		activePartnerSocket->SendLine(buff);
		delete activePartnerSocket;
		activePartnerSocket = NULL;
		return 0;
	}else{
		std::cout<<"  You have currently no incoming connections.\n";
		return -1;
	}
}

int Client::endComm(){
	if(activePartnerSocket){
		std::string buff;
		buff = "END";
		activePartnerSocket->SendLine(buff);
		delete activePartnerSocket;
		activePartnerSocket = NULL;
		return 0;
	}else{
		std::cout<<"  You have currently no ongoing communication.\n";
		return -1;
	}
}

int Client::disconnect(){
	if(activeServerSocket){
		logoutRequest();
		return 0;
	}else{
		std::cout<<"  You are not connected at this moment.\n";
		return -1;
	}
}

int Client::quit(){
	if(activePartnerSocket){
		std::string buff;
		buff = "END";
		activePartnerSocket->SendLine(buff);
		delete activePartnerSocket;
	}
	if(activeServerSocket){
		logoutRequest();
	}
	return 0;
}

int Client::sendMessage(std::string value){
	if(activePartnerSocket){
		std::string mess = "MESS:" + value;
		//mess = encipher(mess);//funkcnost netusim
		activePartnerSocket->SendLine(mess);
		return 0;
	}else{
		std::cout<<"  Message not sent(no recipient connected)\n";
		return -1;
	}
}

bool Client::command(std::string cmd){
	int action;
	std::string value;
	action = commandParse(value , cmd);
	switch(action){
		case 0://help
			help();
			break;
		case 1://register
			registrationRequest();
			break;
		case 2://login 
			loginRequest(value);
			break;
		case 3://online
			listRequest();
			break;
		case 4://connect
			connectToPartner(value);
			break;
		case 5://accept
			acceptComm();
			break;
		case 6://decline
			declineComm();
			break;
		case 7://end
			endComm();
			break;
		case 8://disconnect
			disconnect();
			break;
		case 9://quit
			quit();
			std::cout<<"Have a nice day.\n";
			return true;
		}
	return false;
}

int Client::commandParse(std::string& value , std::string cmd){
	std::vector<string> command = split(cmd , ":");
	if(command.size() > 1)
		value = command[1];
	

	if(command[0].compare("/help") == 0){return 0;}
	if(command[0].compare("/register") == 0){return 1;}
	if(command[0].compare("/login") == 0){return 2;}
	if(command[0].compare("/online") == 0){return 3;}
	if(command[0].compare("/connect") == 0){return 4;}
	if(command[0].compare("/accept") == 0){return 5;}
	if(command[0].compare("/decline") == 0){return 6;}
	if(command[0].compare("/end") == 0){return 7;}
	if(command[0].compare("/disconnect") == 0){return 8;}
	if(command[0].compare("/quit") == 0){return 9;}
	
	std::cout<<"Unknown command.\n";
	return -1;
}

void Client::help(){
	std::cout<<"Type:\n";
	std::cout<<"  \"/register\" to register on server\n";//1
	std::cout<<"  \"/login:[pwd]\" to log in on server, pwd is your password\n";//2
	std::cout<<"  \"/online\" to see who is online\n";//3
	std::cout<<"  \"/connect:[user]\" to connect to specific user\n";//4
	std::cout<<"  \"/accept\" to accept incoming connection\n";//5
	std::cout<<"  \"/decline\" to decline incoming connection\n";//6
	std::cout<<"  \"/end\" to end actual communication\n";//7
	std::cout<<"  \"/disconnect\" to disconnect from server\n";//8
	std::cout<<"  \"/quit\" to end program\n";//9
}