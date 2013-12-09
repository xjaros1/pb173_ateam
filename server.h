#ifndef SERVER_H
#define SERVER_H

#include <list>
#include <afxwin.h>
#include <windows.h> //mutex
#include "polarssl/entropy.h"
#include "polarssl/ctr_drbg.h"

#include "socket.h"
#include "struct.h"

#define THREADCOUNT 5

struct User
{
		std::string login;
		std::string passwordHash;
		cert* certificate;
		int port;
		User(std::string login,std::string passwordHash,cert* certificate):login(login),passwordHash(passwordHash),certificate(certificate){}
};

class Server{
	
private:
	_Guarded_by_(ghMutex) std::list<User*> registeredUsers;
	_Guarded_by_(ghMutex) std::list<User*> onlineUsers;
	cert certificate;
	unsigned char publicKey[128];
	unsigned char privateKey[128];
	
public:
	_Has_lock_kind_( _Lock_kind_mutex_ ) HANDLE ghMutex;

	Server();
	//static UINT waiting(LPVOID a);
	//static UINT answer(LPVOID s);
	/**
	* Checks authenticity of given certificate.
	*
	* @param userCert certficate to be checked
	* @param CApublicKey public key of certification authority
	*
	* @return true when check succesful, false otherwise 
	*/
	_Check_return_ bool checkCert(_In_ cert userCert , _In_ unsigned char* CApublicKey);

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
	_Check_return_ _Requires_lock_held_(ghMutex) int registration(_In_ std::string login , _In_ std::string pwd , _In_ cert* userCert);

	/**
	* Generates random AES key.
	*
	* @param key generated key
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	_Check_return_ int randGenAES(_In_ unsigned char* key);

	/**
	* Generates random pair of RSA keys.
	*
	* @param publicKey generated public key
	* @param privateKey generated private key
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	_Check_return_ int randGenRSA(_In_ unsigned char* publicKey ,_In_ unsigned char* privateKey);

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
	_Check_return_ int cryptoSym(_In_ unsigned char* key ,_In_ unsigned char* iv , _In_ unsigned char* data , _Out_ unsigned char* outData  , _In_ int mode);

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
	_Check_return_ int cryptoAsym(unsigned char* key , unsigned char* data , unsigned char* outData , int mode);
	
	/**
	* Creates a hash of given data.
	*
	* @param data data to be hashed
	* @param output output of hash
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	_Check_return_ int hash(_In_ unsigned char* data , _In_ unsigned char* output);

	/**
	* Sends data to client.
	*
	* @param clientAdress client's adress
	* @param data data to be send
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	_Check_return_ int sendData(_In_reads_(len) char *Buf, _In_ int len, _In_ int Client);

	/**
	* Adds user into list of online users.
	*
	* @param onlineUsers list of online users
	* @param login user's login
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	_Check_return_ _Requires_lock_held_(ghMutex) int login(_In_ std::string login, _In_ std::string password, _In_ int port);

	/**
	* Removes user from list of online users.
	*
	* @param onlineUsers list of online users
	* @param login user's login
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	_Check_return_ _Requires_lock_held_(ghMutex) int logout(_In_ std::string login);

	/**
	* Accepts user's request, takes action based on request type.
	*
	* @param requestType type of request
	* @return returns zero when succesful, nonzero value otherwise
	*/
	_Check_return_ int requestAccept(_In_ int rT);
	_Check_return_ _Requires_lock_held_(ghMutex) _Ret_notnull_ User* getUser(_In_ std::string login);
	_Check_return_ _Requires_lock_held_(ghMutex) _Ret_notnull_ User* getOnlineUser(_In_ std::string login);
		
	_Check_return_ int startServer(_In_ int port);
	_Check_return_ int receiveData(_In_reads_(len) char *Buf, _In_ int len, _In_ int Client);
	_Check_return_ int endSocket();
	_Check_return_ _Requires_lock_held_(ghMutex) _Ret_z_ std::string sendlist(_In_ std::string login);
	_Check_return_ static _Ret_z_ string generatePassword();
	_Check_return_ _Requires_lock_held_(ghMutex) int startClientCommunication(_In_ std::string fromC, _In_ std::string toClient);
};

#endif //SERVER_H
