#ifndef TREE_DIFF_H
#define TREE_DIFF_H

#include "Util.h"
#include <string>

namespace TreeDiff {

enum class Type: char { DeletedFile = 'D', ModifiedFile = 'M', NewFile = 'N', ChangeDir = 'c', EndOfFile = 'e' };
inline bool isFileType(Type type)
	{ return((char) type < (char) Type::ChangeDir); };

class Writer
{
public:
	Writer(const std::string &path);
	~Writer();

	void onNewFile(const char *filename, const std::string &dir, time_t mtime)
		{ putChange(Type::NewFile, filename, dir, mtime); }
	void onModifiedFile(const char *filename, const std::string &dir, time_t mtime)
		{ putChange(Type::ModifiedFile, filename, dir, mtime); }
	void onDeletedFile(const char *filename, const std::string &dir, time_t mtime)
		{ putChange(Type::DeletedFile, filename, dir, mtime); }

private:
	void putChange(Type type, const char *filename, const std::string &dir, time_t mtime);

	Util::BinaryWriter writer;
	std::string currentDir;
};

class Reader
{
public:
	Reader(const std::string &path);

	bool readNext();

	Type getCurrentType() const
		{ return(currentType); };
	const std::string &getCurrentFile() const
		{ return(currentFile); };
	const std::string &getCurrentDir() const
		{ return(currentDir); };
	const time_t getCurrentMTime() const
		{ return(currentMTime); };
	
private:
	Util::BinaryReader reader;
	std::string currentDir;
	std::string currentFile;
	time_t currentMTime;
	Type currentType;
};

}

#endif /* TREE_DIFF_H */
