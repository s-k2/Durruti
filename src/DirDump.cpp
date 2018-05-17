#include "DirDump.h"

#include <algorithm>

DumpedDir::DumpedDir(const UnfinishedDir *data)
	: data(data)
{
}

UnfinishedDir::UnfinishedDir(const std::string &dirname)
	: dirname(dirname)
{
}

void UnfinishedDir::addSubDir(DumpedDir dumpedDir)
{
	subDirs.emplace_back(dumpedDir);
}

void UnfinishedDir::addFile(DumpedFilename dumpedFilename)
{
	files.emplace_back(dumpedFilename);
}

void UnfinishedDir::sort()
{
	std::sort(files.begin(), files.end());
	std::sort(subDirs.begin(), subDirs.end());
}

DumpedDir DirDump::add(const UnfinishedDir &dir)
{
	dirs.emplace_back(new UnfinishedDir(dir));
	dirs.back()->sort();
	return(DumpedDir(dirs.back().get()));
}
