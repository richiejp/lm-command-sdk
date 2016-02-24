#include "lm951lib.h"

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

	commands = (at cr);

	main := (commands | responses) %~on_completed @!on_error;

	write data;
}%%

void lm951_no_op(){}
void lm951_no_op_e(int cs, char c){}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

struct lm951_parser default_state = {
	.cs = %%{ write start; }%%,
	.on_ok_response = lm951_no_op,
	.on_error_response = lm951_no_op,
	.on_error = lm951_no_op_e,
	.on_completed = NULL
};

enum LM951_ISTATUS lm951_inputs(struct lm951_parser *state, 
				   char *data, 
				   size_t *length)
{
	if(*length > 0){
		char *p = data;
		char *pe = p + *length;
		char *eof = NULL;

		%% write init nocs;
		%% write exec;

		*length = *length - (size_t)(p - data);
	}

	if(state->cs >= %%{ write first_final; }%%){
		default_state.cs = %%{ write start; }%%;
		return LM951_COMPLETED;
	}else if(state->cs == %%{ write error; }%%){
		default_state.cs = %%{ write start; }%%;
		return LM951_ERROR;
	}else{
		return LM951_OK;
	}
}

enum LM951_ISTATUS lm951_input(char *data, size_t *length){
	enum LM951_ISTATUS s = lm951_inputs(&default_state, data, length);
	return s;
}

void lm951_restart(struct lm951_parser *state){
	if(state == NULL){
		state = &default_state;
	}

	state->cs = %%{ write start; }%%;
}
