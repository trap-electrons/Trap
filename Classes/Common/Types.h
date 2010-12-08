#ifndef TRAP_TYPES
#define TRAP_TYPES

#define MACOS

// Disable STL check to provide high performance
#if (!(_DEBUG || DEBUG))
#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0
#endif

#include <fstream>
#include <memory>
#include <vector>
#include "stdio.h"
using namespace std;

#define sint int
#define uint unsigned int
#define lint long int
#define real float
#define uword unsigned long
#define ubyte unsigned char

#define sqr(a) a*a

struct vector3 { real v[3]; };
struct vector4 { real v[4]; };
struct float4 { float x,y,z,w; };
struct vertex3 { float x,y,z; };

/// electrons parameters and pi
static const real 
ez = 4.8e-10,
c = 3.0e10, 
me = 9.1e-28, 
pi = 3.1415926535897932384626433832795;


/// shared pointer (smart pointer)
#ifdef MACOS
#include <tr1/memory>
using namespace tr1;
#define shptr shared_ptr
#define wptr weak_ptr
#endif
#ifndef MACOS
#define shptr std::tr1::shared_ptr
#define wptr std::tr1::weak_ptr
#endif

/// Checks file existing
bool isFileExist(const char *filename);
/// Returns a file size
int fileSize(const char* sFileName);

/// Convert wchar to char
void wcharToChar(const wchar_t *w, char *c);

/// Read data from file
void readFile(string &filename, char *buf, size_t size);

/// The macros allows set the value
#define SETTER(name, type, var) inline void name (type value) { var=value; } 
/// The macros allows get the value
#define GETTER(name, type, var)	inline type name () { return var; }
/// The macros combinates GETTER and SETTER
#define GETTER_SETTER(shortname, type, var) SETTER(set##shortname, type, var) GETTER(get##shortname, type, var)

#endif
