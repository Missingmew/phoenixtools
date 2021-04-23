#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "ir.h"
#include "asconfig.h"
#include "common.h"
#include "param.h"
#include "data_support.h"

int main(int argc, char **argv) {
	/* idea:
	- open
	- parse line by line, store labels with their addresses, store references to labels as-if
	- patch up all labels
	- emit
	- close
	*/
	FILE *f, *o;
	unsigned insize;
	char *input = NULL;
	struct ir_script *script = NULL;
	struct asconfig config;
	
	struct params param;
	
	if(!parse_args(&param, argc, argv, "bin")) return 1;
	
	config.gamenum = param.gamenum;
	config.isjp = param.isjp;
	
	if(!(f = fopen(param.infile, "r"))) {
		printf("couldnt open %s as input\n", param.infile);
		return 1;
	}
	
	if(!(o = fopen(param.outfile, "wb"))) {
		printf("couldnt open %s as output\n", param.outfile);
		return 1;
	}
	
	/* attempt to load support files now */
	data_loadfile(DATA_SOUND, param.soundfile);
	data_loadfile(DATA_SPEAKER, param.speakerfile);
	if(ISNDS(config.gamenum)) data_loadfile(DATA_ANIMATIONNDS, param.animfile);
	else data_loadfile(DATA_ANIMATIONGBA, param.animfile);
	data_loadfile(DATA_BACKGROUND, param.bgfile);
	data_loadfile(DATA_LOCATION, param.locationfile);
	
	fseek(f, 0, SEEK_END);
	insize = ftell(f);
	fseek(f, 0, SEEK_SET);
	input = malloc(insize+1);
	fread(input, insize, 1, f);
	input[insize] = 0;
	fclose(f);
	
	lexer_init(input);
	if(!lexer_scan()) printf("couldnt lex file\n");
	
	if(!(script = parser_parse(&config))) return 1;
	
	//~ ir_script_dump(script);
	
	if(!ir_script_preprocess(script, &config)) return 1;
	
	//~ ir_script_dump(script);
	/* for fixing up addresses for sections and labels */
	if(!ir_script_fixup(script)) return 1;
	
	//~ ir_script_dump(script);
	
	ir_script_emit(o, script);
	
	ir_script_free(script);
	
	lexer_finalize();
	
	free(input);
	fclose(o);
	
	params_cleanup(&param);
	data_cleanup();
}