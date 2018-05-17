#include "Options.h"

const char *argp_program_version = "durruti - v0.1";
const char *argp_program_bug_address = "<no-mail@example.com>";

Options::Options(int argc, char **argv)
	: mode(None), baseDir(".")
{
	static char doc[] = "durruti - a file-synchronisation tool"; // give us a name

	/* The options we understand. */
	static struct argp_option options[] = {
		{"write-tree", 't', "FILE", 0, "output tree to FILE" },
		{"compare-tree", 'c', "FILE", 0, "compare two trees" },
		{"write-actions", 'a', "FILE", 0, "determine the actions needed to synchronize" },
		{"old",   'o', "FILE", 0, "input from FILE is old file for compare" },
		{"new",   'n', "FILE", 0, "input from FILE is new file for compare" },
		{ 0 }
	};
	/* Our argp parser. */
	static struct argp argp = { options, parseOpt, "BASE-DIR", doc };

	/* Parse our arguments; every option seen by parse_opt will
	   be reflected in arguments. */
	argp_parse(&argp, argc, argv, 0, 0, this);

}

error_t Options::parseOpt(int key, char *arg, struct argp_state *state)
{
	register Options *thisOptions = (Options *) state->input;
	switch (key)
	{
		case 't':
			thisOptions->mode = WriteTree;
			thisOptions->outFile = arg;
			break;

		case 'c':
			thisOptions->mode = CompareTrees;
			thisOptions->outFile = arg;
			break;

		case 'a':
			thisOptions->mode = WriteActions;
			thisOptions->outFile = arg;
			break;

		case 'o':
			thisOptions->oldFile = arg;
			break;
		case 'n':
			thisOptions->newFile = arg;
			break;

		case ARGP_KEY_ARG:
			if(state->arg_num >= 1)
				argp_usage(state);
			thisOptions->baseDir = arg;
			break;


		case ARGP_KEY_END:
			if(thisOptions->mode == None)
				argp_usage(state);
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

