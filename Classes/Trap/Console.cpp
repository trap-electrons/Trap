#include "Console.h"

Console console;

void Console :: print(string s)
{
	cout << s;
}

void Console :: printLog(string s)
{
	cout << "------------------------------------------------------------------------------\n";
	cout << s << "\n";
	cout << "------------------------------------------------------------------------------\n";
}

void Console :: message(string s)
{
	cout << " :: " << s << "\n";
}

void Console :: printReal(string s, float r)
{
	cout << " :: " << s << r << "\n";
}

void Console :: printInt(string s, int i)
{
	cout << " :: " << s << i << "\n";
}

void Console :: intPrint(int i, string s)
{
	cout << " :: " << i << " "<< s << "\n";
}

int Console :: readInt()
{
	cout <<  " :: -> ";
	int i; 
	try {
		cin >> i; 
	} catch (...) 
	{
		i = 0;
	}
	cout <<  "\n";
	return i;
}

void Console :: printTime(clock_t time)
{
	cout << " :: Time Elapsed: " << time << "\n";
}

void Console :: printProcessed(string s, int progress)
{
	cout << " :: " << s << ": " << progress << "%		";
	if (progress!=100)
		cout << "\r";
	else 
		cout << "\n";
}

void Console :: error(string s)
{
	cout << " :: -> Error: " << s << "\n";
}

void Console :: warning(string s)
{
	cout << " :: Warning: " << s << "\n";
}

void Console :: keypress() 
{
	cout << "\n\n :: Press any key and Enter\n :: -> ";
	char a;
	try {
		cin >> a;
	}
	catch (...) { }
}
