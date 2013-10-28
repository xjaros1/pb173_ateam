#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include "minunit.h"



/*typedef int persInfo;
typedef int cert;
typedef int adress;*/

struct adress{
	std::string IP;

};
struct cert{

};
struct personInfo{

};
enum requestType{
	LOGIN, LOGOUT, REG, ADRESS, CA
};

class Client{
private:
	unsigned char publicKey[128];
	unsigned char privateKey[128];
	unsigned char symKey[32];
	unsigned char iv[32];	// inicializacny vektor
	personInfo PI;
	cert myCert;
	cert partnerCert;
	adress myAdress;
	adress partnerAdress;
public:
	Client() {}
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
	int cryptoSym(unsigned char key[32] , unsigned char iv[32] , unsigned char* data , unsigned char* outData  , int mode);

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
	* Sends data to other client.
	*
	* @param partnerAdress other client's adress
	* @param data data to be send
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int sendData(char *buff, int len);

	/**
	* Generates random AES key.
	*
	* @param key generated key
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int randGenAES(unsigned char* key);

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
	int loginRequest(requestType rT , unsigned char* login , unsigned char* password);

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
	int registrationRequest(requestType rT , cert myCert);

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
	
	int connectTo(std::string IP, int port);
	int receiveData(char* buff, int len);
	int endSocket();

};


#endif //CLIENT
