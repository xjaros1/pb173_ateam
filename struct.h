#ifndef STRUCT_H
#define STRUCT_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#define SERVER_DEFAULT_PORT 111
using namespace std;


struct cert{

};

struct personInfo{

};

enum requestType{
	LOGIN, LOGOUT, REG, ADRESS, CA, LIST, COMM
};

vector<string> split(const string& s, const string& delim, const bool keep_empty = true);
unsigned int waiting(void* a);
unsigned int answer(void* s);
void starting(int port);
unsigned int clientWaiting(void* a);
unsigned int pre_generatingKeyEnc(void* s);


//vector<string> mySplit(string line,char separator) {
  //  vector<string> result;
	//return result;
//}

#endif 