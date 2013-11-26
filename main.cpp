//v prvom rade treba vyriesit preco mi komunikacia medzi klientami blbne... vyzera ze sa tam posiela nejaky riadok navyse. skus si to spustiti ako jeden program 
//ako server a dvoch klientov obobch zaregistruj,prihlas a jednym sa pokus skontaktovat toho druheho... v jednom klientovi ti vypadne nieco ako ak chcete potvrdit 
//komunikaciu stlacte a. tu teba stalit a dvakrat.. to je ten problem co som riesilôa so svendom s tym cin.get() mozes to popripade toto potvrdenie cele zrusit.

//dalej treba dokoncit tenr logout. ja ho nemam nie preto ze by samotny logout bol problem ale kedze som nevyriesila ako ukoncit komunikaciu medzi klientami tak som sa venovala 
//tomu.. asi skus normalne do mainu pridat moznost ze ked klient nieco stlaci tak sa posle logout request.

//treba vymysliet ako sa bude dat skoncit komunikacia medzi klientami najrozumnejsie bude asi zaviest nejaky fixnz retazec ktory sa neposle ale vzdy sa nan bude testovat a ked sa 
//objavi komunikacia sa ukonci. co znamena ze sa prerusiten nekonecny while cyklus snedline-ov a receiveline-ov

//dalej v maine je to riesene tak ze klient ma atribut komunicacion ktory ked je zapnuty tak sa prerusi ten hlavny cyklus co vypisuje "ak sa chces prihlasit stlac p, ak registrovat 
//stlac r, ak komunikovat stlacte k\n" problem je ze on sa uplne zrusi a aj ked by sa komunikacia ukoncila tak uz sa neda vpoddstate nic robit.. takze to treba este vyriesit.


//volitelne sa da este prerobit ta vec s globalnymi premmnymi ak si pocuival a pametas si moj rozhovor so svendom.. ze by sa vlaknobvej funkcii nepredaval len ten socket ale 
//trebars struktura ktorA  by obsahovala este aj ukazatel na naseho klienta alebo na ans server.. podla toho co je potreba.. toto nie je az tak nutne kvoli funkcnosti.. 
//len by sme zrusili globalne premenne a teda si trosku skrajsili kod.

#include <iostream>
#include "server.h"
#include "client.h"

Server* myServer;//globalne premenne ktore som pouzila kvoli tomu ze vlaknova funkcia ma byt staticka
Client* myClient;

unsigned int pre_generatingKeyEnc(void* s)
{
	unsigned char key[16] = {212,212,212,212,212,212,212,212,212,212,212,212,212,212,212,212};
	Client* k = (Client*) s;
	k->counterEnc = 0;
	k->counterDec = 0;
	k->getPointerEnc = 0;
	k->putPointerEnc = 0;
	k->getPointerDec = 0;
	k->putPointerDec = 0;
	unsigned char  input[16];
	aes_context ctx;
	while(true)
	{
		if (k->getPointerEnc<984)
	{
		for(int i = 15;i>=0;i--)
		{
			int x = k->counterEnc;
			input[i] = x%256;
			x = x/256;
		}
		aes_setkey_enc(&ctx, key, 128);
		unsigned char output[16];
		aes_crypt_ecb(&ctx,AES_ENCRYPT,input,output);
		for(int i = 0;i<16;i++)
		{
			k->encBuffer[(k->putPointerEnc+i)%1000] = output[i];
		}
		k->counterEnc++;
		k->putPointerEnc+=16;
		k->putPointerEnc = k->putPointerEnc % 1000;
		k->getPointerEnc+=16;
		cout << k->getPointerEnc<<endl;
	}
		if(k->getPointerDec<984)
	{
		for(int i = 15;i>=0;i--)
		{
			int x = k->counterDec;
			input[i] = x%256;
			x = x/256;
		}
		aes_setkey_enc(&ctx, key, 128);
		unsigned char output[16];
		aes_crypt_ecb(&ctx,AES_ENCRYPT,input,output);
		for(int i = 0;i<16;i++)
		{
			k->decBuffer[(k->putPointerDec+i)%1000] = output[i];
		}
		k->counterDec++;
		k->putPointerDec+=16;
		k->putPointerDec = k->putPointerDec % 1000;
		k->getPointerDec+=16;
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
		//r = r.substr(0,r.size()-1);
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
			r = myClient->decipher(r);//moje
			//r = r.substr(0,r.size()-1);//moje
			message = split(r,":");//moje
			if(message[0].compare("MESS") == 0){
				message[1] = message[1].substr(0,message[1].size()-1);
				cout<<myClient->partnerName<<" says: "<<message[1]<<std::endl;
			}
		}
		
	}
	return 0;
}

//funkcia v ktorej klient caka na spravu. ta prva cast, teda prvy while cyklus je pouzity v pripade ze tento klient dostal od serveru ziadost o komunikaciu. 
//a on ako prvy poslal spravu (v metode klienta send data) teda v ramci klientskej komunikacie.. sa hra na klienta (nie server) kedze komunikacia zacala uz inde poouzivam nie socket 
//ziskany z metody accept ale activePartnerSocket ktory bol vytvoreny v tej metode send data. mozno by sa dal cely ten while cyklus presunut tam. 
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
		cout<< receiveLine <<endl;
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


/*************************************************
>>>>>>>>>>>>>>>>>>>>>>>MAIN<<<<<<<<<<<<<<<<<<<<<<
*************************************************/
int main(int argc , char** argv){
	/*unsigned int port = 0;
	std::string tmp = argv[1];
	if(tmp.compare("server") == 0){
		std::cout<<"Running as a server."<<std::endl;		
		myServer = new Server();
		starting(SERVER_DEFAULT_PORT);
		std::cout<<"Server started.\n";
		while(1){}
		delete myServer;
	}
	else{	
	Client* c = new Client("lenka");
	CWinThread* crypt = new CWinThread();
	crypt = AfxBeginThread(pre_generatingKeyEnc , (LPVOID) c);
	Client* c1 = new Client("Janko");
	CWinThread* crypt1 = new CWinThread();
	crypt1 = AfxBeginThread(pre_generatingKeyEnc , (LPVOID) c1);
	string	pokus = c->encipher("ahoj");
	cout << pokus;
	pokus = c1->decipher(pokus);
	cout << pokus;
	cin.get();
	}*/
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

			if(cmd[0] == '/'){
				quit = myClient->command(cmd);
			}else{
				myClient->sendMessage(cmd);
			}
		}
		delete myClient;
	}
	return 0;
}






