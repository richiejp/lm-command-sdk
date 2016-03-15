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
		pkt.type = LM048_AT_OK;
		state->on_ok_response();
	}

	action on_error_response {
		pkt.type = LM048_AT_ERROR;
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

static struct lm048_packet default_queue_array[LM048_DEFAULT_QUEUE_LENGTH][2];
static struct lm048_queue default_queue = {
	.array = default_queue_array,
	.front = 0,
	.back = 0,
	.length = LM048_DEFAULT_QUEUE_LENGTH
};

struct lm048_parser lm048_default_state = {
	.cs = %%{ write start; }%%,
	.on_ok_response = lm048_no_op,
	.on_error_response = lm048_no_op,
	.on_error = lm048_no_op_e,
	.on_completed = NULL,
	.queue = &default_queue
};

enum LM048_STATUS lm048_enqueue(struct lm048_queue *const queue,
				struct lm048_packet const command,
				struct lm048_packet const response){
	struct lm048_queue *que = queue;
	if(que == NULL){
		que = &default_queue;
	}
	que->array[que->back][0] = command;
	que->array[que->back][1] = response;
	if(que->length <= ++que->back){
		que->back = 0;
		if(que->back == que->front){
			return LM048_FULL;
		}
	}

	return LM048_COMPLETED;
}

static enum LM048_STATUS dequeue(struct lm048_queue *const queue,
				 struct lm048_packet const received){
	if(queue->front == queue->back){
		return LM048_COMPLETED;
	}

	struct lm048_packet *expected = &(queue->array[queue->front][1]);
	if(queue->length <= ++queue->front){
		queue->front = 0;
	}

	if(expected->type != received.type){
		return LM048_UNEXPECTED;
	}

	return LM048_DEQUEUED;
}

enum LM048_STATUS lm048_next_in_queue(struct lm048_packet const *const pkt)
{
	return LM048_COMPLETED;
}

struct lm048_queue 
lm048_init_queue(struct lm048_packet (*const array)[2],
		 size_t const length)
{
	struct lm048_queue q = {
		.array = array,
		.front = 0,
		.back = 0,
		.length = length
	};
	return q;
}

enum LM048_STATUS lm048_inputs(struct lm048_parser *const state, 
			       char const *const data, 
			       size_t *const length)
{
	struct lm048_packet pkt = {
		.type = LM048_AT_NONE
	};

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
		return dequeue(state->queue, pkt);
	}else if(state->cs == %%{ write error; }%%){
		state->cs = %%{ write start; }%%;
		return LM048_ERROR;
	}else{
		return LM048_OK;
	}
}

enum LM048_STATUS lm048_input(char const *const data, size_t *const length)
{
	enum LM048_STATUS s = 
		lm048_inputs(&lm048_default_state, data, length);
	return s;
}

void lm048_restart(struct lm048_parser *state)
{
	if(state == NULL){
		state = &lm048_default_state;
	}

	state->cs = %%{ write start; }%%;
}

void lm048_init(struct lm048_parser *state)
{
	state->cs = %%{ write start; }%%;
	state->on_ok_response = lm048_no_op;
	state->on_error_response = lm048_no_op;
	state->on_error = lm048_no_op_e;
	state->on_completed = NULL;
	state->queue = &default_queue;
}

