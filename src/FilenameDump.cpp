#include "FilenameDump.h"

DumpedFilenameData::DumpedFilenameData(const char *filename, time_t mtime)
	: filename(filename), mtime(mtime)
{
}

DumpedFilename::DumpedFilename(DumpedFilenameData *data)
	: data(data)
{
}

DumpedFilename FilenameDump::add(const char *filename, time_t mtime)
{
	filenames.emplace_back(new DumpedFilenameData(filename, mtime));

	return(DumpedFilename(filenames.back().get()));
}
