#ifndef OPTIONS_H
#define OPTIONS_H

#include <argp.h>
#include <string>

class Options
{
public:
	Options(int argc, char **argv);

	enum Mode { None, WriteTree, CompareTrees, WriteActions };

	Mode getMode() const
		{ return(mode); }
	const std::string &getOutFile() const
		{ return(outFile); }
	const std::string &getOldFile() const
		{ return(oldFile); }
	const std::string &getNewFile() const
		{ return(newFile); }
	const std::string &getBaseDir() const
		{ return(baseDir); }
	

private:
	static error_t parseOpt(int key, char *arg, struct argp_state *state);

	Mode mode;
	std::string outFile;
	std::string oldFile;
	std::string newFile;
	std::string baseDir;
};

#endif /* OPTIONS_H */
