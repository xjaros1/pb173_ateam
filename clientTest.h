#include <iostream>
#include <sstream>
#include "client.h"
#include "minunit.h"

/*extern int tests_run;
unsigned char TestCrypto::keySym[32] = "0123456789abcdef";
unsigned char TestCrypto::iv[32] = {0};
unsigned char TestCrypto::data[20] = "test crypto";


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
};*/

class TestLogin{
public:
	static char* test_registrationRequest(){
		Client* testClient = new Client("Lubo");
		int conf = testClient->registrationRequest();
		mu_assert("Error login request", conf == 0);
	}

	static char* test_loginRequest(){
		bool success = false;
		Client* testClient = new Client("Lubo");
		testClient->registrationRequest();
		testClient->loginRequest("heslo");

		std::stringstream buff;
		buff<<"LIST:Lubo";
		testClient->activeServerSocket->SendLine(buff.str());
		Sleep(100);
		std::string r = testClient->activeServerSocket->ReceiveLine();
		r = r.substr(0,r.size()-1);
		vector<string> message = split(r,":");

		for(int i =0;i<message.size();i++)
		{
			if(message[i].compare("Lubo") == 0){success=true;}
		}
		mu_assert("Error login request", success);
	}

	static char* test_logoutRequest(){
		bool success = true;
		Client* myClient = new Client("Lubo");
		myClient->registrationRequest();
		myClient->loginRequest("heslo");
		myClient->logoutRequest();

		Client* testClient = new Client("Dakto");
		testClient->registrationRequest();
		testClient->loginRequest("heslo");

		std::stringstream buff;
		buff<<"LIST:Dakto";
		testClient->activeServerSocket->SendLine(buff.str());
		Sleep(100);
		std::string r = testClient->activeServerSocket->ReceiveLine();
		r = r.substr(0,r.size()-1);
		vector<string> message = split(r,":");

		for(int i =0;i<message.size();i++)
		{
			if(message[i].compare("Lubo") == 0){success=false;}
		}
		mu_assert("Error logout request", success);

	}

	/*static char* test_adressRequest(){

		unsigned char* partnerLogin = (unsigned char*) "Lenka";
		mu_assert("Error adress request", cl.adressRequest(requestType::ADRESS, partnerLogin) == 0);
		return 0;
	}
	static char* test_certRequest(){
		personInfo PI = personInfo();
		mu_assert("Error cert request", cl.certificateRequest(requestType::CA, PI) == 0);
		return 0;
	}*/

	static char* all_tests(){
		mu_run_test(test_loginRequest);
		mu_run_test(test_logoutRequest);
		mu_run_test(test_registrationRequest);
		//mu_run_test(test_adressRequest);
		//mu_run_test(test_certRequest);
		return 0;
	}

};