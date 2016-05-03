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
#include <stdio.h>

#define CR LM048_COMMAND_DELIMETER
#define CRLF LM048_RESPONSE_DELIMETER
#define ATP "AT+"

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

LM048_API enum LM048_STATUS lm048_skip_line(char *const data,
					    size_t *const length)
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

	action on_ok_resp {
		pkt->type = LM048_AT_OK;
		state->on_ok_response();
	}

	action on_error_resp {
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

	action on_ver_resp {
		pkt->type = LM048_AT_VER_RESPONSE;
	}

	action on_value_resp {
		pkt->type = LM048_AT_VALUE_RESPONSE;
	}

	action on_pin {
		pkt->type = LM048_AT_PIN;
	}

	action on_baud_cmd {
		pkt->type = LM048_AT_BAUD;
	}

	action on_baud_resp {
		pkt->type = LM048_AT_BAUD_RESPONSE;
	}

	action on_reset {
		pkt->type = LM048_AT_RESET;
	}

	action on_enable_mod {
		pkt->modifier = LM048_ATM_ENABLE;
	}

	action on_disable_mod {
		pkt->modifier = LM048_ATM_DISABLE;
	}

	action on_get_mod {
		pkt->modifier = LM048_ATM_GET;
	}

	action on_set_mod {
		pkt->modifier = LM048_ATM_SET;
	}

	action on_set_enum_mod {
		pkt->modifier = LM048_ATM_SET_ENUM;
	}

	action on_connect_evnt {
		pkt->type = LM048_AT_CONNECT_EVNT;
	}

	action on_disconnect_evnt {
		pkt->type = LM048_AT_DISCONNECT_EVNT;
	}

	action add_to_payload {
		payload_add(pkt, *p);
	}

	action clear_payload {
		pkt->payload_length = 0;
	}

	action resp_context {
		if(resp != NULL && resp->type == LM048_AT_VALUE_RESPONSE){
			pkt->payload_length = 0;
			fgoto value_resp;
		}
	}

	cr = '\r';
	lf = '\n';
	crlf = cr lf;
	at = [aA][tT];
	ok = 'OK' crlf; 
	error = 'ERROR' crlf;

	on = '+' @on_enable_mod;
	off = '-' @on_disable_mod;
	get = '?' @on_get_mod;
	set = '=' @on_set_mod;

	command_resp = (ok @on_ok_resp | error @on_error_resp);
	value_resp := ((any - cr)+ $add_to_payload crlf ok @on_value_resp) 
			     @!on_error %~on_completed;
	
	ver = [vV][eE][rR] @on_ver;
	ver_resp = 'F/W VERSION:' ' '{1,5} 'v' @clear_payload .
		   (digit{1,2} '.' digit{2,3}) $add_to_payload .
		   ' '{,5} crlf ok @on_ver_resp;

	pin = [pP][iI][nN] .
		(on | off | get | 
		 set @clear_payload .
		 (alnum | punct | [\t\v\f ])+ @add_to_payload) @on_pin;

	baud = [bB][aA][uU][dD] @clear_payload .
		(get | (digit{2} $add_to_payload %on_set_enum_mod));
	baud_cmd = baud @on_baud_cmd;
	baud_resp = baud crlf ok @on_baud_resp;

	reset = [rR][eE][sS][eE][tT] @on_reset;

	bluetooth_addr = 
		(alnum{4} '-' alnum{2} '-' alnum{6}) $add_to_payload; 
	connect_evnt = 'CONNECT' ' '{0,2} '"' @clear_payload
		       bluetooth_addr  '"' crlf @on_connect_evnt;
	disconnect_evnt = 'DISCONNECT' ' '{0,2} '"' @clear_payload
			bluetooth_addr '"' crlf @on_disconnect_evnt;

	responses = lf @resp_context (command_resp | 
				      ver_resp | 
				      baud_resp |
				      connect_evnt |
				      disconnect_evnt);
	commands = at (cr @on_at_at | 
		       ('+' (ver | 
			     pin | 
			     baud_cmd |
			     reset) 
			cr));

	main := (cr* (commands | responses)) %~on_completed
		      @!on_error;
	
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
LM048_API struct lm048_parser lm048_default_state = {
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

LM048_API enum LM048_STATUS lm048_packet_init(struct lm048_packet *const pkt,
				char *const payload,
				size_t payload_capacity)
{
	pkt->type = LM048_AT_NONE;
	pkt->modifier = LM048_ATM_NONE;
	pkt->payload = payload;
	pkt->payload_length = 0;
	pkt->payload_capacity = payload_capacity;

	return LM048_COMPLETED;
}

LM048_API enum LM048_STATUS lm048_enqueue(struct lm048_queue *const queue,
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

	if(lm048_packet_has_modifier(expected)
	   && expected->modifier != received->modifier){
		return LM048_UNEXPECTED;
	}

	if(lm048_packet_has_payload(expected)
	   && (expected->payload_length != received->payload_length
	       || strncmp(expected->payload,
			  received->payload,
			  expected->payload_length)))
	{
		return LM048_UNEXPECTED;
	}

	return LM048_DEQUEUED;
}

LM048_API enum LM048_STATUS
lm048_queue_front(struct lm048_queue const *const queue,
		  struct lm048_packet const **cmd,
		  struct lm048_packet const **resp)
{
	struct lm048_queue const *que = &default_queue;
	if(queue != NULL){
		que = queue;
	}

	enum LM048_STATUS ret;

	if(que->front == que->back){
		if(cmd != NULL)
			*cmd = NULL;
		if(resp != NULL)
			*resp = NULL;
		ret = LM048_EMPTY;
	}else{
		if(cmd != NULL)
			*cmd = &(que->array[que->front][0]);
		if(resp != NULL)
			*resp = &(que->array[que->front][1]);
		ret = LM048_COMPLETED;
	}

	return ret;
}

LM048_API struct lm048_queue
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

LM048_API int lm048_packet_has_modifier(struct lm048_packet const *const pkt)
{
	switch(pkt->type){
	case LM048_AT_NONE:
	case LM048_AT_OK:
	case LM048_AT_ERROR:
	case LM048_AT_AT:
	case LM048_AT_VALUE_RESPONSE:
	case LM048_AT_VER:
	case LM048_AT_VER_RESPONSE:
	case LM048_AT_BAUD_RESPONSE:
	case LM048_AT_RESET:
	case LM048_AT_ENQ:
	case LM048_AT_CONNECT_EVNT:
	case LM048_AT_DISCONNECT_EVNT:
		return 0;
	case LM048_AT_PIN:
	case LM048_AT_BAUD:
		return 1;
	default:
		return -1;
	}
}

LM048_API int lm048_packet_has_payload(struct lm048_packet const *const pkt)
{
	switch(pkt->type){

	//-Never has payload
	case LM048_AT_NONE:
	case LM048_AT_OK:
	case LM048_AT_ERROR:
	case LM048_AT_AT:
	case LM048_AT_VER:
	case LM048_AT_RESET:
	case LM048_AT_ENQ:
		return 0;

	//-Always has payload
	case LM048_AT_VALUE_RESPONSE:
	case LM048_AT_VER_RESPONSE:
	case LM048_AT_BAUD_RESPONSE:
	case LM048_AT_CONNECT_EVNT:
	case LM048_AT_DISCONNECT_EVNT:
		return 1;

	//-Depends on modifier
	case LM048_AT_PIN:
	case LM048_AT_BAUD:
		break;
	default:
		return -1;
	}

	switch(pkt->modifier){
	case LM048_ATM_ENABLE:
	case LM048_ATM_DISABLE:
	case LM048_ATM_GET:
		return 0;
	case LM048_ATM_SET:
	case LM048_ATM_SET_ENUM:
		return 1;
	case LM048_ATM_NONE:
		break;
	}
	return -1;
}

LM048_API enum LM048_STATUS
lm048_write_packet(struct lm048_packet const *const packet,
		   char *const buf,
		   size_t *const length)
{
	size_t len = 0;
	char const *cmd;
	char const *mod;
	char const *end = CR;

	switch(packet->type){
	case LM048_AT_NONE:
		return LM048_OK;
	case LM048_AT_OK:
		cmd = CRLF "OK";
		end = CRLF;
		break;
	case LM048_AT_ERROR:
		cmd = CRLF "ERROR";
		end = CRLF;
		break;
	case LM048_AT_AT:
		cmd = "AT";
		break;
	case LM048_AT_VALUE_RESPONSE:
		cmd = CRLF;
		end = CRLF "OK" CRLF;
		break;
	case LM048_AT_VER:
		cmd = ATP "VER";
		break;
	case LM048_AT_VER_RESPONSE:
		cmd = CRLF "F/W VERSION:";
		end = CRLF "OK" CRLF;
		break;
	case LM048_AT_PIN:
		cmd = ATP "PIN";
		break;
	case LM048_AT_BAUD:
		cmd = ATP "BAUD";
	case LM048_AT_BAUD_RESPONSE:
		cmd = CRLF "BAUD";
		end = CRLF "OK" CRLF;
	case LM048_AT_RESET:
		cmd = ATP "RESET";
	case LM048_AT_ENQ:
		cmd = ATP "ENQ";
	default:
		*length = 0;
		return LM048_ERROR;
	}

	switch(packet->modifier){
	case LM048_ATM_ENABLE:
		mod = "+";
		break;
	case LM048_ATM_DISABLE:
		mod = "-";
		break;
	case LM048_ATM_GET:
		mod = "?";
		break;
	case LM048_ATM_SET:
		mod = "=";
		break;
	case LM048_ATM_SET_ENUM:
	case LM048_ATM_NONE:
		break;
	}

	len = strlen(cmd) + strlen(end);
	if(lm048_packet_has_modifier(packet)
	   && packet->modifier != LM048_ATM_SET_ENUM)
		len += strlen(mod);
	if(lm048_packet_has_payload(packet))
		len += packet->payload_length;

	if(len >= *length){
		*length = len;
		return LM048_FULL;
	}

	*buf = '\0';
	strcat(buf, cmd);
	if(lm048_packet_has_modifier(packet)
	   && packet->modifier != LM048_ATM_SET_ENUM)
		strcat(buf, mod);
	if(lm048_packet_has_payload(packet))
		strncat(buf, packet->payload, packet->payload_length);
	strcat(buf, end);

	*length = len;
	return LM048_COMPLETED;
}

LM048_API enum LM048_STATUS
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


LM048_API enum LM048_STATUS lm048_inputs(struct lm048_parser *const state,
				         char const *const data,
				         size_t *const length)
{
	struct lm048_packet const *resp;
	struct lm048_packet *const pkt = &state->current;

	lm048_queue_front(state->queue, NULL, &resp);

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

LM048_API enum LM048_STATUS
lm048_input(char const *const data, size_t *const length)
{
	enum LM048_STATUS s =
		lm048_inputs(&lm048_default_state, data, length);
	return s;
}

LM048_API void lm048_restart(struct lm048_parser *state)
{
	if(state == NULL){
		state = &lm048_default_state;
	}

	state->cs = %%{ write start; }%%;
}

LM048_API void lm048_init(struct lm048_parser *state)
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

LM048_API enum LM048_STATUS
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
