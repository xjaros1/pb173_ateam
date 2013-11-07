//v prvom rade treba vyriesit preco mi komunikacia medzi klientami blbne... vyzera ze sa tam posiela nejaky riadok navyse. skus si to spustiti ako jeden program ako server a dvoch klientov obobch zaregistruj,prihlas a jednym sa pokus skontaktovat toho druheho... v jednom klientovi ti vypadne nieco ako ak chcete potvrdit komunikaciu stlacte a. tu teba stalit a dvakrat.. to je ten problem co som riesilôa so svendom s tym cin.get() mozes to popripade toto potvrdenie cele zrusit.

//dalej treba dokoncit tenr logout. ja ho nemam nie preto ze by samotny logout bol problem ale kedze som nevyriesila ako ukoncit komunikaciu medzi klientami tak som sa venovala tomu.. asi skus normalne do mainu pridat moznost ze ked klient nieco stlaci tak sa posle logout request.

//treba vymysliet ako sa bude dat skoncit komunikacia medzi klientami najrozumnejsie bude asi zaviest nejaky fixnz retazec ktory sa neposle ale vzdy sa nan bude testovat a ked sa objavi komunikacia sa ukonci. co znamena ze sa prerusiten nekonecny while cyklus snedline-ov a receiveline-ov

//dalej v maine je to riesene tak ze klient ma atribut komunicacion ktory ked je zapnuty tak sa prerusi ten hlavny cyklus co vypisuje "ak sa chces prihlasit stlac p, ak registrovat stlac r, ak komunikovat stlacte k\n" problem je ze on sa uplne zrusi a aj ked by sa komunikacia ukoncila tak uz sa neda vpoddstate nic robit.. takze to treba este vyriesit.


//volitelne sa da este prerobit ta vec s globalnymi premmnymi ak si pocuival a pametas si moj rozhovor so svendom.. ze by sa vlaknobvej funkcii nepredaval len ten socket ale trebars struktura ktorA  by obsahovala este aj ukazatel na naseho klienta alebo na ans server.. podla toho co je potreba.. toto nie je az tak nutne kvoli funkcnosti.. len by sme zrusili globalne premenne a teda si trosku skrajsili kod.

#include <iostream>
#include "server.h"
#include "client.h"

Server* myServer;//globalne premenne ktore som pouzila kvoli tomu ze vlaknova funkcia ma byt staticka
Client* myClient;

UINT clientWaiting(LPVOID a) // funkcia v ktorej klient caka na spravu. ta prva cast, teda prvy while cyklus je pouzity v pripade ze tento klient dostal od serveru ziadost o komunikaciu. a on ako prvy poslal spravu (v metode klienta send data) teda v ramci klientskej komunikacie.. sa hra na klienta (nie server) kedze komunikacia zacala uz inde poouzivam nie socket ziskany z metody accept ale activePartnerSocket ktory bol vytvoreny v tej metode send data. mozno by sa dal cely ten while cyklus presunut tam. 
{
	SocketServer* pSocketClient = (SocketServer*) a;
	std::cout<<"Able to accept message...\n";
	while(1){
		try{
			Sleep(100);
			Socket* s = NULL;
			if(myClient->activePartnerSocket==NULL) { s = pSocketClient->Accept();std::cout<<"New connection detected.\n";}			
			
			if (myClient->activePartnerSocket!=NULL)
			{
				while(1)
				{
					std::string r = myClient->activePartnerSocket->ReceiveLine();
					Sleep(100);
					cout << r;
					r = r.substr(0,r.size()-1);
					vector<string> message = split(r,":");
					myClient->communication = true;
						cout << "napiste od odpoved \n";
						string response;
						cin >> response;
						myClient->activePartnerSocket->SendLine(response);
					
				}
			}
			else
			{
				while(1) //toto je cast ked sa klient tvari ako servetr nacuva a proste mu pridu data
				{
					std::string r = s->ReceiveLine();
					Sleep(100);
					cout << r;
					r = r.substr(0,r.size()-1);
					vector<string> message = split(r,":");
					myClient->communication = true;
					if (message[0]=="COMM")
					{
						cout << "uzivatel "<<message[1]<< " s vami chce komunikovat, ak chcete zacat komunikaciu stlacte a\n";
						char c;
						cin >>c;
						if (c == 'a') myClient->sendData(atoi(message[2].c_str()));
						else cout <<"chyba je v nacitavani\n";
						cout <<"ide breaK\n";
						break;
					}
					else
					{
						cout << "napiste od odpoved (ak bude xxx komunikacia sa ukonci)\n";
						string response;
						cin >> response;
						s->SendLine(response);
					}
				}
			}
			
		}
		catch(...){
			std::cout<<"vynimka";
		}
	}
	return 0;
}
UINT answer(LPVOID s){ //cakacia metoda serveru... oddelujem informacia v spravach pres server dvojbodkou... to sda parsuju a vola sa prislusna metoda na parsovanie som si odniekial skopirovala metode co fungujre podobne ako split v jave... jejdefinicia je ulozena v struct suboroch. je tu vyriesena registracia, login, ziadost o zaslanie zoznamu online userov a ziadsot o komunikaciu s jednym z nich. chyba len logout.. to by nemolo byt nic zlozite
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
			}
		}
	}
	delete pSocket;
	return 0;
}
void starting(int port)//metoda startuje nove vlakno serveru  to nove vlakno nasledne statuje x novych v pripade ziadostti o komunikaciu
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

int main(int argc , char** argv){
	if(argc != 2){
		std::cerr<<"[ERROR] Bad argument."<<std::endl;
		return 1;
	}
	unsigned int port;
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
		std::cout<<"Running as client\n";
		std::cout<<"Specify your login: ";
		string login;
		std::cin>>login;
		myClient = new Client(login);
		while(!myClient->communication)
		{
			cout << "ak sa chces prihlasit stlac p, ak registrovat stlac r, ak komunikovat stlacte k\n";
			char x;
			cin >> x;
			if (x=='r')
			{
				int n = myClient->registrationRequest();
			}
			if (x=='p')
			{
				cout << "zadajte heslo:";
				string heslo;
				cin >> heslo;
				myClient->loginRequest(heslo);
			}
			if (x=='k')
			{
				cout<< "nasleduje zoznam online uzivatelov\n";
				myClient->listRequest();
				cout << "vyberte si jedneho z nich a jeho login napisete\n";
				string partnerLogin;
				cin >> partnerLogin;
				myClient->communicationRequest(partnerLogin);
				myClient->communication = true;
			}
		}
		while(1){}
		delete myClient;
	}

	return 0;
}