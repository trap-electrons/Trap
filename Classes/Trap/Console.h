#ifndef CONSOLE_X
#define CONSOLE_X

#include <iostream>
#include <string>
#include "time.h"
using namespace std;

/// Console allows to print some information. Provides handy functions.
struct Console {
public:
	void print(string s);
	void printLog(string s);
	void message(string s);
	void intPrint(int i, string s);
	void printInt(string s, int i);
	void printReal(string s, float r);
	void printTime(clock_t time);
	void printProcessed(string s, int progress);
	void error(string s);
	void warning(string s);

	void keypress();
	int readInt();
};

#endif