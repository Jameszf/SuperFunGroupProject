#ifndef sysec
#define sysec
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <stdlib.h>

namespace sysc
{
	bool pathIsValid(const std::string path)
	{
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}

	void raise(const char * error)
	{
		MessageBox(NULL, error, "Error", MB_OK);
		std::cin;
		std::exit(-1);
	}
}

#endif