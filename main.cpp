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

int commandParse(std::string& value , std::string cmd){
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
	if(command[0].compare("/sendJunk") == 0){return 10;}
	
	std::cout<<"Unknown command.\n";
	return -1;
}

UINT output(LPVOID s){
	Socket* server = (Socket*) s;

	std::string r;
	while(1){
		r = server->ReceiveLine();
		Sleep(100);
		r = r.substr(0,r.size()-1);
		vector<string> message = split(r,":");

		if(message[0].compare("COMM") == 0){
			cout << "User "<<message[1]<<" wants to establish a communication. Accept or decline him.\n";
			myClient->incomingConnection = true;
			myClient->activePartnerSocket = new SocketClient("127.0.0.1" , atoi(message[2].c_str()));
			myClient->partnerName = message[1];
			break;
		}
		if(message[0].compare("YES") == 0){
			std::cout<<"Connection accepted.\n";
			myClient->activePartnerSocket = new SocketClient("127.0.0.1" , atoi(message[1].c_str()));
		}
		if(message[0].compare("NO") == 0){
			std::cout<<"Connection declined.\n";
			myClient->partnerName.clear();
			break;
		}
		if(message[0].compare("END") == 0){
			std::cout<<"Connection terminated on partner's side.\n";
			delete myClient->activePartnerSocket;
			myClient->activePartnerSocket = NULL;
			break;
		}
		if(message[0].compare("MESS") == 0){
			cout<<myClient->partnerName<<" says: "<<message[1]<<std::endl;
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

void help(){
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

//cakacia metoda serveru... oddelujem informacia v spravach pres server dvojbodkou... to sda parsuju a vola sa prislusna metoda na 
//parsovanie som si odniekial skopirovala metode co fungujre podobne ako split v jave... jej definicia je ulozena v struct suboroch. 
//je tu vyriesena registracia, login, ziadost o zaslanie zoznamu online userov a ziadsot o komunikaciu s jednym z nich. chyba len logout.. to by nemolo byt nic zlozite
UINT answer(LPVOID s){
	Socket* pSocket = (Socket*) s;
	while(1){
		Sleep(100);
		if(pSocket != NULL){
			std::string r = pSocket->ReceiveLine();
			r = r.substr(0,r.size()-1);
			if(r != ""){
				vector<string> message = split(r,":");
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
					cout<< r <<endl;
					int conf;
					conf = myServer->login(message[1],message[2],atoi(message[3].c_str()));
					if (conf==0) pSocket->SendLine("OK");
					else pSocket->SendLine("NOK");
				}
				if(message[0]=="LIST")
				{
					string conf = myServer->sendlist(message[1]);
					pSocket->SendLine(conf);
				}
				if(message[0]=="COMM")
				{
					int conf = myServer->startClientCommunication(message[1],message[2]);
					if (conf==0) pSocket->SendLine("OK");
					else pSocket->SendLine("NOK");
				}
				if(message[0]=="LOGOUT"){
					int conf = myServer->logout(message[1]);
					if (conf==0) pSocket->SendLine("OK");
					else pSocket->SendLine("NOK");
				}
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
	if(argc != 2){
		std::cerr<<"[ERROR] Bad argument."<<std::endl;
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
		//std::getline(std::cin , login);
		cin>>login;
		std::cout<<"For a list of commands type \"/help\"\n";
		myClient = new Client(login);

		while(!quit){
			std::string cmd;
			std::stringstream buff;
			int action = -1;
			int n = 0;

			std::getline(std::cin , cmd);
			if(cmd.size() == 0){std::getline(std::cin , cmd);}
			buff.clear();

			if(cmd[0] == '/'){
				std::string value;
				action = commandParse(value , cmd);
				switch(action){
				case 0://help
					help();
					break;
				case 1://register
					n = myClient->registrationRequest();
					break;
				case 2://login 
					myClient->loginRequest(value);
					break;
				case 3://online
					myClient->listRequest();
					break;
				case 4://connect
					if(!myClient->activePartnerSocket){
						myClient->partnerName = value;
						myClient->communicationRequest(value);
					}else{
						std::cout<<"  You already have ongoing communication.\n";	
					}
					break;
				case 5://accept
					if(myClient->incomingConnection){
						std::cout<<"You are now connected to "<<myClient->partnerName<<std::endl;
						buff<<"YES:"<<myClient->port;
						Sleep(100);
						myClient->activePartnerSocket->SendLine(buff.str());
					}else{
						std::cout<<"  You have currently no incoming connections.\n";
					}
					break;
				case 6://decline
					if(myClient->incomingConnection){
						buff<<"NO";
						myClient->activePartnerSocket->SendLine(buff.str());
						delete myClient->activePartnerSocket;
						myClient->activePartnerSocket = NULL;
					}else{
						std::cout<<"  You have currently no incoming connections.\n";
					}
					break;
				case 7://end
					if(myClient->activePartnerSocket){
						buff<<"END";
						myClient->activePartnerSocket->SendLine(buff.str());
						delete myClient->activePartnerSocket;
						myClient->activePartnerSocket = NULL;
					}else{
						std::cout<<"  You have currently no ongoing communication.\n";
					}
					break;
				case 8://disconnect
					if(myClient->activeServerSocket){
						myClient->logoutRequest();
					}else{
						std::cout<<"  You are not connected at this moment.\n";	
					}
					break;
				case 9://quit
					std::cout<<"Have a nice day.\n";
					if(myClient->activePartnerSocket){
						buff<<"END";
						myClient->activePartnerSocket->SendLine(buff.str());
						delete myClient->activePartnerSocket;
					}
					if(myClient->activeServerSocket){
						myClient->logoutRequest();
					}
					quit = true;
					break;
				case 10://sendJunk
					int intValue = 0;
					std::stringstream ss;
					ss<<value;
					ss>>intValue;
					for(int i = 0 ; i < intValue ; i++){
						buff<<i;
					}
					myClient->activePartnerSocket->SendLine(buff.str());
					break;
				}
			}else{
				if(myClient->activePartnerSocket){
					std::string mess = "MESS:";
					mess = mess + cmd;
					myClient->activePartnerSocket->SendLine(mess);
				}else{
					std::cout<<"  Message not sent(no recipient connected)\n";
				}
			}
		}
		delete myClient;
	}

	if(tmp.compare("testServer") == 0){
			
	}

	if(tmp.compare("testClient") == 0){
		
	}

	return 0;
}






