#include "Index.h"

#include <string.h>
#include <string>

namespace Index {

template <class T>
class Comparator
{
public:
	Comparator(const std::string &oldPath, const std::string &currentPath, T &sink)
		: oldIndex(oldPath), newIndex(currentPath), sink(sink)
	{
		oldIndex.readNext();
		newIndex.readNext(); // read the first directory-entry (.)

		compareDir();
	}

private:
	typedef void (T::*FileCallback)(const char *file, const std::string &dir, time_t mtime);

	Reader oldIndex;
	Reader newIndex;
	T &sink;

	void compareDir()
	{
		while(1) {
			oldIndex.readNext();
			newIndex.readNext();

			readTilTypeAndNameMatch();
			if(oldIndex.getCurrent().getType() == Entry::LeaveDir)
				return; // we're done with this directory
			else if(oldIndex.getCurrent().getType() == Entry::EnterDir)
				compareDir(); // recursively read this sub-directory directory
			else if(oldIndex.getCurrent().getType() == Entry::File)
				compareFile();
		}
	}

	void readTilTypeAndNameMatch()
	{
		int compareResult;
		do {
			compareResult = compareTypeAndName();
			if(compareResult > 0)
				processNextWithSubs(newIndex, &T::onNewFile);
			else if(compareResult < 0)
				processNextWithSubs(oldIndex, &T::onDeletedFile);
		} while(compareResult != 0);
		// in a valid index at least the LeaveDir-entries match, so there's no need to specify an extra abort-condition
	}

	// < 0: new comes after old, 0: both are equal, > 0: old is greater than new
	int compareTypeAndName()
	{
		if(oldIndex.getCurrent().getType() != newIndex.getCurrent().getType())
			return(oldIndex.getCurrent().getType() - newIndex.getCurrent().getType());

		if(oldIndex.getCurrent().getType() == Entry::LeaveDir) // both are Leaves
			return(0);

		return(strcmp(oldIndex.getCurrent().getName(), newIndex.getCurrent().getName()));
	}

	void processNextWithSubs(Reader &reader, FileCallback callback)
	{
		if(reader.getCurrent().getType() == Entry::File)
			(sink.*callback)(reader.getCurrent().getName(), reader.getCurrentPath(), reader.getCurrent().getMTime());
		else if(reader.getCurrent().getType() == Entry::EnterDir)
			processSub(reader, callback);
		reader.readNext();
	}

	void processSub(Reader &reader, FileCallback callback)
	{
		while(1) {
			reader.readNext();
			if(reader.getCurrent().getType() == Entry::LeaveDir)
				return; // we're done with this directory
			else if(reader.getCurrent().getType() == Entry::EnterDir)
				processSub(reader, callback); // recursively read this sub-directory directory
			else if(reader.getCurrent().getType() == Entry::File)
				(sink.*callback)(reader.getCurrent().getName(), reader.getCurrentPath(), reader.getCurrent().getMTime());
		}
	}

	void compareFile()
	{
		if(oldIndex.getCurrent().getMTime() != newIndex.getCurrent().getMTime())
			sink.onModifiedFile(newIndex.getCurrent().getName(), newIndex.getCurrentPath(), newIndex.getCurrent().getMTime());
	}
};

}

