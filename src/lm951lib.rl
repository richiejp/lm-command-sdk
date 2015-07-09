#include "lm951lib.h"

static struct ragelState globalRagelState = { 0 };

%%{
	machine atcmd;

	main := [aA][tT];

	write data;
}%%

int lm951InputWith(struct ragelState *state, char *data, int length){
	int cs = state->currentState;

	if(length > 0){
		char *p = data;
		char *pe = p + length;

		%%{
			write init;
			write exec;
		}%%

		state->currentState = 0;
	}

	return cs >= atcmd_first_final;
}

int lm951Input(char *data, int length){
	return lm951InputWith(&globalRagelState, data, length);
}

