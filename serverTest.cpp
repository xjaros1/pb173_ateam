#include "server.h"
#include "minunit.h"
#include <string>

using namespace std;

 int test_registration() {
	 
	 Server server();

	 const string login1 = "Lenka";
	 const string heslo1 = "Heslo1";

	 const cert certifikat1 = cert();
	 
	 User user1(login1,heslo1,certifikat1);

	 server.registration(login1,heslo1,certifikat1);
	 User* found = server.getUser(login1);
	 mu_assert("User is not found in the database",found==NULL);
	 mu_assert("Login is not the same",found.login==user1.login);
	 mu_assert("Password is not the same",found.password==user1.password);
	 mu_assert("Certificate is not the same",found.certificate==user1.certificate);

	 mu_assert("registration without password",server.registration("bez hesla","",cert1)!=0);
	 mu_assert("registration without certificate",server.registration("bez certifikatu","heslo",cert1)!=0);
	 mu_assert("double registration of same login",server.registration("Lenka","h",cert1)!=0);





     return 0;
 }