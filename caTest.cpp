#include "certification_authority.h"
#include "server.h"
#include "minunit.h"
extern int tests_run;
static char* test_sign() 
{
	CertificationAuthority* ca = new CertificationAuthority(); 
	cert certifikat = ca->sign("personal info");
	Server* s = new Server();
	mu_assert("sign is not verified",s->checkCert(certifikat, ca->getPubKey()));

}
static char* test_addToList()
{
	CertificationAuthority* ca = new CertificationAuthority();
	cert certifikat = cert();
	ca->addToList(certifikat);
	mu_assert("certificate can't be verified",ca->verifyCert(certifikat));
	

}
static char* test_removeFromList()
{
	CertificationAuthority* ca = new CertificationAuthority();
	cert certifikat = cert();
	ca->addToList(certifikat);
	ca->removeFromList(certifikat);
	mu_assert("removed certificate can be verified",!ca->verifyCert(certifikat));

}
static char* all_run_tests(){

		mu_run_test(test_sign);
		mu_run_test(test_addToList);
		mu_run_test(test_removeFromList);
		return 0;
	}