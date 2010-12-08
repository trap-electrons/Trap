#include "Types.h"

//--- File Exists -------------------------------------------------------------
 bool isFileExist(const char *filename) {
	ifstream f(filename, ios::in | ios::binary);
	if (!f) return false; 
	f.close();
	return true;
}


//--- File Size ---------------------------------------------------------------
 int fileSize(const char* sFileName)
{
  std::ifstream f;

  f.open(sFileName, std::ios_base::binary | std::ios_base::in);
  if (!f.good() || f.eof() || !f.is_open()) { return 0; }

  f.seekg(0, std::ios_base::beg);
  std::ifstream::pos_type begin_pos = f.tellg();
  f.seekg(0, std::ios_base::end);

  return static_cast<int>(f.tellg() - begin_pos);
}

//-----------------------------------------------------------------------------
void wcharToChar(const wchar_t *w, char *c) 
{
   // size_t size = wcslen(w) + 1;
   // size_t convertedChars = 0;
	//wcstombs_s(&convertedChars, c, size, w, _TRUNCATE);
	c = NULL;
}


//-----------------------------------------------------------------------------
void readFile(string &filename, char *buf, size_t size)
{
	std::ifstream f;

	f.open(filename.c_str(), std::ios_base::binary | std::ios_base::in);
	if (!f.good() || f.eof() || !f.is_open()) { return; }

	size_t p=size;
	f.read(buf, size);
	buf[p] = 0;

	f.close();
}