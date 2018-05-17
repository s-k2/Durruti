#ifndef FILENAME_DUMP_H
#define FILENAME_DUMP_H

/* this has nothing to do with specs, just a dummy! */

#include <list>
#include <memory>
#include <string>

class DumpedFilenameData
{
private:
	DumpedFilenameData(const char *filename, time_t mtime);

	std::string filename;
	time_t mtime;

	friend class FilenameDump;
	friend class DumpedFilename;
};

class DumpedFilename
{
public:
	inline time_t getMTime() const
		{ return(data->mtime); };
	inline const char *getFilename() const
		{ return(data->filename.c_str()); };
	inline bool operator < (const DumpedFilename &other) const
		{ return(data->filename < other.data->filename); };
private:
	DumpedFilename(DumpedFilenameData *data);

	DumpedFilenameData *data;

	friend class FilenameDump;
};

class FilenameDump
{
public:
	DumpedFilename add(const char *filename, time_t mtime);

private:
	std::list<std::unique_ptr<DumpedFilenameData>> filenames;
};

#endif /* FILENAME_DUMP_H */
