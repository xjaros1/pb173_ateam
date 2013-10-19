#include "client.h"

unsigned char TestCrypto::keySym[32] = "0123456789ABCDEF";
unsigned char TestCrypto::iv[32] = {0};
unsigned char TestCrypto::data[20] = "TEST CRYPTO";


int Client::cryptoSym(unsigned char key[32] , unsigned char iv[32] , unsigned char* data , unsigned char* outData  , int mode){
	TestCrypto::all_run_tests();
}
