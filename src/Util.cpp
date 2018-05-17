#include "Util.h"

#include <stdexcept>

using namespace Util;

BinaryWriter::BinaryWriter(const std::string &path)
{
	binary = fopen(path.c_str(), "wb");
	if(binary == NULL)
		throw std::runtime_error("Could not write to file: " + path);
}

BinaryWriter::~BinaryWriter()
{
	fclose(binary);
}

void BinaryWriter::write(const std::string &str)
{
	if(fwrite(str.c_str(), str.size() + 1, 1, binary) != 1) // include trailing '\0'
		throw std::runtime_error("input-output-error while writing");
}

void BinaryWriter::write(char ch)
{
	if(fwrite(&ch, sizeof(ch), 1, binary) != 1)
		throw std::runtime_error("input-output-error while writing");
}

void BinaryWriter::write(time_t time)
{
	uint64_t timeAsInt = time;
	if(fwrite(&timeAsInt, sizeof(timeAsInt), 1, binary) != 1)
		throw std::runtime_error("input-output-error while writing");
}

BinaryReader::BinaryReader(const std::string &path)
{
	binary = fopen(path.c_str(), "rb");
	if(binary == NULL)
		throw std::runtime_error("Could not read from file: " + path);
}

BinaryReader::~BinaryReader()
{
	fclose(binary);
}

bool BinaryReader::canRead()
{
	return(!feof(binary));
}

std::string BinaryReader::readString()
{
	std::string str;
	char ch;
	while((ch = readChar()) != 0x00)
		str += ch;

	return(str);
}

char BinaryReader::readChar()
{
	char ch;
	if(!tryReadChar(&ch))
		throw std::runtime_error("reading beyond file-end");

	return(ch);
}

time_t BinaryReader::readTime()
{
	uint64_t timeAsInt;
	if(fread(&timeAsInt, sizeof(timeAsInt), 1, binary) != 1)
		throw std::runtime_error("reading beyond file-end");
	return((time_t) timeAsInt);
}

bool BinaryReader::tryReadChar(char *singleChar)
{
	return(fread(singleChar, sizeof(char), 1, binary) == 1);
}
