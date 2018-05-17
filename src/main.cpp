#include "TreeRecorder.h"
#include "FileWalker.h"
#include "TreeDiff.h"
#include "IndexComparator.h"
#include "TreeDiffComparator.h"

#include "Options.h"

#include <time.h>

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif /* _LARGEFILE64_SOURCE */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/sendfile.h>

#include <sys/ioctl.h>
#include <linux/fs.h>

#include <atomic>

using namespace std;

class CopyAction
{
public:
	CopyAction(const std::string &sourcePath, const std::string &destPath)
		: sourcePath(sourcePath), destPath(destPath), source(-1), dest(-1), bytesCopied(0)
	{
	}

	~CopyAction()
	{
		if(source >= 0)
			checkedClose(source);

		if(dest >= 0)
			checkedClose(dest);
	}

	void checkedClose(int &fd)
	{
		int ret = close(fd);
		if(ret < 0)
			throw std::runtime_error("closing " + std::to_string(fd) + "file returned an error: " + string(strerror(errno)));
		fd = -1;
	}

	void init()
	{
		// open the input-file read-only, but return with an error if it is a symlink
		source = open(sourcePath.c_str(), O_RDONLY | O_LARGEFILE | O_NOFOLLOW | O_NONBLOCK);
		if(source < 0)
			throw std::runtime_error("open source-file " + sourcePath + " failed with " + string(strerror(errno)));

		dest = open(destPath.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_LARGEFILE | O_NONBLOCK, 0777);
		if(dest < 0)
			throw std::runtime_error("open dest-file " + destPath + " failed with " + string(strerror(errno)));
	}

	bool reflinkCopy()
	{
		// if reflink worked, return true
		if(ioctl(dest, FICLONE, source) != -1)
			return(true);

		if(errno == EXDEV)
			printf("Reflink not supported for those files, as they are not on the same filesystem\n");
		else if(errno == EOPNOTSUPP || errno == EINVAL)
			printf("Reflink not supported for this filesystem\n");
		else
			// throw an exception if there is another error with the file-descriptors
			throw std::runtime_error("reflink-error on " + sourcePath + ": " + string(strerror(errno)));

		// if it is just not supported return false
		return(false);
	}

	bool sendfileChunk()
	{
		off64_t currentPos = bytesCopied;
		static const size_t SIZE_OF_BLOCK = 1024 * 1024 * 100;

		ssize_t ret = sendfile64(dest, source, &currentPos, SIZE_OF_BLOCK);
		if(ret < 0)
			throw std::runtime_error("sendfile error on " + sourcePath + ": " + string(strerror(errno)));
		// TODO: handle EAGAIN

		bytesCopied = currentPos;

		return(ret == SIZE_OF_BLOCK);
	}

	bool copyChunk()
	{
		static const size_t SIZE_OF_BLOCK = 1024;

		// TODO: determine block-size just once and do this in initialization
		copyBuffer.reset(new char[SIZE_OF_BLOCK]);

		ssize_t ret = read(source, (void *) copyBuffer.get(), SIZE_OF_BLOCK);
		if(ret < 0)
			throw std::runtime_error("read error on " + sourcePath + ": " + string(strerror(errno)));

		// we reached the end of the file, so tell this the caller
		if(ret == 0)
			return(false);

		// write the number of bytes read
		ret = write(dest, (void *) copyBuffer.get(), ret);
		if(ret < 0)
			throw std::runtime_error("write error on " + sourcePath + ": " + string(strerror(errno)));

		return(true);
	}
	
	inline off64_t getBytesCopied() const
		{ return(bytesCopied); }

private:
	std::string sourcePath;
	std::string destPath;

	int source;
	int dest;
	std::atomic<off64_t> bytesCopied;
	std::unique_ptr<char[]> copyBuffer;
};

struct TreeDiffSink
{
	void copyToRight(const TreeDiff::Reader &diff)
	{
		CopyAction action("test-tree/a/" + diff.getCurrentDir() + diff.getCurrentFile(),
					"test-tree/b/" + diff.getCurrentDir() + diff.getCurrentFile());
		action.init();
		while(action.copyChunk())
			;
	}
	
	void copyToLeft(const TreeDiff::Reader &diff)
	{
		CopyAction action("test-tree/b/" + diff.getCurrentDir() + diff.getCurrentFile(),
				"test-tree/a/" + diff.getCurrentDir() + diff.getCurrentFile());
		action.init();
		while(action.copyChunk())
			;
	}

	void conflict(const TreeDiff::Reader &left, const TreeDiff::Reader &right)
	{
		printf("\x1B[31m%*s  %c === %c  %s\x1B[0m\n", 50, 
			(left.getCurrentDir() + left.getCurrentFile()).c_str(), (char) left.getCurrentType(), 
			(char) right.getCurrentType(), (right.getCurrentDir() + right.getCurrentFile()).c_str());

		printf("%*s    ===    %s\n", 
			50, timeToString(left.getCurrentMTime()).c_str(), 
			timeToString(right.getCurrentMTime()).c_str());
	}

	std::string timeToString(time_t timeVal)
	{
		char timeStr[101];
		struct tm* tmInfo;

		tmInfo = localtime(&timeVal);
		strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S.000", tmInfo);
		return(timeStr);
	}


};

int main(int argc, char *argv[])
{
	Options options(argc, argv);

	if(options.getMode() == Options::WriteTree) {
		TreeRecorder recorder(options.getOutFile());
		FileWalker<TreeRecorder> fileWalker(options.getBaseDir(), recorder);
	} else if(options.getMode() == Options::CompareTrees) {
		TreeDiff::Writer writer(options.getOutFile());
		Index::Comparator<TreeDiff::Writer> comp(options.getOldFile(), options.getNewFile(), writer);
	} else if(options.getMode() == Options::WriteActions) {
		TreeDiffSink sink;
		TreeDiff::Comparator<TreeDiffSink> comparator(options.getOldFile(), options.getNewFile(), sink);
		//"tree-diff.left.durruti", "tree-diff.right.durruti", sink);
	}

#if 0
	const char *basePath = ".";

	if(argc > 1)
		basePath = argv[1];
	// record the complete file-tree
	

	// compare it with the last sync to get all changed files
	{
		}

	TreeDiff::Reader reader("tree-diff.durruti");
	while(reader.readNext()) {
		printf("%c %s%s\n", reader.getCurrentType(), reader.getCurrentDir().c_str(), reader.getCurrentFile().c_str());
	}

//#else


	TreeDiffSink sink;
	TreeDiff::Comparator<TreeDiffSink> comparator("tree-diff.left.durruti", "tree-diff.right.durruti", sink);
	
	// now compare it with the foreign changes
#endif
	return(0);
}
