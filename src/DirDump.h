#ifndef DIR_DUMP_H
#define DIR_DUMP_H

#include <vector>
#include <string>
#include <utility>

#include "FilenameDump.h"

class UnfinishedDir;

class DumpedDir
{
public:
	inline const std::string &getDirname() const;
	inline const std::vector<DumpedFilename> &getFiles() const;
	inline const std::vector<DumpedDir> &getSubDirs() const;
	inline bool operator < (const DumpedDir &other) const;

private:
	DumpedDir(const UnfinishedDir *unfinishedDir);

	const UnfinishedDir *data;

	friend class DirDump;
};

class UnfinishedDir
{
public:
	UnfinishedDir(const std::string &dirname);
	void addSubDir(DumpedDir dumpedDir);
	void addFile(DumpedFilename dumpedFilename);

private:
	void sort();

	std::string dirname;
	std::vector<DumpedFilename> files;
	std::vector<DumpedDir> subDirs;

	friend class DumpedDir;
	friend class DirDump;
};

inline const std::string &DumpedDir::getDirname() const
{
	return(data->dirname);
}
inline const std::vector<DumpedFilename> &DumpedDir::getFiles() const
{
	return(data->files);
}
inline const std::vector<DumpedDir> &DumpedDir::getSubDirs() const
{
	return(data->subDirs);
}
inline bool DumpedDir::operator < (const DumpedDir &other) const
{
	return(data->dirname < other.data->dirname);
}

class DirDump
{
public:
	DumpedDir add(const UnfinishedDir &dir);

private:
	std::vector<std::unique_ptr<UnfinishedDir>> dirs;
};

#endif /* DIR_DUMP_H */
