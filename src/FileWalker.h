#ifndef FILE_WALKER_H
#define FILE_WALKER_H

#include <ftw.h>
#include <stack>
#include <string>

#include <fstream>

#define MAX_NFTW_FDS 64

extern void *thisWalkerVoid; // TODO: Don't use nftw any longer -> no need to do this ugly hack

template <class T>
class FileWalker
{
public:
	FileWalker(const std::string &baseDir, T &sink)
		: currentLevel(0), sink(sink)
	{
		thisWalkerVoid = this;
		if(nftw64(baseDir.c_str(), FileWalker<T>::walkCallback<T>, MAX_NFTW_FDS, FTW_PHYS) < 0) {
			fprintf(stderr, "nftw64 error\n");
			return;
		}
		thisWalkerVoid = NULL;

		while(currentLevel > 0) {
			currentLevel--;
			onLeaveDir();
		}
	}

private:
	int currentLevel;
	std::stack<std::string> directoryStack;
	T &sink;

	template <class TCallback>
	static int walkCallback(const char *path, const struct stat64 *st, int tflag, struct FTW *ftwInfo)
	{
		auto thisWalker = (FileWalker<TCallback> *) thisWalkerVoid;
		thisWalker->onItem(path, st, tflag, ftwInfo);

		return(0);
	}

	void onItem(const char *path, const struct stat64 *st, int tflag, struct FTW *ftwInfo)
	{
		while(ftwInfo->level < currentLevel) {
			currentLevel--;
			onLeaveDir();
		}

		if(tflag == FTW_D) {
			currentLevel++;
			onEnterDir(path, st, tflag, ftwInfo);
		} else if(tflag == FTW_DNR) {
			// TODO: some kind of error handling???
		} else if(tflag == FTW_NS) {
			// TODO: some kind of error handling???
		} else if(tflag == FTW_F) {
			onFile(path, st, tflag, ftwInfo);
		}
	}

	void onEnterDir(const char *path, const struct stat64 *st, int tflag, struct FTW *ftwInfo)
	{
		if(ftwInfo->level == 0)
			sink.enterDir(".");
		else
			sink.enterDir(path + ftwInfo->base);
	}
	
	void onLeaveDir()
	{
		sink.leaveDir();
	}

	void onFile(const char *path, const struct stat64 *st, int tflag, struct FTW *ftwInfo)
	{
		sink.processFile(path + ftwInfo->base, st->st_mtime);
	}
};

#endif /* FILE_WALKER_H */
