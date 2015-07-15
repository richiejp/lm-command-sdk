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

int lm951_input(struct lm951_parser *state, char *data, int length)
{
	if(length > 0){
		char *p = data;
		char *pe = p + length;

		%%{
			write init;
			write exec;
		}%%

	}

	return state->cs >= atcmd_first_final;
}

int lm951_input(char *data, int length){
	return lm951_input(&sstate, data, length);
}

