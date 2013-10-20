#include "client.h"

unsigned char testcrypto::keysym[32] = "0123456789abcdef";
unsigned char testcrypto::iv[32] = {0};
unsigned char testcrypto::data[20] = "test crypto";


int client::cryptosym(unsigned char key[32] , unsigned char iv[32] , unsigned char* data , unsigned char* outdata  , int mode){
	testcrypto::all_run_tests();
}
