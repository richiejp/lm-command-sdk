
#line 1 "build/lm048lib.rl"
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


#line 28 "build/lm048lib.c"
static const int skipline_start = 1;
static const int skipline_first_final = 2;
static const int skipline_error = 0;

static const int skipline_en_main = 1;


#line 30 "build/lm048lib.rl"


LM048_API enum LM048_STATUS lm048_skip_line(char *const data,
					    size_t *const length)
{
	if(*length > 0){
		int cs;
		char *p = data;
		char *pe = p + *length;

		
#line 48 "build/lm048lib.c"
	{
	cs = skipline_start;
	}

#line 41 "build/lm048lib.rl"
		
#line 55 "build/lm048lib.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	if ( (*p) == 13 )
		goto tr1;
	goto st1;
tr1:
#line 27 "build/lm048lib.rl"
	{ {p++; cs = 2; goto _out;} }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 76 "build/lm048lib.c"
	goto st0;
st0:
cs = 0;
	goto _out;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 42 "build/lm048lib.rl"

		*length -= (size_t)(p - data);

		if(cs == 2){
			return LM048_COMPLETED;
		}
	}

	return LM048_OK;
}


#line 102 "build/lm048lib.c"
static const int atcmd_start = 1;
static const int atcmd_first_final = 86;
static const int atcmd_error = 0;

static const int atcmd_en_value_resp = 79;
static const int atcmd_en_main = 1;


#line 192 "build/lm048lib.rl"


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
	.cs = 1,
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
	pkt->modifier = LM048_ATM_GET;
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

		
#line 458 "build/lm048lib.c"
	{
	}

#line 538 "build/lm048lib.rl"
		
#line 464 "build/lm048lib.c"
	{
	int _ps = 0;
	if ( p == pe )
		goto _test_eof;
	switch (  state->cs )
	{
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	_ps = 1;
	switch( (*p) ) {
		case 10: goto tr1;
		case 13: goto st1;
		case 65: goto st58;
		case 97: goto st58;
	}
	goto tr0;
tr0:
#line 68 "build/lm048lib.rl"
	{
		state->on_error((_ps), (*p));
	}
	goto st0;
#line 489 "build/lm048lib.c"
st0:
 state->cs = 0;
	goto _out;
tr1:
#line 140 "build/lm048lib.rl"
	{
		if(resp != NULL && resp->type == LM048_AT_VALUE_RESPONSE){
			pkt->payload_length = 0;
			{goto st79;}
		}
	}
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 506 "build/lm048lib.c"
	_ps = 2;
	switch( (*p) ) {
		case 66: goto st3;
		case 69: goto st15;
		case 70: goto st21;
		case 79: goto st55;
		case 98: goto st3;
	}
	goto tr0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	_ps = 3;
	switch( (*p) ) {
		case 65: goto st4;
		case 97: goto st4;
	}
	goto tr0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	_ps = 4;
	switch( (*p) ) {
		case 85: goto st5;
		case 117: goto st5;
	}
	goto tr0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	_ps = 5;
	switch( (*p) ) {
		case 68: goto tr10;
		case 100: goto tr10;
	}
	goto tr0;
tr10:
#line 136 "build/lm048lib.rl"
	{
		pkt->payload_length = 0;
	}
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 556 "build/lm048lib.c"
	_ps = 6;
	if ( (*p) == 63 )
		goto tr12;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr11;
	goto tr0;
tr11:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
#line 573 "build/lm048lib.c"
	_ps = 7;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr13;
	goto tr0;
tr13:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 588 "build/lm048lib.c"
	_ps = 8;
	if ( (*p) == 13 )
		goto tr14;
	goto tr0;
tr14:
#line 128 "build/lm048lib.rl"
	{
		pkt->modifier = LM048_ATM_SET_ENUM;
	}
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 603 "build/lm048lib.c"
	_ps = 9;
	if ( (*p) == 10 )
		goto st10;
	goto tr0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	_ps = 10;
	if ( (*p) == 79 )
		goto st11;
	goto tr0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	_ps = 11;
	if ( (*p) == 75 )
		goto st12;
	goto tr0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	_ps = 12;
	if ( (*p) == 13 )
		goto st13;
	goto tr0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	_ps = 13;
	if ( (*p) == 10 )
		goto tr19;
	goto tr0;
tr77:
#line 128 "build/lm048lib.rl"
	{
		pkt->modifier = LM048_ATM_SET_ENUM;
	}
	goto st86;
tr19:
#line 104 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_BAUD_RESPONSE;
	}
	goto st86;
tr26:
#line 63 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_ERROR;
		state->on_error_response();
	}
	goto st86;
tr56:
#line 88 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_VER_RESPONSE;
	}
	goto st86;
tr63:
#line 58 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_OK;
		state->on_ok_response();
	}
	goto st86;
tr65:
#line 80 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_AT;
	}
	goto st86;
st86:
#line 72 "build/lm048lib.rl"
	{
		if(state->on_completed != NULL){
			state->on_completed();
		}else{
			{p++;  state->cs = 86; goto _out;}
		}
	}
	if ( ++p == pe )
		goto _test_eof86;
case 86:
#line 690 "build/lm048lib.c"
	goto st0;
tr12:
#line 120 "build/lm048lib.rl"
	{
		pkt->modifier = LM048_ATM_GET;
	}
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 702 "build/lm048lib.c"
	_ps = 14;
	if ( (*p) == 13 )
		goto st9;
	goto tr0;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	_ps = 15;
	if ( (*p) == 82 )
		goto st16;
	goto tr0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	_ps = 16;
	if ( (*p) == 82 )
		goto st17;
	goto tr0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	_ps = 17;
	if ( (*p) == 79 )
		goto st18;
	goto tr0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	_ps = 18;
	if ( (*p) == 82 )
		goto st19;
	goto tr0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	_ps = 19;
	if ( (*p) == 13 )
		goto st20;
	goto tr0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	_ps = 20;
	if ( (*p) == 10 )
		goto tr26;
	goto tr0;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	_ps = 21;
	if ( (*p) == 47 )
		goto st22;
	goto tr0;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	_ps = 22;
	if ( (*p) == 87 )
		goto st23;
	goto tr0;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	_ps = 23;
	if ( (*p) == 32 )
		goto st24;
	goto tr0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	_ps = 24;
	if ( (*p) == 86 )
		goto st25;
	goto tr0;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	_ps = 25;
	if ( (*p) == 69 )
		goto st26;
	goto tr0;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	_ps = 26;
	if ( (*p) == 82 )
		goto st27;
	goto tr0;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	_ps = 27;
	if ( (*p) == 83 )
		goto st28;
	goto tr0;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	_ps = 28;
	if ( (*p) == 73 )
		goto st29;
	goto tr0;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
	_ps = 29;
	if ( (*p) == 79 )
		goto st30;
	goto tr0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	_ps = 30;
	if ( (*p) == 78 )
		goto st31;
	goto tr0;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	_ps = 31;
	if ( (*p) == 58 )
		goto st32;
	goto tr0;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	_ps = 32;
	if ( (*p) == 32 )
		goto st33;
	goto tr0;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	_ps = 33;
	switch( (*p) ) {
		case 32: goto st34;
		case 118: goto tr40;
	}
	goto tr0;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	_ps = 34;
	switch( (*p) ) {
		case 32: goto st35;
		case 118: goto tr40;
	}
	goto tr0;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
	_ps = 35;
	switch( (*p) ) {
		case 32: goto st36;
		case 118: goto tr40;
	}
	goto tr0;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
	_ps = 36;
	switch( (*p) ) {
		case 32: goto st37;
		case 118: goto tr40;
	}
	goto tr0;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	_ps = 37;
	if ( (*p) == 118 )
		goto tr40;
	goto tr0;
tr40:
#line 136 "build/lm048lib.rl"
	{
		pkt->payload_length = 0;
	}
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
#line 909 "build/lm048lib.c"
	_ps = 38;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr44;
	goto tr0;
tr44:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st39;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
#line 924 "build/lm048lib.c"
	_ps = 39;
	if ( (*p) == 46 )
		goto tr45;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr46;
	goto tr0;
tr45:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st40;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
#line 941 "build/lm048lib.c"
	_ps = 40;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr47;
	goto tr0;
tr47:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 956 "build/lm048lib.c"
	_ps = 41;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr48;
	goto tr0;
tr48:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
#line 971 "build/lm048lib.c"
	_ps = 42;
	switch( (*p) ) {
		case 13: goto st43;
		case 32: goto st48;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr51;
	goto tr0;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
	_ps = 43;
	if ( (*p) == 10 )
		goto st44;
	goto tr0;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
	_ps = 44;
	if ( (*p) == 79 )
		goto st45;
	goto tr0;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	_ps = 45;
	if ( (*p) == 75 )
		goto st46;
	goto tr0;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
	_ps = 46;
	if ( (*p) == 13 )
		goto st47;
	goto tr0;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	_ps = 47;
	if ( (*p) == 10 )
		goto tr56;
	goto tr0;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	_ps = 48;
	switch( (*p) ) {
		case 13: goto st43;
		case 32: goto st49;
	}
	goto tr0;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	_ps = 49;
	switch( (*p) ) {
		case 13: goto st43;
		case 32: goto st50;
	}
	goto tr0;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	_ps = 50;
	switch( (*p) ) {
		case 13: goto st43;
		case 32: goto st51;
	}
	goto tr0;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
	_ps = 51;
	switch( (*p) ) {
		case 13: goto st43;
		case 32: goto st52;
	}
	goto tr0;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	_ps = 52;
	if ( (*p) == 13 )
		goto st43;
	goto tr0;
tr51:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st53;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
#line 1078 "build/lm048lib.c"
	_ps = 53;
	switch( (*p) ) {
		case 13: goto st43;
		case 32: goto st48;
	}
	goto tr0;
tr46:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st54;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
#line 1095 "build/lm048lib.c"
	_ps = 54;
	if ( (*p) == 46 )
		goto tr45;
	goto tr0;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	_ps = 55;
	if ( (*p) == 75 )
		goto st56;
	goto tr0;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	_ps = 56;
	if ( (*p) == 13 )
		goto st57;
	goto tr0;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	_ps = 57;
	if ( (*p) == 10 )
		goto tr63;
	goto tr0;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	_ps = 58;
	switch( (*p) ) {
		case 84: goto st59;
		case 116: goto st59;
	}
	goto tr0;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	_ps = 59;
	switch( (*p) ) {
		case 13: goto tr65;
		case 43: goto st60;
	}
	goto tr0;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	_ps = 60;
	switch( (*p) ) {
		case 66: goto st61;
		case 80: goto st68;
		case 82: goto st73;
		case 86: goto st77;
		case 98: goto st61;
		case 112: goto st68;
		case 114: goto st73;
		case 118: goto st77;
	}
	goto tr0;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	_ps = 61;
	switch( (*p) ) {
		case 65: goto st62;
		case 97: goto st62;
	}
	goto tr0;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	_ps = 62;
	switch( (*p) ) {
		case 85: goto st63;
		case 117: goto st63;
	}
	goto tr0;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	_ps = 63;
	switch( (*p) ) {
		case 68: goto tr73;
		case 100: goto tr73;
	}
	goto tr0;
tr73:
#line 136 "build/lm048lib.rl"
	{
		pkt->payload_length = 0;
	}
	goto st64;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
#line 1200 "build/lm048lib.c"
	_ps = 64;
	if ( (*p) == 63 )
		goto tr75;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr74;
	goto tr0;
tr74:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st65;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
#line 1217 "build/lm048lib.c"
	_ps = 65;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr76;
	goto tr0;
tr76:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
#line 100 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_BAUD;
	}
	goto st66;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
#line 1236 "build/lm048lib.c"
	_ps = 66;
	if ( (*p) == 13 )
		goto tr77;
	goto tr0;
tr75:
#line 120 "build/lm048lib.rl"
	{
		pkt->modifier = LM048_ATM_GET;
	}
#line 100 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_BAUD;
	}
	goto st67;
tr81:
#line 112 "build/lm048lib.rl"
	{
		pkt->modifier = LM048_ATM_ENABLE;
	}
#line 96 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_PIN;
	}
	goto st67;
tr82:
#line 116 "build/lm048lib.rl"
	{
		pkt->modifier = LM048_ATM_DISABLE;
	}
#line 96 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_PIN;
	}
	goto st67;
tr84:
#line 120 "build/lm048lib.rl"
	{
		pkt->modifier = LM048_ATM_GET;
	}
#line 96 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_PIN;
	}
	goto st67;
tr89:
#line 108 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_RESET;
	}
	goto st67;
tr91:
#line 84 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_VER;
	}
	goto st67;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
#line 1297 "build/lm048lib.c"
	_ps = 67;
	if ( (*p) == 13 )
		goto st86;
	goto tr0;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	_ps = 68;
	switch( (*p) ) {
		case 73: goto st69;
		case 105: goto st69;
	}
	goto tr0;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	_ps = 69;
	switch( (*p) ) {
		case 78: goto st70;
		case 110: goto st70;
	}
	goto tr0;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	_ps = 70;
	switch( (*p) ) {
		case 43: goto tr81;
		case 45: goto tr82;
		case 61: goto tr83;
		case 63: goto tr84;
	}
	goto tr0;
tr83:
#line 124 "build/lm048lib.rl"
	{
		pkt->modifier = LM048_ATM_SET;
	}
#line 136 "build/lm048lib.rl"
	{
		pkt->payload_length = 0;
	}
	goto st71;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
#line 1348 "build/lm048lib.c"
	_ps = 71;
	if ( (*p) == 9 )
		goto tr85;
	if ( (*p) > 12 ) {
		if ( 32 <= (*p) && (*p) <= 126 )
			goto tr85;
	} else if ( (*p) >= 11 )
		goto tr85;
	goto tr0;
tr85:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
#line 96 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_PIN;
	}
	goto st72;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
#line 1372 "build/lm048lib.c"
	_ps = 72;
	switch( (*p) ) {
		case 9: goto tr85;
		case 13: goto st86;
	}
	if ( (*p) > 12 ) {
		if ( 32 <= (*p) && (*p) <= 126 )
			goto tr85;
	} else if ( (*p) >= 11 )
		goto tr85;
	goto tr0;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	_ps = 73;
	switch( (*p) ) {
		case 69: goto st74;
		case 101: goto st74;
	}
	goto tr0;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
	_ps = 74;
	switch( (*p) ) {
		case 83: goto st75;
		case 115: goto st75;
	}
	goto tr0;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
	_ps = 75;
	switch( (*p) ) {
		case 69: goto st76;
		case 101: goto st76;
	}
	goto tr0;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
	_ps = 76;
	switch( (*p) ) {
		case 84: goto tr89;
		case 116: goto tr89;
	}
	goto tr0;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
	_ps = 77;
	switch( (*p) ) {
		case 69: goto st78;
		case 101: goto st78;
	}
	goto tr0;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	_ps = 78;
	switch( (*p) ) {
		case 82: goto tr91;
		case 114: goto tr91;
	}
	goto tr0;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	_ps = 79;
	if ( (*p) == 13 )
		goto tr0;
	goto tr92;
tr92:
#line 132 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st80;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
#line 1462 "build/lm048lib.c"
	if ( (*p) == 13 )
		goto st81;
	goto tr92;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	_ps = 81;
	if ( (*p) == 10 )
		goto st82;
	goto tr0;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	_ps = 82;
	if ( (*p) == 79 )
		goto st83;
	goto tr0;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	_ps = 83;
	if ( (*p) == 75 )
		goto st84;
	goto tr0;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	_ps = 84;
	if ( (*p) == 13 )
		goto st85;
	goto tr0;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	_ps = 85;
	if ( (*p) == 10 )
		goto tr98;
	goto tr0;
tr98:
#line 92 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_VALUE_RESPONSE;
	}
	goto st87;
st87:
#line 72 "build/lm048lib.rl"
	{
		if(state->on_completed != NULL){
			state->on_completed();
		}else{
			{p++;  state->cs = 87; goto _out;}
		}
	}
	if ( ++p == pe )
		goto _test_eof87;
case 87:
#line 1524 "build/lm048lib.c"
	goto st0;
	}
	_test_eof1:  state->cs = 1; goto _test_eof; 
	_test_eof2:  state->cs = 2; goto _test_eof; 
	_test_eof3:  state->cs = 3; goto _test_eof; 
	_test_eof4:  state->cs = 4; goto _test_eof; 
	_test_eof5:  state->cs = 5; goto _test_eof; 
	_test_eof6:  state->cs = 6; goto _test_eof; 
	_test_eof7:  state->cs = 7; goto _test_eof; 
	_test_eof8:  state->cs = 8; goto _test_eof; 
	_test_eof9:  state->cs = 9; goto _test_eof; 
	_test_eof10:  state->cs = 10; goto _test_eof; 
	_test_eof11:  state->cs = 11; goto _test_eof; 
	_test_eof12:  state->cs = 12; goto _test_eof; 
	_test_eof13:  state->cs = 13; goto _test_eof; 
	_test_eof86:  state->cs = 86; goto _test_eof; 
	_test_eof14:  state->cs = 14; goto _test_eof; 
	_test_eof15:  state->cs = 15; goto _test_eof; 
	_test_eof16:  state->cs = 16; goto _test_eof; 
	_test_eof17:  state->cs = 17; goto _test_eof; 
	_test_eof18:  state->cs = 18; goto _test_eof; 
	_test_eof19:  state->cs = 19; goto _test_eof; 
	_test_eof20:  state->cs = 20; goto _test_eof; 
	_test_eof21:  state->cs = 21; goto _test_eof; 
	_test_eof22:  state->cs = 22; goto _test_eof; 
	_test_eof23:  state->cs = 23; goto _test_eof; 
	_test_eof24:  state->cs = 24; goto _test_eof; 
	_test_eof25:  state->cs = 25; goto _test_eof; 
	_test_eof26:  state->cs = 26; goto _test_eof; 
	_test_eof27:  state->cs = 27; goto _test_eof; 
	_test_eof28:  state->cs = 28; goto _test_eof; 
	_test_eof29:  state->cs = 29; goto _test_eof; 
	_test_eof30:  state->cs = 30; goto _test_eof; 
	_test_eof31:  state->cs = 31; goto _test_eof; 
	_test_eof32:  state->cs = 32; goto _test_eof; 
	_test_eof33:  state->cs = 33; goto _test_eof; 
	_test_eof34:  state->cs = 34; goto _test_eof; 
	_test_eof35:  state->cs = 35; goto _test_eof; 
	_test_eof36:  state->cs = 36; goto _test_eof; 
	_test_eof37:  state->cs = 37; goto _test_eof; 
	_test_eof38:  state->cs = 38; goto _test_eof; 
	_test_eof39:  state->cs = 39; goto _test_eof; 
	_test_eof40:  state->cs = 40; goto _test_eof; 
	_test_eof41:  state->cs = 41; goto _test_eof; 
	_test_eof42:  state->cs = 42; goto _test_eof; 
	_test_eof43:  state->cs = 43; goto _test_eof; 
	_test_eof44:  state->cs = 44; goto _test_eof; 
	_test_eof45:  state->cs = 45; goto _test_eof; 
	_test_eof46:  state->cs = 46; goto _test_eof; 
	_test_eof47:  state->cs = 47; goto _test_eof; 
	_test_eof48:  state->cs = 48; goto _test_eof; 
	_test_eof49:  state->cs = 49; goto _test_eof; 
	_test_eof50:  state->cs = 50; goto _test_eof; 
	_test_eof51:  state->cs = 51; goto _test_eof; 
	_test_eof52:  state->cs = 52; goto _test_eof; 
	_test_eof53:  state->cs = 53; goto _test_eof; 
	_test_eof54:  state->cs = 54; goto _test_eof; 
	_test_eof55:  state->cs = 55; goto _test_eof; 
	_test_eof56:  state->cs = 56; goto _test_eof; 
	_test_eof57:  state->cs = 57; goto _test_eof; 
	_test_eof58:  state->cs = 58; goto _test_eof; 
	_test_eof59:  state->cs = 59; goto _test_eof; 
	_test_eof60:  state->cs = 60; goto _test_eof; 
	_test_eof61:  state->cs = 61; goto _test_eof; 
	_test_eof62:  state->cs = 62; goto _test_eof; 
	_test_eof63:  state->cs = 63; goto _test_eof; 
	_test_eof64:  state->cs = 64; goto _test_eof; 
	_test_eof65:  state->cs = 65; goto _test_eof; 
	_test_eof66:  state->cs = 66; goto _test_eof; 
	_test_eof67:  state->cs = 67; goto _test_eof; 
	_test_eof68:  state->cs = 68; goto _test_eof; 
	_test_eof69:  state->cs = 69; goto _test_eof; 
	_test_eof70:  state->cs = 70; goto _test_eof; 
	_test_eof71:  state->cs = 71; goto _test_eof; 
	_test_eof72:  state->cs = 72; goto _test_eof; 
	_test_eof73:  state->cs = 73; goto _test_eof; 
	_test_eof74:  state->cs = 74; goto _test_eof; 
	_test_eof75:  state->cs = 75; goto _test_eof; 
	_test_eof76:  state->cs = 76; goto _test_eof; 
	_test_eof77:  state->cs = 77; goto _test_eof; 
	_test_eof78:  state->cs = 78; goto _test_eof; 
	_test_eof79:  state->cs = 79; goto _test_eof; 
	_test_eof80:  state->cs = 80; goto _test_eof; 
	_test_eof81:  state->cs = 81; goto _test_eof; 
	_test_eof82:  state->cs = 82; goto _test_eof; 
	_test_eof83:  state->cs = 83; goto _test_eof; 
	_test_eof84:  state->cs = 84; goto _test_eof; 
	_test_eof85:  state->cs = 85; goto _test_eof; 
	_test_eof87:  state->cs = 87; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch (  state->cs ) {
	case 1: 
	case 2: 
	case 3: 
	case 4: 
	case 5: 
	case 6: 
	case 7: 
	case 8: 
	case 9: 
	case 10: 
	case 11: 
	case 12: 
	case 13: 
	case 14: 
	case 15: 
	case 16: 
	case 17: 
	case 18: 
	case 19: 
	case 20: 
	case 21: 
	case 22: 
	case 23: 
	case 24: 
	case 25: 
	case 26: 
	case 27: 
	case 28: 
	case 29: 
	case 30: 
	case 31: 
	case 32: 
	case 33: 
	case 34: 
	case 35: 
	case 36: 
	case 37: 
	case 38: 
	case 39: 
	case 40: 
	case 41: 
	case 42: 
	case 43: 
	case 44: 
	case 45: 
	case 46: 
	case 47: 
	case 48: 
	case 49: 
	case 50: 
	case 51: 
	case 52: 
	case 53: 
	case 54: 
	case 55: 
	case 56: 
	case 57: 
	case 58: 
	case 59: 
	case 60: 
	case 61: 
	case 62: 
	case 63: 
	case 64: 
	case 65: 
	case 66: 
	case 67: 
	case 68: 
	case 69: 
	case 70: 
	case 71: 
	case 72: 
	case 73: 
	case 74: 
	case 75: 
	case 76: 
	case 77: 
	case 78: 
	case 79: 
	case 80: 
	case 81: 
	case 82: 
	case 83: 
	case 84: 
	case 85: 
#line 68 "build/lm048lib.rl"
	{
		state->on_error((_ps), (*p));
	}
	break;
#line 1709 "build/lm048lib.c"
	}
	}

	_out: {}
	}

#line 539 "build/lm048lib.rl"

		*length = *length - (size_t)(p - data);
	}

	if(state->cs >= 86){
		state->cs = 1;
		return dequeue(state->queue, pkt);
	}else if(state->cs == 0){
		state->cs = 1;
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

	state->cs = 1;
}

LM048_API void lm048_init(struct lm048_parser *state)
{
	state->cs = 1;
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
