
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

#define CR LM048_COMMAND_DELIMETER
#define CRLF LM048_RESPONSE_DELIMETER
#define ATP "at+"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif


#line 27 "build/lm048lib.c"
static const int skipline_start = 1;
static const int skipline_first_final = 2;
static const int skipline_error = 0;

static const int skipline_en_main = 1;


#line 29 "build/lm048lib.rl"


enum LM048_STATUS lm048_skip_line(char *const data, size_t *const length)
{
	if(*length > 0){
		int cs;
		char *p = data;
		char *pe = p + *length;

		
#line 46 "build/lm048lib.c"
	{
	cs = skipline_start;
	}

#line 39 "build/lm048lib.rl"
		
#line 53 "build/lm048lib.c"
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
#line 26 "build/lm048lib.rl"
	{ {p++; cs = 2; goto _out;} }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 74 "build/lm048lib.c"
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

#line 40 "build/lm048lib.rl"

		*length -= (size_t)(p - data);

		if(cs == 2){
			return LM048_COMPLETED;
		}
	}

	return LM048_OK;
}


#line 100 "build/lm048lib.c"
static const int atcmd_start = 1;
static const int atcmd_first_final = 51;
static const int atcmd_error = 0;

static const int atcmd_en_main = 1;


#line 124 "build/lm048lib.rl"


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
		ret = ATP "VER?" CR;
		break;
	}

	if(ret == NULL){
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

		
#line 313 "build/lm048lib.c"
	{
	}

#line 328 "build/lm048lib.rl"
		
#line 319 "build/lm048lib.c"
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
		case 10: goto st2;
		case 13: goto st1;
		case 65: goto st45;
		case 97: goto st45;
	}
	goto tr0;
tr0:
#line 66 "build/lm048lib.rl"
	{
		state->on_error((_ps), (*p));
	}
	goto st0;
#line 344 "build/lm048lib.c"
st0:
 state->cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	_ps = 2;
	switch( (*p) ) {
		case 69: goto st3;
		case 70: goto st9;
		case 79: goto st42;
	}
	goto tr0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	_ps = 3;
	if ( (*p) == 82 )
		goto st4;
	goto tr0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	_ps = 4;
	if ( (*p) == 82 )
		goto st5;
	goto tr0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	_ps = 5;
	if ( (*p) == 79 )
		goto st6;
	goto tr0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	_ps = 6;
	if ( (*p) == 82 )
		goto st7;
	goto tr0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	_ps = 7;
	if ( (*p) == 13 )
		goto st8;
	goto tr0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	_ps = 8;
	if ( (*p) == 10 )
		goto tr12;
	goto tr0;
tr12:
#line 61 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_ERROR;
		state->on_error_response();
	}
	goto st51;
tr41:
#line 86 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_VER_RESPONSE;
	}
	goto st51;
tr48:
#line 56 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_OK;
		state->on_ok_response();
	}
	goto st51;
tr50:
#line 78 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_AT;
	}
	goto st51;
tr55:
#line 82 "build/lm048lib.rl"
	{
		pkt->type = LM048_AT_VER;
	}
	goto st51;
st51:
#line 70 "build/lm048lib.rl"
	{
		if(state->on_completed != NULL){
			state->on_completed();
		}else{
			{p++;  state->cs = 51; goto _out;}
		}
	}
	if ( ++p == pe )
		goto _test_eof51;
case 51:
#line 451 "build/lm048lib.c"
	goto st0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	_ps = 9;
	if ( (*p) == 87 )
		goto st10;
	goto tr0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	_ps = 10;
	if ( (*p) == 32 )
		goto st11;
	goto tr0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	_ps = 11;
	if ( (*p) == 86 )
		goto st12;
	goto tr0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	_ps = 12;
	if ( (*p) == 69 )
		goto st13;
	goto tr0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	_ps = 13;
	if ( (*p) == 82 )
		goto st14;
	goto tr0;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	_ps = 14;
	if ( (*p) == 83 )
		goto st15;
	goto tr0;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	_ps = 15;
	if ( (*p) == 73 )
		goto st16;
	goto tr0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	_ps = 16;
	if ( (*p) == 79 )
		goto st17;
	goto tr0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	_ps = 17;
	if ( (*p) == 78 )
		goto st18;
	goto tr0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	_ps = 18;
	if ( (*p) == 58 )
		goto st19;
	goto tr0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	_ps = 19;
	if ( (*p) == 32 )
		goto st20;
	goto tr0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	_ps = 20;
	switch( (*p) ) {
		case 32: goto st21;
		case 118: goto tr25;
	}
	goto tr0;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	_ps = 21;
	switch( (*p) ) {
		case 32: goto st22;
		case 118: goto tr25;
	}
	goto tr0;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	_ps = 22;
	switch( (*p) ) {
		case 32: goto st23;
		case 118: goto tr25;
	}
	goto tr0;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	_ps = 23;
	switch( (*p) ) {
		case 32: goto st24;
		case 118: goto tr25;
	}
	goto tr0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	_ps = 24;
	if ( (*p) == 118 )
		goto tr25;
	goto tr0;
tr25:
#line 94 "build/lm048lib.rl"
	{
		pkt->payload_length = 0;
	}
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
#line 599 "build/lm048lib.c"
	_ps = 25;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr29;
	goto tr0;
tr29:
#line 90 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st26;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
#line 614 "build/lm048lib.c"
	_ps = 26;
	if ( (*p) == 46 )
		goto tr30;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr31;
	goto tr0;
tr30:
#line 90 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 631 "build/lm048lib.c"
	_ps = 27;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr32;
	goto tr0;
tr32:
#line 90 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
#line 646 "build/lm048lib.c"
	_ps = 28;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr33;
	goto tr0;
tr33:
#line 90 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 661 "build/lm048lib.c"
	_ps = 29;
	switch( (*p) ) {
		case 13: goto st30;
		case 32: goto st35;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr36;
	goto tr0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	_ps = 30;
	if ( (*p) == 10 )
		goto st31;
	goto tr0;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	_ps = 31;
	if ( (*p) == 79 )
		goto st32;
	goto tr0;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	_ps = 32;
	if ( (*p) == 75 )
		goto st33;
	goto tr0;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	_ps = 33;
	if ( (*p) == 13 )
		goto st34;
	goto tr0;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	_ps = 34;
	if ( (*p) == 10 )
		goto tr41;
	goto tr0;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
	_ps = 35;
	switch( (*p) ) {
		case 13: goto st30;
		case 32: goto st36;
	}
	goto tr0;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
	_ps = 36;
	switch( (*p) ) {
		case 13: goto st30;
		case 32: goto st37;
	}
	goto tr0;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	_ps = 37;
	switch( (*p) ) {
		case 13: goto st30;
		case 32: goto st38;
	}
	goto tr0;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
	_ps = 38;
	switch( (*p) ) {
		case 13: goto st30;
		case 32: goto st39;
	}
	goto tr0;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	_ps = 39;
	if ( (*p) == 13 )
		goto st30;
	goto tr0;
tr36:
#line 90 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st40;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
#line 768 "build/lm048lib.c"
	_ps = 40;
	switch( (*p) ) {
		case 13: goto st30;
		case 32: goto st35;
	}
	goto tr0;
tr31:
#line 90 "build/lm048lib.rl"
	{
		payload_add(pkt, *p);
	}
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 785 "build/lm048lib.c"
	_ps = 41;
	if ( (*p) == 46 )
		goto tr30;
	goto tr0;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
	_ps = 42;
	if ( (*p) == 75 )
		goto st43;
	goto tr0;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
	_ps = 43;
	if ( (*p) == 13 )
		goto st44;
	goto tr0;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
	_ps = 44;
	if ( (*p) == 10 )
		goto tr48;
	goto tr0;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	_ps = 45;
	switch( (*p) ) {
		case 84: goto st46;
		case 116: goto st46;
	}
	goto tr0;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
	_ps = 46;
	switch( (*p) ) {
		case 13: goto tr50;
		case 43: goto st47;
	}
	goto tr0;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	_ps = 47;
	switch( (*p) ) {
		case 86: goto st48;
		case 118: goto st48;
	}
	goto tr0;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	_ps = 48;
	switch( (*p) ) {
		case 69: goto st49;
		case 101: goto st49;
	}
	goto tr0;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	_ps = 49;
	switch( (*p) ) {
		case 82: goto st50;
		case 114: goto st50;
	}
	goto tr0;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	_ps = 50;
	if ( (*p) == 13 )
		goto tr55;
	goto tr0;
	}
	_test_eof1:  state->cs = 1; goto _test_eof; 
	_test_eof2:  state->cs = 2; goto _test_eof; 
	_test_eof3:  state->cs = 3; goto _test_eof; 
	_test_eof4:  state->cs = 4; goto _test_eof; 
	_test_eof5:  state->cs = 5; goto _test_eof; 
	_test_eof6:  state->cs = 6; goto _test_eof; 
	_test_eof7:  state->cs = 7; goto _test_eof; 
	_test_eof8:  state->cs = 8; goto _test_eof; 
	_test_eof51:  state->cs = 51; goto _test_eof; 
	_test_eof9:  state->cs = 9; goto _test_eof; 
	_test_eof10:  state->cs = 10; goto _test_eof; 
	_test_eof11:  state->cs = 11; goto _test_eof; 
	_test_eof12:  state->cs = 12; goto _test_eof; 
	_test_eof13:  state->cs = 13; goto _test_eof; 
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
#line 66 "build/lm048lib.rl"
	{
		state->on_error((_ps), (*p));
	}
	break;
#line 984 "build/lm048lib.c"
	}
	}

	_out: {}
	}

#line 329 "build/lm048lib.rl"

		*length = *length - (size_t)(p - data);
	}

	if(state->cs >= 51){
		state->cs = 1;
		return dequeue(state->queue, pkt);
	}else if(state->cs == 0){
		state->cs = 1;
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

	state->cs = 1;
}

void lm048_init(struct lm048_parser *state)
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
