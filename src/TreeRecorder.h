#ifndef TREE_RECORDER_H
#define TREE_RECORDER_H

#include <stack>
#include <string>

#include "DirDump.h"
#include "FilenameDump.h"
#include "Index.h"

class TreeRecorder
{
public:
	TreeRecorder(const std::string &path);
	void enterDir(const char *dirname);
	void processFile(const char *filename, time_t mtime);
	void leaveDir();
private:
	std::stack<UnfinishedDir> unfinishedDirs;

	FilenameDump filenameDump;
	DirDump dirDump;

	Index::Writer index;
	void printDir(DumpedDir dumpedDir, int level = 0);
	void writeDir(DumpedDir dumpedDir);
};

#endif /* TREE_RECORDER_H */
