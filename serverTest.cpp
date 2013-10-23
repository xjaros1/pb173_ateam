#include "server.h"
#include "minunit.h"
#include <string>

extern int tests_run;
unsigned char TestCrypto::keySym[32] = "0123456789abcdef";
unsigned char TestCrypto::iv[32] = {0};
unsigned char TestCrypto::data[20] = "test crypto";

using namespace std;

 static char* test_registration() {
	 
	 Server* server  = new Server();
	 const string login1 = "Lenka";
	 const string heslo1 = "Heslo1";
	 const cert certifikat1 = cert();	 
	 User* user1 = new User(login1,heslo1,certifikat1);

	 //TEST correct registration
	 mu_assert("problem in correct registration",server->registration(login1,heslo1,certifikat1));
	 User* found = server->getUser(login1);
	 mu_assert("User is not found in the database",found==NULL);
	 mu_assert("Login is not the same",found->login==user1->login);
	 mu_assert("Password is not the same",found->password==user1->password);
	 mu_assert("Certificate is not the same",found->certificate==user1->certificate);

	 //TEST registration without password
	 mu_assert("registration without password",server->registration("bez hesla","",certifikat1)!=0);
	 //TEST registration without without certificate
	 //mu_assert("registration without certificate",server->registration("bez certifikatu","heslo",NULL)!=0);
	 //TEST registration without without login
	 mu_assert("registration without login",server->registration("","heslo",certifikat1)!=0);
	 //TEST
	 mu_assert("double registration of same login",server->registration("Lenka","heslo",certifikat1)!=0);

     
 }
  static char* test_login()
  {
	  Server* server = new Server();
	  const cert certifikat1 = cert();
	  //TEST prihlasit zaregistrovaneho uzivatela
	  server->registration("Lenka","heslo",certifikat1);
	  server->registration("Zuzka","heslo",certifikat1);
	  mu_assert("login return non zero value",server->login("Lenka","heslo"));
	  User* found = server->getOnlineUser("Lenka");
	  mu_assert("User is not found in the database",found!=NULL);
	  mu_assert("Login is not the same",found->login=="Lenka");
	  mu_assert("Password is not the same",found->password=="heslo");
	  mu_assert("Certificate is not the same",found->certificate==certifikat1);

	  //TEST pokus o login neregistrovaneho	  
	  mu_assert("login of unregistered user passed",server->login("neregistrovany","heslo")!=0);
	  found = server->getOnlineUser("neregistrovany");
	  mu_assert("unregistered user is found in database of online users",found==NULL);

	  //TEST prihlasenie s nespravnym heslom
	  mu_assert("login with wrong password",server->login("Zuzka","nespravne"));
	  found = server->getOnlineUser("Zuzka");
	  mu_assert("after wrong password the user is found in list of online",found==NULL);

	  //TEST prihlasenie bez hesla
	  mu_assert("login without password",server->login("Zuzka",""));
	  found = server->getOnlineUser("Zuzka");
	  mu_assert("after login without password the user is found in list of online",found==NULL);

	  //TEST login without username
	  mu_assert("login without username",server->login("","heslo"));

  }

  static char* test_logout()
  {
	  Server* server = new Server();
	  const cert certifikat1 = cert();
	  server->registration("Lenka","heslo",certifikat1);
	  //TEST korektne odhlasenie
	  server->login("Lenka","heslo");
	  mu_assert("correct logout didn't passed",server->logout("Lenka")==0);
	  User* found = server->getOnlineUser("Lenka");
	  mu_assert("user wasn't logout",found==NULL);
	  found = NULL;
	  found = server->getUser("Lenka");
	  mu_assert("after logout user was deleted from registrated",found!=NULL);

	  //TEST odhlasenie offline uzivatela
	  mu_assert("logout of offline user passed without error",server->logout("Lenka")!=0);
	  
  }


class TestCryptoClient{
public:
	static unsigned char keySym[32];
	static unsigned char keyPrivate[128];
	static unsigned char keyPublic[128];
	static unsigned char data[20];
	static unsigned char output[20];
	static unsigned char test[20];
	static unsigned char iv[32];
	static Server s1;

	static char* test_cryptoSym(){

		sl.cryptoSym(keySym, iv, data, output, 1);

		sl.cryptoSym(keySym, iv, output, test, 0);

		mu_assert("Error crypto", strcmp((const char*) test, (const char*)data) == 0);
		return 0;
	}
	static char* test_cryptoAsym(){

		sl.cryptoAsym(keyPublic, data, output, 1);

		sl.cryptoAsym(keyPrivate, output, test, 0);

		mu_assert("Error crypto asym", strcmp( (const char*) test, (const char*) data) == 0);
		return 0;
	}
	static char* test_generAES()
	{
		keySym[0] = 0;
		sl.randGenAES(keySym);
		mu_assert("Wrong gener sym key", strlen( (const char*) keySym) != 0);
		return 0;
	}
	static char* test_generRSA()
	{
		keyPrivate[0] = 0;
		keyPublic[0] = 0;
		sl.randGenRSA(keyPublic, keyPrivate);
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