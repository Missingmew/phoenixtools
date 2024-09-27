#ifndef PARAM_H_
#define PARAM_H_

struct params {
	/* required */
	char *infile;
	unsigned gamenum;
	
	/* optional/automatic */
	unsigned isstd;
	unsigned isjp;
	unsigned isunity;
	char *outfile;
	char * scriptName;
	
	/* support */
	char *soundfile;
	char *speakerfile;
	char *persfile;
	char *bgfile;
	char *locationfile;
	char *evidencefile;
	char *animfile;
	
	/* state */
	unsigned autoalloc_out;
};

unsigned parse_args(struct params *ret, int argc, char **argv, char *autosuffix);
void print_help(char *argv0);
void params_cleanup(struct params *par);

#endif
