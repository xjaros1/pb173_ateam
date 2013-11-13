#include <string>
#include <iostream>
#include <afxwin.h>
#include <sstream>

#include "server.h"



Server::Server(){

}





string Server::generatePassword()//toto raz bude geneerovat anhodne hesla.. zatial netreba riesit
{
	return "heslo";
}

//odpoved na jednotlive klientske requesty toto konkretne o registraciu. tato a jej podobne funkcie su volane zo statickej metody answer ktoru najdes v maine
int Server::registration(std::string login , std::string pwd , cert* userCert)
{
	cout <<"Klient "<<login<<" sa pokusa zaregistrovat\n";
	if (getUser(login)!=NULL) {cout<< "Neuspesne, taky login uz je zaregistrovany\n"; return 1;}
	if(pwd.empty()) { cout<<"Bez hesla" << std::endl; return 1;}
	if(userCert) {cout <<"bez certifikatu"<<endl; return 1;}//toto je kvoli testovaniu, potom treba prehodit podmienku
	if(login.empty()) {cout <<"Bez loginu" << endl; return 1;}
	registeredUsers.push_back(new User(login,pwd,userCert));
	cout << "Registracia prebehla uspesne\n";
	return 0;
}

// pomocne metody notner aby sa dalo rozumne pracovat podla loginu vratia usera
User* Server::getUser(std::string login)
{
	if (registeredUsers.empty()) return NULL;
	list<User*>::iterator iter;
	for(iter = registeredUsers.begin();iter!=registeredUsers.end();iter++)
	{
		if ((*iter)->login == login) return (*iter);
	}
	return NULL;
}
User* Server::getOnlineUser(std::string login)
{
	if (onlineUsers.empty()) return NULL;
	list<User*>::iterator iter;
	for(iter = onlineUsers.begin();iter!=onlineUsers.end();iter++)
	{
		if ((*iter)->login == login) return (*iter);
	}
	return NULL;
}

 // dalsia obsluzna metoda
int Server::login(std::string login,std::string password, int port)
{
	cout << "uzivatel " <<login <<" sa pokusa prihlasit\n";
	User* u = getOnlineUser(login);
	if (u) {cout << "uzivatel uz je online\n";return -1;}
	u = getUser(login);
	if (!u) {cout << "dany uzivatel nie je registrovany\n"; return -1;}
	if (u->password!=password){cout << "heslo nie je spravne\n"; return -1;}
	u->port = port;
	cout << "nastavujem port na " << port <<endl;
	onlineUsers.push_back(u);
	cout << "prihlasenie uspesne\n";
	return 0;
}
string Server::sendlist(std::string login)
{
	User* u = getOnlineUser(login);
	if (!u) {cout << "dany uzivatel nie je prihlaseny, nemozno mu poslat zoznam online klientov\n"; return "NOK:Sorry, you have to login first.";}
	string result = "";
	if (onlineUsers.size()<=1){cout << "zozname online klientov nebsahuje nikoho vhodneho\n"; return "NOK:There are no users online.";}
	list<User*>::iterator iter;
	for(iter = onlineUsers.begin();iter!=onlineUsers.end();iter++)
	{
		if (iter!=onlineUsers.begin() && (*iter)->login!=login) result +=":";
		if ((*iter)->login!=login) result+=(*iter)->login;
	}
	return result;
}

int Server::startClientCommunication(std::string fromC, std::string toC)
{
	cout << "klient " << fromC << " sa pokusa nadviazat komunikaciu s klinetom " << toC <<endl;
	User* f = getOnlineUser(fromC);
	if(!f) {cout << "CHYBA! klient ktory chce komunikovat nie je online\n";return -1;}
	User* t = getOnlineUser(toC);
	if(!t) {cout << "CHYBA! cielovy klient nie je online\n";return -1;}
	SocketClient* partnerClientSocket;
	try{
		cout << "pokusam sa nadviazat komunikaciu s klientom "<<toC <<" na porte "<<t->port<<endl;
		// pokus o nasdviazanie komunikacie s vyziadanym klientomn posielam mu hlavne port klienta ktory s nim chce komunikovat aby sa mu mal ako ozvat
		partnerClientSocket = new SocketClient("127.0.0.1" , t->port);
		stringstream toSend("");
		toSend << "COMM";
		toSend << ":";
		toSend << fromC;
		toSend << ":";
		toSend << f->port;
		partnerClientSocket->SendLine(toSend.str());
	}catch(...)
	{
		std::cout<<"Pripojenie neuspesne\n";
		partnerClientSocket = NULL;
	}

	return 0;
}

int Server::logout(std::string login){
	cout << "User "<<login <<" wants to logout.\n";
	User* u = getUser(login);
	if (!u) {cout << "User is not registered.\n"; return -1;}
	u = getOnlineUser(login);
	if (!u) {cout << "User is already disconnected.\n";return -1;}


	for(std::list<User*>::iterator i = onlineUsers.begin(); i != onlineUsers.end() ; i++){
		if((*i) == u){
			//delete *i;
			i = onlineUsers.erase(i);

			if(i == onlineUsers.end()){
				break;
			}
		}
	}

	cout << "Logout succesful\n";
	return 0;
}
