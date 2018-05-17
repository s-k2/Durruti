#ifndef INDEX_H
#define INDEX_H

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

namespace Index {

class Entry
{
public:
	enum Type { EnterDir = 'D', File = 'A', LeaveDir = 'L' };
	struct Layout
	{
		uint64_t mtime;
		uint8_t type;
		char name[];
	};

	void read(FILE *index);

	inline Type getType() const
		{ return((Type) getData()->type); };
	inline time_t getMTime() const
		{ return(getData()->mtime); };
	inline const char *getName() const
		{ return(getData()->name); };

private:
	std::vector<uint8_t> bytes;

	inline Layout *getData() const
		{ return((Layout *) &bytes[0]); };
};

class Reader
{
public:
	Reader(const std::string &indexPath);
	~Reader();

	bool hasNext();
	const Entry &readNext();
	const Entry &getCurrent();

	const std::string &getCurrentPath()
		{ return(currentPath); }
private:
	FILE *binary;
	Entry entry;
	std::string currentPath;
};

class Writer
{
public:
	Writer(const std::string &path);
	~Writer();

	void enterDir(const std::string &path);
	void leaveDir();
	void putFile(const char *path, time_t mtime);
private:
	FILE *binary;
};

}

#endif /* INDEX_H */
