#include "TreeDiff.h"

using namespace TreeDiff;

Writer::Writer(const std::string &path)
	: writer(path)
{
}

Writer::~Writer()
{
	writer.write((char) Type::EndOfFile);
}

void Writer::putChange(Type type, const char *filename, const std::string &dir, time_t mtime)
{
	if(dir != currentDir) {
		writer.write((char) Type::ChangeDir);
		writer.write(dir);
		currentDir = dir;
	}
	writer.write((char) type);
	writer.write(filename);
	writer.write(mtime);
}

Reader::Reader(const std::string &path)
	: reader(path)
{
}

bool Reader::readNext()
{
	currentType = (Type) reader.readChar();
	if(currentType == Type::ChangeDir) {
		currentDir = reader.readString();
	} else if(isFileType(currentType)) {
		currentFile = reader.readString();
		currentMTime = reader.readTime();
	} else {
		currentDir = currentFile = "";
	}

	return(true);
}
