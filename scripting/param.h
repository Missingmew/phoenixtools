#ifndef PARAM_H_
#define PARAM_H_

struct params {
	/* required */
	char *infile;
	unsigned gamenum;
	
	/* optional/automatic */
	unsigned isjp;
	unsigned isunity;
	char *outfile;
	
	/* support */
	char *soundfile;
	char *speakerfile;
	char *animfile;
	char *bgfile;
	char *locationfile;
	char *evidencefile;
	
	/* state */
	unsigned autoalloc_out;
};

unsigned parse_args(struct params *ret, int argc, char **argv, char *autosuffix);
void print_help(char *argv0);
void params_cleanup(struct params *par);

#endif
