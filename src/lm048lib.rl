/** Copyright LM Technologies 2016
 *
 * This file is licensed under the terms of the Adaptive Public License 
 * set forth in licenses.txt in the root directory of the LM Command SDK 
 * repository.
 *
 * Author(s): Richard Palethorpe <richard@lm-technologies.com>
 */

#include "lm048lib.h"
#include <string.h>

#define CR LM048_COMMAND_DELIMETER
#define CRLF LM048_RESPONSE_DELIMETER
#define ATP "at+"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

%%{
	machine skipline;

	main := (^'\r')* '\r' @{ fbreak; };
	
	write data;
}%%

enum LM048_STATUS lm048_skip_line(char *const data, size_t *const length)
{
	if(*length > 0){
		int cs;
		char *p = data;
		char *pe = p + *length;

		%% write init;
		%% write exec;

		*length -= (size_t)(p - data);

		if(cs == %%{ write first_final; }%%){
			return LM048_COMPLETED;
		}
	}

	return LM048_OK;
}

%%{
	machine atcmd;

	access state->;

	action on_ok_response {
		pkt->type = LM048_AT_OK;
		state->on_ok_response();
	}

	action on_error_response {
		pkt->type = LM048_AT_ERROR;
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

	action on_at_at {
		pkt->type = LM048_AT_AT;
	}

	action on_ver {
		pkt->type = LM048_AT_VER;
	}

	action on_ver_response {
		pkt->type = LM048_AT_VER_RESPONSE;
	}

	action add_to_payload {
		payload_add(pkt, *p);
	}

	action clear_payload {
		pkt->payload_length = 0;
	}

	cr = '\r';
	lf = '\n';
	crlf = cr lf;

	ok = 'OK' crlf; 
	error = 'ERROR' crlf;
	command_response = lf .
			   (ok @on_ok_response | 
			    error @on_error_response);

	ver_resp = lf 'F/W VERSION:' ' '{1,5} .
		   'v' @clear_payload .
		   (digit{1,2} '.' digit{2,3}) $add_to_payload .
		   ' '{,5} crlf ok @on_ver_response;

	responses = command_response | ver_resp; 

	at = [aA][tT];
	at_at = at cr @on_at_at;
	ver = at '+' [vV][eE][rR] cr @on_ver;

	commands = at_at | ver;

	main := cr* (commands | responses) %~on_completed @!on_error;
	
	write data;
}%%

void lm048_no_op(){}
void lm048_no_op_e(int cs, char c){}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

static struct lm048_packet
default_queue_array[LM048_DEFAULT_QUEUE_LENGTH][2];

static struct lm048_queue default_queue = {
	.array = default_queue_array,
	.front = 0,
	.back = 0,
	.length = LM048_DEFAULT_QUEUE_LENGTH
};

static char default_payload[LM048_DEFAULT_PAYLOAD_LENGTH];
struct lm048_parser lm048_default_state = {
	.cs = %%{ write start; }%%,
	.on_ok_response = lm048_no_op,
	.on_error_response = lm048_no_op,
	.on_error = lm048_no_op_e,
	.on_completed = NULL,
	.queue = &default_queue,
	.current.payload = default_payload,
	.current.payload_length = 0,
	.current.payload_capacity = LM048_DEFAULT_PAYLOAD_LENGTH
};

static enum LM048_STATUS payload_add(struct lm048_packet *const pkt, char c)
{
	if(pkt->payload_length < pkt->payload_capacity){
		pkt->payload[pkt->payload_length] = c;
		pkt->payload_length += 1;
		return LM048_COMPLETED;
	}

	return LM048_FULL;
}

enum LM048_STATUS lm048_packet_init(struct lm048_packet *const pkt,
				char *const payload,
				size_t payload_capacity)
{
	pkt->type = LM048_AT_NONE;
	pkt->modifier = LM048_ATM_QUERY;
	pkt->payload = payload;
	pkt->payload_length = 0;
	pkt->payload_capacity = payload_capacity;

	return LM048_COMPLETED;
}

enum LM048_STATUS lm048_enqueue(struct lm048_queue *const queue,
				struct lm048_packet const command,
				struct lm048_packet const response)
{
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
				 struct lm048_packet const *const received){
	if(queue->front == queue->back){
		return LM048_COMPLETED;
	}

	struct lm048_packet *cmd_echo = &(queue->array[queue->front][0]);
	struct lm048_packet *expected = &(queue->array[queue->front][1]);

	if(cmd_echo->type == received->type){
		return LM048_OK;
	}

	if(queue->length <= ++queue->front){
		queue->front = 0;
	}

	if(expected->type != received->type){
		return LM048_UNEXPECTED;
	}

	if(expected->payload_length != received->payload_length){
		return LM048_UNEXPECTED;
	}

	if(expected->payload_length > 0 &&
	   strncmp(expected->payload,
		   received->payload,
		   expected->payload_length) != 0)
	{
		return LM048_UNEXPECTED;
	}

	return LM048_DEQUEUED;
}

enum LM048_STATUS lm048_queue_front(struct lm048_queue const *const queue,
				    struct lm048_packet const **cmd,
				    struct lm048_packet const **resp)
{
	struct lm048_queue const *que = &default_queue;
	if(queue != NULL){
		que = queue;
	}

	enum LM048_STATUS ret;

	if(que->front == que->back){
		*cmd = NULL;
		*resp = NULL;
		ret = LM048_EMPTY;
	}else{
		*cmd = &(que->array[que->front][0]);
		*resp = &(que->array[que->front][1]);
		ret = LM048_COMPLETED;
	}

	return ret;
}

struct lm048_queue
lm048_queue_init(struct lm048_packet (*const array)[2],
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

enum LM048_STATUS
lm048_write_packet(struct lm048_packet const *const packet,
		   char *const buffer,
		   size_t *const length)
{
	char const *ret = NULL;
	switch(packet->type){
	case LM048_AT_NONE:
		ret = "";
	case LM048_AT_OK:
		ret = CRLF "OK" CRLF;
		break;
	case LM048_AT_ERROR:
		ret = CRLF "ERROR" CRLF;
		break;
	case LM048_AT_AT:
		ret = "AT" CR;
		break;
	case LM048_AT_VER:
		ret = ATP "VER" CR;
		break;
	default:
		*length = 0;
		return LM048_ERROR;
	}

	if(strlen(ret) > *length){
		*length = strlen(ret);
		return LM048_FULL;
	}

	strcpy(buffer, ret);
	*length = strlen(ret);

	return LM048_COMPLETED;
}

enum LM048_STATUS
lm048_write_front_command(struct lm048_queue const *const queue,
			 char *const buffer,
			 size_t *const length)
{
	struct lm048_packet const *cmd, *resp;
	enum LM048_STATUS s = lm048_queue_front(queue, &cmd, &resp);
	if(s != LM048_COMPLETED){
		*length = 0;
		return s;
	}

	return lm048_write_packet(cmd, buffer, length);
}


enum LM048_STATUS lm048_inputs(struct lm048_parser *const state,
			       char const *const data,
			       size_t *const length)
{
	struct lm048_packet *const pkt = &state->current;

	if(*length > 0 && data != NULL){
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

	lm048_packet_init(&state->current,
			  default_payload,
			  LM048_DEFAULT_PAYLOAD_LENGTH);
}

enum LM048_STATUS
lm048_packet_payload(struct lm048_packet const *const packet,
		     char *const buffer,
		     size_t const length)
{
	struct lm048_packet const *pkt =
		&(lm048_default_state.current);

	if(packet != NULL){
		pkt = packet;
	}

	if(length <= pkt->payload_capacity){
		return LM048_FULL;
	}

	if(pkt->payload_length < 1){
		return LM048_OK;
	}

	memcpy(buffer, pkt->payload, pkt->payload_length);
	buffer[pkt->payload_length] = '\0';

	return LM048_COMPLETED;
}
