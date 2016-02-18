#include "lm951lib.h"

%%{
	machine atcmd;

	access state->;

	cr = '\r';
	lf = '\n';
	crlf = cr lf;

	command_ok = crlf 'OK' crlf;
	command_error = crlf 'ERROR' crlf;
	command_response = command_ok | command_error;

	responses = command_response;

	at = [aA][tT];

	commands = at cr;

	main := commands | responses;

	write data;
}%%

static struct lm951_parser sstate = {
	.cs = atcmd_start
};

enum LM951_ISTATUS lm951_inputs(struct lm951_parser *state, 
				   char *data, 
				   int length)
{
	if(length > 0){
		char *p = data;
		char *pe = p + length;

		%% write init nocs;
		%% write exec;

	}

	if(state->cs >= atcmd_first_final){
		return LM951_COMPLETED;
	}else if(state->cs == atcmd_error){
		return LM951_ERROR;
	}else{
		return LM951_OK;
	}
}

enum LM951_ISTATUS lm951_input(char *data, int length){
	enum LM951_ISTATUS s = lm951_inputs(&sstate, data, length);
	if(s == LM951_COMPLETED || s == LM951_ERROR){
		sstate.cs = atcmd_start;
	}
	return s;
}
