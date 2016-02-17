#include "lm951lib.h"

static struct lm951_parser sstate = {
	.cs = 0
};

%%{
	machine atcmd;

	access state->;

	main := [aA][tT];

	write data;
}%%

enum LM951_ISTATUS lm951_inputs(struct lm951_parser *state, 
				   char *data, 
				   int length)
{
	if(length > 0){
		char *p = data;
		char *pe = p + length;

		%%{
			write init;
			write exec;
		}%%

	}

	if(state->cs >= atcmd_first_final){
		return LM951_COMPLETED;
	}else{
		return LM951_OK;
	}
}

enum LM951_ISTATUS lm951_input(char *data, int length){
	enum LM951_ISTATUS s = lm951_inputs(&sstate, data, length);
	if(s == LM951_COMPLETED){
		sstate.cs = 0;
	}
	return s;
}
