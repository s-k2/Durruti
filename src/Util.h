#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string>

namespace Util {

class BinaryWriter {
public:
	BinaryWriter(const std::string &path);
	~BinaryWriter();

	void write(const std::string &str);
	void write(char ch);
	void write(time_t time);

private:
	FILE *binary;
};

class BinaryReader {
public:
	BinaryReader(const std::string &path);
	~BinaryReader();

	bool canRead();

	std::string readString();
	time_t readTime();
	char readChar();
	bool tryReadChar(char *singleChar);

private:
	FILE *binary;
};


}

#endif /* UTIL_H */
