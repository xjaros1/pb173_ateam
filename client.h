#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <afxwin.h>

#include "polarssl/aes.h"
#include "polarssl/entropy.h"
#include "polarssl/ctr_drbg.h"

#include "socket.h"
#include "struct.h"
//#include "minunit.h"



struct adress{
	std::string IP;

};




class Client{
public:
	SocketClient* activePartnerSocket;
	SocketClient* activeServerSocket;
	std::string partnerName;
	int port;
	bool incomingConnection;
	unsigned char key[16];
	unsigned char encBuffer[1000];
	unsigned char decBuffer[1000];
	int getPointerEnc;
	int putPointerEnc;
	int counterEnc;
	int getPointerDec;
	int putPointerDec;
	int counterDec;
	bool stop;
private:
	unsigned char publicKey[128];
	unsigned char privateKey[128];
	personInfo PI;
	std::string login;
	cert myCert;
	cert partnerCert;
	adress myAdress;
	adress partnerAdress;
public:
	Client(string login);
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
	int cryptoSym(std::string, unsigned char iv[16], std::string data, std::string& outData, int mode);

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
	int loginRequest(std::string password);

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
	int registrationRequest();

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

	/*
	* Metody pre obsluhu prikazov uzivatela(Lubo)
	*/
private:
	int commandParse(std::string& , std::string);
	int listRequest();
	int communicationRequest(std::string);
	int logoutRequest();
	int connectToPartner(std::string value);
	int acceptComm();
	int declineComm();
	int endComm();
	int disconnect();
	int quit();
	void help();
public:
	bool command(std::string);
	int sendMessage(std::string);

	/**
	* XOR given plaintext to pre-generated encryption buffer
	* 
	* @param text plaintext to be enciphered
	*
	* @return ciphertext
	*/
	std::string encipher(std::string text);

	/**
	* XOR given ciphertext to pre-generated decryption buffer
	*
	* @param text ciphertext to be decrypted
	*
	* @return plaintext
	*/
	std::string decipher(std::string text);
};


#endif //CLIENT
