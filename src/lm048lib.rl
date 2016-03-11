#include "lm048lib.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

%%{
	machine atcmd;

	access state->;

	action on_ok_response {
		state->on_ok_response();
	}

	action on_error_response {
		state->on_error_response();
	}

	action on_error {
		state->on_error(fcurs, fc);
	}

	action on_completed {
		if(state->on_completed != NULL){
			state->on_completed();
		}else{
			fbreak;
		}
	}

	cr = '\r';
	lf = '\n';
	crlf = cr lf;

	command_ok = crlf 'OK' crlf @on_ok_response;
	command_error = crlf 'ERROR' crlf @on_error_response;
	command_response = command_ok | command_error;

	responses = command_response; 

	at = [aA][tT];
	ver = '+' [vV][eE][rR];

	commands = at (cr | (ver) cr);

	main := (commands | responses) %~on_completed @!on_error;

	write data;
}%%

void lm048_no_op(){}
void lm048_no_op_e(int cs, char c){}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

struct lm048_parser lm048_default_state = {
	.cs = %%{ write start; }%%,
	.on_ok_response = lm048_no_op,
	.on_error_response = lm048_no_op,
	.on_error = lm048_no_op_e,
	.on_completed = NULL
};

enum LM048_ISTATUS lm048_inputs(struct lm048_parser *state, 
				   char const *data, 
				   size_t *length)
{
	if(*length > 0){
		char const *p = data;
		char const *pe = p + *length;
		char const *eof = NULL;

		%% write init nocs;
		%% write exec;

		*length = *length - (size_t)(p - data);
	}

	if(state->cs >= %%{ write first_final; }%%){
		state->cs = %%{ write start; }%%;
		return LM048_COMPLETED;
	}else if(state->cs == %%{ write error; }%%){
		state->cs = %%{ write start; }%%;
		return LM048_ERROR;
	}else{
		return LM048_OK;
	}
}

enum LM048_ISTATUS lm048_input(char const *data, size_t *length){
	enum LM048_ISTATUS s = 
		lm048_inputs(&lm048_default_state, data, length);
	return s;
}

void lm048_restart(struct lm048_parser *state){
	if(state == NULL){
		state = &lm048_default_state;
	}

	state->cs = %%{ write start; }%%;
}

void lm048_init(struct lm048_parser *state){
	state->cs = %%{ write start; }%%;
	state->on_ok_response = lm048_no_op;
	state->on_error_response = lm048_no_op;
	state->on_error = lm048_no_op_e;
	state->on_completed = NULL;
}
