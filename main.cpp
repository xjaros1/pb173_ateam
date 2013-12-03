#include <iostream>
#include "server.h"
#include "client.h"

Server* myServer;
Client* myClient;

unsigned int pre_generatingKeyEnc(void* s)
{
	Client* k = (Client*) s;
	k->counterEnc = 0;
	k->counterDec = 0;
	k->getPointerEnc = 0;
	k->putPointerEnc = 0;
	k->getPointerDec = 0;
	k->putPointerDec = 0;
	unsigned char input[16];
	aes_context ctx;

	do{
		for(int i = 15 ; i >= 0 ; i--){
			int x = k->counterEnc;
			input[i] = x % 256;
			x = x / 256;
		}
		aes_setkey_enc(&ctx , k->key , 128);
		unsigned char output[16];
		aes_crypt_ecb(&ctx , AES_ENCRYPT , input , output);

		for(int i = 0 ; i < 16 ; i++){
			k->encBuffer[k->putPointerEnc + i] = output[i];
			k->decBuffer[k->putPointerEnc + i] = output[i];
		}

		k->counterEnc++;
		k->putPointerEnc += 16;
		k->putPointerDec += 16;
	}while((k->putPointerEnc + 16) < 1000);

	while(!k->stop){
		Sleep(100);

		int lim = k->getPointerEnc - 16;
		if(lim < 0 && k->putPointerEnc >= 16)lim = 1000 + lim;

		if ((k->putPointerEnc % 1000) < lim){
			if(k->putPointerEnc == -1)k->putPointerEnc++;
			for(int i = 15 ; i >= 0 ; i--){
				int x = k->counterEnc;
				input[i] = x % 256;
				x = x / 256;
			}
			aes_setkey_enc(&ctx , k->key , 128);
			unsigned char output[16];
			aes_crypt_ecb(&ctx , AES_ENCRYPT , input , output);
			for(int i = 0 ; i < 16 ; i++){
				k->encBuffer[(k->putPointerEnc + i) % 1000] = output[i];
			}
			k->counterEnc++;
			k->putPointerEnc = (k->putPointerEnc + 16) % 1000;
		}
		
		lim = k->getPointerDec - 16;
		if(lim < 0 && k->putPointerDec >= 16)lim = 1000 + lim;

		if((k->putPointerDec % 1000) < lim ){
			for(int i = 15;i>=0;i--){
				int x = k->counterDec;
				input[i] = x%256;
				x = x/256;
			}
			aes_setkey_enc(&ctx, k->key, 128);
			unsigned char output[16];
			aes_crypt_ecb(&ctx,AES_ENCRYPT,input,output);
			for(int i = 0;i<16;i++){
				k->decBuffer[(k->putPointerDec + i) % 1000] = output[i];
			}
			k->counterDec++;
			k->putPointerDec = (k->putPointerDec + 16) % 1000;
		}
	}
	return 0;
}

UINT output(LPVOID s){
	Socket* server = (Socket*) s;

	std::string r;
	while(1){
		r = server->ReceiveLine();
		bool understand = false;
		
		Sleep(100);
		vector<string> message = split(r,":");
		message[message.size()-1] = message[message.size()-1].substr(0,message[message.size()-1].size()-1);

		if(message[0].compare("COMM") == 0){
			cout << "User "<<message[1]<<" wants to establish a communication. Accept or decline him.\n";
			myClient->incomingConnection = true;
			myClient->activePartnerSocket = new SocketClient("127.0.0.1" , atoi(message[2].c_str()));
			myClient->partnerName = message[1];
			understand = true;
			break;
		}
		
		if(message[0].compare("YES") == 0){
			std::cout<<"Connection accepted.\n";
			myClient->activePartnerSocket = new SocketClient("127.0.0.1" , atoi(message[1].c_str()));
			for(int i = 0 ; i < 16 ; i++){myClient->key[i] = message[2][i];}
			CWinThread* crypt = new CWinThread();
			myClient->stop = false;
			crypt = AfxBeginThread(pre_generatingKeyEnc , (LPVOID) myClient);

			understand = true;
		}
		if(message[0].compare("NO") == 0){
			std::cout<<"Connection declined.\n";
			myClient->partnerName.clear();
			understand = true;
			break;
		}
		if(message[0].compare("END") == 0){
			std::cout<<"Connection terminated on partner's side.\n";
			myClient->stop = true;
			delete myClient->activePartnerSocket;
			myClient->activePartnerSocket = NULL;
			understand = true;
			break;
		}
		if(message[0].compare("MESS") == 0){
			cout<<myClient->partnerName<<" says: "<<message[1]<<std::endl;
			understand = true;
		}
		if (!understand)
		{
			r = r.substr(0,r.size()-1);
			r = myClient->decipher(r);
			message = split(r,":");
			if(message[0].compare("MESS") == 0){
				cout<<myClient->partnerName<<" says: "<<message[1]<<std::endl;
			}
		}
		
	}
	return 0;
}

UINT clientWaiting(LPVOID a) 
{
	SocketServer* pSocketClient = (SocketServer*) a;
	std::cout<<"Able to accept message...\n";
	try{
		while(1){
			Sleep(100);
			Socket* s = NULL;
			s = pSocketClient->Accept();
			CWinThread* outThread = new CWinThread();
			outThread = AfxBeginThread(output , (LPVOID) s);
		}
	}catch(...){
		std::cout<<"vynimka";
	}
	return 0;
}

void sendMessage(Socket* pSocket, int conf)
{
	if(conf == 0) pSocket->SendLine("OK:");
	else pSocket->SendLine("NOK");
}

void parseMessage(Socket* pSocket, std::string receiveLine){

	vector<string> message = split(receiveLine,":");
	if(message[0]=="REG")
	{
		int conf;
		string psw = Server::generatePassword();
		conf = myServer->registration(message[1],psw,NULL);
		if (conf==0) pSocket->SendLine("OK:"+ psw);
		else pSocket->SendLine("NOK");
	}
	if(message[0]=="LOGIN")
	{
		int conf;
		conf = myServer->login(message[1],message[2],atoi(message[3].c_str()));
		sendMessage(pSocket, conf);
	}
	if(message[0]=="LIST")
	{
		string conf = myServer->sendlist(message[1]);
		pSocket->SendLine(conf);
	}
	if(message[0]=="COMM")
	{
		int conf = myServer->startClientCommunication(message[1],message[2]);
		sendMessage(pSocket, conf);
	}
	if(message[0]=="LOGOUT"){
		int conf = myServer->logout(message[1]);
		sendMessage(pSocket, conf);
	}
}

UINT answer(LPVOID s){
	Socket* pSocket = (Socket*) s;
	while(1){
		Sleep(100);
		if(pSocket != NULL){

			std::string r = pSocket->ReceiveLine();
			r = r.substr(0,r.size()-1);
			if(r != ""){
				parseMessage(pSocket, r);
			}
		}
	}
	delete pSocket;
	return 0;
}

void starting(int port)//metoda startuje nove vlakno serveru  to nove vlakno nasledne startuje x novych v pripade ziadostti o komunikaciu
{
	std::cout<<"Server created.\n";
	SocketServer* server = new SocketServer(port , 1);
	std::cout<<"Server started.\n";
	CWinThread* m_WaitingServer = new CWinThread();
	m_WaitingServer = AfxBeginThread(waiting , (LPVOID) server);
}

UINT waiting(LPVOID a){//toto caka na spojenie 
	SocketServer* pSocketServer = (SocketServer*) a;
	std::cout<<"Waiting for client...\n";
	while(1){
		try{
			CWinThread* m_pProxyThread = new CWinThread();
			Socket* s = pSocketServer->Accept();
			std::cout<<"New connection detected.\n";
			m_pProxyThread = AfxBeginThread(answer , (LPVOID) s);
		}
		catch(...){
			std::cout<<"vynimka";
		}
	}
	return 0;
}


/************************************************
>>>>>>>>>>>>>>>>>>>>>>>MAIN<<<<<<<<<<<<<<<<<<<<<<
*************************************************/
int main(int argc , char** argv){
	if(argc != 2){
		std::cerr<<"[ERROR] Unknown argument. Use argument \"server\" or \"client\"\n";
		return 1;
	}
	unsigned int port = 0;
	std::string tmp = argv[1];
	

	if(tmp.compare("server") == 0){
		std::cout<<"Running as a server."<<std::endl;		
		myServer = new Server();
		starting(SERVER_DEFAULT_PORT);
		std::cout<<"Server started.\n";
		while(1){}
		delete myServer;
	}

	if(tmp.compare("client") == 0){
		bool quit = false;
		std::cout<<"Running as client\n";
		std::cout<<"Specify your login: ";
		std::string login;
		cin>>login;
		std::cout<<"For a list of commands type \"/help\"\n";
		myClient = new Client(login);

		while(!quit){
			std::string cmd;

			std::getline(std::cin , cmd);
			if(cmd.size() == 0){std::getline(std::cin , cmd);}
			if(cmd.size() == 0)cmd = '\n';

			if(cmd[0] == '/'){
				quit = myClient->command(cmd);
			}else{
				myClient->sendMessage(cmd);
			}
		}
		delete myClient;
	}
	exit(0);
}






