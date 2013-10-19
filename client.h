#ifndef CLIENT_H
#define CLIENT_H

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
#include <iostream>

extern int tests_run;

class Client{
private:
	unsigned char publicKey[128];
	unsigned char privateKey[128];
	unsigned char symKey[32];
	unsigned char iv[32];
	persInfo PI;
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
	int sendData(adress partnerAdress , unsigned char* data);

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
};

class TestCrypto{
public:
	static unsigned char keySym[32];
	static unsigned char keyPrivate[128];
	static unsigned char keyPublic[128];
	static unsigned char data[20];
	static unsigned char output[20];
	static unsigned char test[20];
	static unsigned char iv[32];
	static Client cl;

	static char* test_cryptoSym(){

		cl.cryptoSym(keySym, iv, data, output, 1);

		cl.cryptoSym(keySym, iv, output, test, 0);

		mu_assert("Error crypto", strcmp((const char*) test, (const char*)data) == 0);
		return 0;
	}
	static char* test_cryptoAsym(){

		cl.cryptoAsym(keyPublic, data, output, 1);

		cl.cryptoAsym(keyPrivate, output, test, 0);

		mu_assert("Error crypto asym", strcmp( (const char*) test, (const char*) data) == 0);
		return 0;
	}
	static char* test_generAES()
	{
		keySym[0] = 0;
		cl.randGenAES(keySym);
		mu_assert("Wrong gener sym key", strlen( (const char*) keySym) != 0);
		return 0;
	}
	static char* test_generRSA()
	{
		keyPrivate[0] = 0;
		keyPublic[0] = 0;
		cl.randGenRSA(keyPublic, keyPrivate);
		mu_assert("Wrong gener asym key", (strlen((const char*)keySym) != 0 && strlen((const char*)keyPublic)!= 0 ));
		return 0;
	}
	static char* all_run_tests(){

		mu_run_test(test_cryptoAsym);
		mu_run_test(test_cryptoSym);
		mu_run_test(test_generAES);
		mu_run_test(test_generRSA);
		return 0;
	}
};
class TestLogin{
public:
	static Client cl;
	static unsigned char* login;
	static char* test_loginRequest(){
		requestType rT;
		unsigned char* password;
		mu_assert("Error login request", cl.loginRequest(rT, login, password));
		return 0;
	}
	static char* test_logoutRequest(){
		requestType rT;
		mu_assert("Error logout request", cl.logoutRequest(rT, login));
		return 0;
	}
	static char* test_registrationRequest(){
		requestType rT;
		cert myCert;
		mu_assert("Error registration request", cl.registrationRequest(rT, myCert) );
		return 0;
	}
	static char* test_adressRequest(){
		requestType rT;
		unsigned char* partnerLogin;
		mu_assert("Error adress request", cl.adressRequest(rT, partnerLogin));
		return 0;
	}
	static char* test_certRequest(){
		requestType rT;
		PersonInfo PI;
		mu_assert("Error cert request", cl.certificateRequest(rT, PI));
		return 0;
	}
	static char* test_sendData(){
		unsigned char* partnerLogin;
		unsigned char* data;
		mu_assert("Error send data", cl.sendData(partnerLogin, data));
		return 0;
	}
	static char* all_run_test(){
		mu_run_test(test_loginRequest);
		mu_run_test(test_logoutRequest);
		mu_run_test(test_registrationRequest);
		mu_run_test(test_adressRequest);
		mu_run_test(test_certRequest);
		mu_run_test(test_sendData);
		return 0;
	}

};



#endif //CLIENT
