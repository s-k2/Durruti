#include "Index.h"
using namespace Index;

#include <string.h>
#include <stdexcept>
using namespace std;

void Entry::read(FILE *index)
{
	bytes.resize(sizeof(char) + sizeof(uint64_t) + 2 * sizeof(char)); // save space for the bytes that are always present
	if(fread(&bytes[0], sizeof(char) + sizeof(uint64_t), 1, index) < 1)
		throw std::runtime_error("reading beyond file-end");

	if(getType() == EnterDir || getType() == File) {
		if(fread(&bytes[sizeof(char) + sizeof(uint64_t)], 2 * sizeof(char), 1, index) < 1)
			throw std::runtime_error("reading beyond file-end");
		while(bytes.back() != 0x00) {
			bytes.push_back(0x00);
			if(fread(&bytes[bytes.size() - 1], 1, 1, index) < 1)
				throw std::runtime_error("reading beyond file-end" + string(getName()));
		}
	}
}

Reader::Reader(const string &indexPath)
{
	binary = fopen(indexPath.c_str(), "rb");
	if(binary == NULL)
		throw std::runtime_error("Could not read file-index: " + indexPath);
}

Reader::~Reader()
{
	fclose(binary);
}

bool Reader::hasNext()
{
	return(!feof(binary));
}

const Entry &Reader::readNext()
{
	entry.read(binary);
	if(entry.getType() == Entry::EnterDir && !(entry.getName()[0] == '.' && entry.getName()[1] == 0x00)) {
		currentPath += entry.getName() + string("/");
	} else if(entry.getType() == Entry::LeaveDir) {
		size_t slashPos = currentPath.find_last_of('/', currentPath.size() - 2);
		if(slashPos == string::npos) // delete all if we're in the root
			currentPath.clear();
		else
			currentPath.erase(slashPos);
	}

	return(entry);
}

const Entry &Reader::getCurrent()
{
	return(entry);
}

Writer::Writer(const std::string &path)
{
	binary = fopen(path.c_str(), "wb");
	if(binary == NULL)
		throw std::runtime_error("Could not write file-index");
}

Writer::~Writer()
{
	fclose(binary);
}

void Writer::enterDir(const string &path)
{
	uint64_t mtime64 = 0xdeadbeef;
	fwrite(&mtime64, sizeof(mtime64), 1, binary);

	uint8_t header = Entry::EnterDir;
	fwrite(&header, sizeof(header), 1, binary);

	fwrite(path.c_str(), path.size() + 1, 1, binary);
}

void Writer::leaveDir()
{
	uint64_t mtime64 = 0xdeadbeef;
	fwrite(&mtime64, sizeof(mtime64), 1, binary);

	uint8_t header = Entry::LeaveDir;
	fwrite(&header, sizeof(header), 1, binary);
}

void Writer::putFile(const char *path, time_t mtime)
{
	uint64_t mtime64 = mtime;
	fwrite(&mtime64, sizeof(mtime64), 1, binary);

	uint8_t header = Entry::File;
	fwrite(&header, sizeof(header), 1, binary);

	fwrite(path, strlen(path) + 1, 1, binary);
}
