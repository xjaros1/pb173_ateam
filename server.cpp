#include <string>
#include <iostream>
#include <afxwin.h>
#include <sstream>
#include <cstdlib>

#include "server.h"



Server::Server(){

}

string Server::generatePassword()//toto raz bude geneerovat anhodne hesla.. zatial netreba riesit//uz by to malo generovat hesla a treba to riesit
{
	unsigned char pwd[10];
	std::string password;
	ctr_drbg_context ctr_drbg;											
	entropy_context entropy;
	char *pers = "Rebus in adversis animum submittere noli: spem retine, spes una hominen nec morte relin quit.";

	entropy_init( &entropy );										
	ctr_drbg_init( &ctr_drbg, entropy_func, &entropy , (unsigned char *) pers, strlen( pers ) );
	ctr_drbg_random( &ctr_drbg, pwd, 10 );

	srand(pwd[1]);
	int r = 0;
	for(int i = 0 ; i < 10 ; i++){
		r = rand() % 3;
		switch(r){
		case 0:
			password.push_back((pwd[i] % 10) + 48);
			break;
		case 1:
			password.push_back((pwd[i] % 26) + 65);
			break;
		case 2:
			password.push_back((pwd[i] % 26) + 97);
			break;
		}
	}
	return password;
}

int Server::registration(std::string login , std::string pwd , cert* userCert)
{
	cout <<"Client "<<login<<" is trying to register\n";
	if (getUser(login)!=NULL) {cout<< "[ERROR] User is already registered.\n"; return 1;}
	if(pwd.empty()) { cout<<"[ERROR] No password specified." << std::endl; return 1;}
	if(userCert) {cout <<"[ERROR] No certificate given."<<endl; return 1;}//toto je kvoli testovaniu, potom treba prehodit podmienku... tak nakoniec neimplementovane
	if(login.empty()) {cout <<"[ERROR] Empty login." << endl; return 1;}

	//prevod hesla do SHA-512 hashu, plaintext sa neuklada
	std::string pwdHashString;
	unsigned char pwdHash[64];
	unsigned char input[10];
	for(int i = 0 ; i < pwd.size() ; i++){input[i] = pwd[i];}
	sha4(input , 10 , pwdHash , 0);
	for(int i = 0 ; i < 64 ; i++){pwdHashString.push_back(pwdHash[i]);}

	registeredUsers.push_back(new User(login,pwdHashString,userCert));
	cout << "Registration successful\n";
	return 0;
}

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

int Server::login(std::string login,std::string password, int port)
{
	cout << "User " <<login <<" is trying to log in.\n";
	User* u = getOnlineUser(login);
	if (u) {cout << "[ERROR] User is already online.\n";return -1;}
	u = getUser(login);
	if (!u) {cout << "[ERROR] User is not registered yet.\n"; return -1;}

	//kontrola spravnosti hashu hesla
	unsigned char pwd[10];
	unsigned char pwdHash[64];
	std::string pwdHashString;
	for(int i = 0 ; (i < password.size() && i < 10); i++){pwd[i] = password[i];}
	sha4(pwd , 10 , pwdHash , 0);
	for(int i = 0 ; i < 64 ; i++){pwdHashString.push_back(pwdHash[i]);}
	if (u->passwordHash.compare(pwdHashString) != 0){cout << "[ERROR] Wrong password.\n"; return -1;}

	u->port = port;
	cout << "Setting port to: " << port <<endl;
	onlineUsers.push_back(u);
	cout << "Login successful.\n";
	return 0;
}
string Server::sendlist(std::string login)
{
	User* u = getOnlineUser(login);
	if (!u) {cout << "[ERROR] User not online, cannot send online list.\n"; return "NOK:Sorry, you have to login first.";}
	string result = "";
	if (onlineUsers.size()<=1){cout << "[ERROR] Online list is empty.\n"; return "NOK:There are no users online.";}
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
	cout << "Client " << fromC << " is trying to establish communication with us " << toC <<endl;
	User* f = getOnlineUser(fromC);
	if(!f) {cout << "[ERROR] Client is not logged in.\n";return -1;}
	User* t = getOnlineUser(toC);
	if(!t) {cout << "[ERROR] Requested user is not online.\n";return -1;}
	SocketClient* partnerClientSocket;
	try{
		cout << "Trying to establish communication with "<<toC <<" on port "<<t->port<<endl;
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
		std::cout<<"[ERROR] Connection failed.\n";
		partnerClientSocket = NULL;
	}

	return 0;
}

int Server::logout(std::string login){
	cout << "User "<<login <<" wants to logout.\n";
	User* u = getUser(login);
	if (!u) {cout << "[ERROR] User is not registered.\n"; return -1;}
	u = getOnlineUser(login);
	if (!u) {cout << "[ERROR] User is already disconnected.\n";return -1;}


	for(std::list<User*>::iterator i = onlineUsers.begin(); i != onlineUsers.end() ; i++){
		if((*i) == u){
			i = onlineUsers.erase(i);

			if(i == onlineUsers.end()){
				break;
			}
		}
	}

	cout << "Logout successful\n";
	return 0;
}
