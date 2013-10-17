#ifndef CERTIFICATIONAUTHORITY_H
#define CERTIFICATIONAUTHORITY_H

class CertificationAuthority{
private:
	unsigned char privateKey[128];
	unsigned char publicKey128;
	certList validCerts;
	cert certificate;
public:
    /**
	* Creates certificate of user, signs it with CA's private key.
	*
	* @param userInfo user's personal information
	* @param userPublicKey user's public key
	* 
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int sign(persInfo userInfo , unsigned char userPublicKey[128]);

	/**
	* Sends certificate to specified adress.
	*
	* @param userCert user's certificate encrypted with CA's public key
	* @param userAdress user's adress
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int sendCert(cert userCert , adress userAdress);

	/**
	* Adds user's certificate to list of valid certificates.
	*
	* @param userCert user's certificate
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int addToList(cert userCert);

	/**
	* Removes user's certificate from list of valid certificates
	*
	* @param userCert user's certificate
	*
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int removeFromList(cert userCert);

	/**
	* Accepts user's request, takes action based on request type.
	*
	* @param requestType type of request
	* @return returns zero when succesful, nonzero value otherwise
	*/
	int requestAccept(requestType rT);
};


#endif //CERTIFICATION_AUTHORITY
