#include "TreeRecorder.h"

using namespace std;

TreeRecorder::TreeRecorder(const string &path)
	: index(path)
{
}

void TreeRecorder::enterDir(const char *dirname)
{
	unfinishedDirs.emplace(dirname);
}

void TreeRecorder::processFile(const char *filename, time_t mtime)
{
	DumpedFilename dumpedFile = filenameDump.add(filename, mtime);

	unfinishedDirs.top().addFile(dumpedFile);
}

void TreeRecorder::leaveDir()
{
	DumpedDir dumpedDir = dirDump.add(unfinishedDirs.top());
	unfinishedDirs.pop();

	if(!unfinishedDirs.empty())
		unfinishedDirs.top().addSubDir(dumpedDir);
	else
		writeDir(dumpedDir);
}

#if 0
void TreeRecorder::printDir(DumpedDir dumpedDir, int level)
{
	printf("%s%s\n", string(level, '\t').c_str(), dumpedDir.getDirname().c_str());
	for(auto subDir : dumpedDir.getSubDirs())
		printDir(subDir, level + 1);
	for(auto file : dumpedDir.getFiles())
		printf("%s%s\n", string(level + 1, '\t').c_str(), file.getFilename());//.c_str());
}
#endif

void TreeRecorder::writeDir(DumpedDir dumpedDir)
{
	index.enterDir(dumpedDir.getDirname());
	for(auto file : dumpedDir.getFiles())
		index.putFile(file.getFilename(), file.getMTime());
	for(auto subDir : dumpedDir.getSubDirs())
		writeDir(subDir);
	index.leaveDir();
}

