
#line 1 "build/lm048lib.rl"
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


#line 18 "build/lm048lib.c"
static const int atcmd_start = 1;
static const int atcmd_first_final = 19;
static const int atcmd_error = 0;

static const int atcmd_en_main = 1;


#line 68 "build/lm048lib.rl"


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
	.cs = 1,
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

	struct lm048_packet *cmd_echo = &(queue->array[queue->front][0]);
	struct lm048_packet *expected = &(queue->array[queue->front][1]);

	if(cmd_echo->type == received.type){
		return LM048_OK;
	}

	if(queue->length <= ++queue->front){
		queue->front = 0;
	}

	if(expected->type != received.type){
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
lm048_write_packet(char *const buffer,
		   size_t *const length,
		   struct lm048_packet const *const packet)
{
	char const *ret = NULL;
	switch(packet->type){
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

	return lm048_write_packet(buffer, length, cmd);
}


enum LM048_STATUS lm048_inputs(struct lm048_parser *const state,
			       char const *const data,
			       size_t *const length)
{
	struct lm048_packet pkt = {
		.type = LM048_AT_NONE
	};

	if(*length > 0 && data != NULL){
		char const *p = data;
		char const *pe = p + *length;
		char const *eof = NULL;

		
#line 200 "build/lm048lib.c"
	{
	}

#line 241 "build/lm048lib.rl"
		
#line 206 "build/lm048lib.c"
	{
	int _ps = 0;
	if ( p == pe )
		goto _test_eof;
	switch (  state->cs )
	{
case 1:
	_ps = 1;
	switch( (*p) ) {
		case 13: goto st2;
		case 65: goto st13;
		case 97: goto st13;
	}
	goto tr0;
tr0:
#line 29 "build/lm048lib.rl"
	{
		state->on_error((_ps), (*p));
	}
	goto st0;
#line 227 "build/lm048lib.c"
st0:
 state->cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	_ps = 2;
	if ( (*p) == 10 )
		goto st3;
	goto tr0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	_ps = 3;
	switch( (*p) ) {
		case 69: goto st4;
		case 79: goto st10;
	}
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
	if ( (*p) == 82 )
		goto st6;
	goto tr0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	_ps = 6;
	if ( (*p) == 79 )
		goto st7;
	goto tr0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	_ps = 7;
	if ( (*p) == 82 )
		goto st8;
	goto tr0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	_ps = 8;
	if ( (*p) == 13 )
		goto st9;
	goto tr0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	_ps = 9;
	if ( (*p) == 10 )
		goto tr11;
	goto tr0;
tr11:
#line 24 "build/lm048lib.rl"
	{
		pkt.type = LM048_AT_ERROR;
		state->on_error_response();
	}
	goto st19;
tr14:
#line 19 "build/lm048lib.rl"
	{
		pkt.type = LM048_AT_OK;
		state->on_ok_response();
	}
	goto st19;
tr16:
#line 41 "build/lm048lib.rl"
	{
		pkt.type = LM048_AT_AT;
	}
	goto st19;
tr21:
#line 45 "build/lm048lib.rl"
	{
		pkt.type = LM048_AT_VER;
	}
	goto st19;
st19:
#line 33 "build/lm048lib.rl"
	{
		if(state->on_completed != NULL){
			state->on_completed();
		}else{
			{p++;  state->cs = 19; goto _out;}
		}
	}
	if ( ++p == pe )
		goto _test_eof19;
case 19:
#line 335 "build/lm048lib.c"
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	_ps = 10;
	if ( (*p) == 75 )
		goto st11;
	goto tr0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	_ps = 11;
	if ( (*p) == 13 )
		goto st12;
	goto tr0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	_ps = 12;
	if ( (*p) == 10 )
		goto tr14;
	goto tr0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	_ps = 13;
	switch( (*p) ) {
		case 84: goto st14;
		case 116: goto st14;
	}
	goto tr0;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	_ps = 14;
	switch( (*p) ) {
		case 13: goto tr16;
		case 43: goto st15;
	}
	goto tr0;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	_ps = 15;
	switch( (*p) ) {
		case 86: goto st16;
		case 118: goto st16;
	}
	goto tr0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	_ps = 16;
	switch( (*p) ) {
		case 69: goto st17;
		case 101: goto st17;
	}
	goto tr0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	_ps = 17;
	switch( (*p) ) {
		case 82: goto st18;
		case 114: goto st18;
	}
	goto tr0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	_ps = 18;
	if ( (*p) == 13 )
		goto tr21;
	goto tr0;
	}
	_test_eof2:  state->cs = 2; goto _test_eof; 
	_test_eof3:  state->cs = 3; goto _test_eof; 
	_test_eof4:  state->cs = 4; goto _test_eof; 
	_test_eof5:  state->cs = 5; goto _test_eof; 
	_test_eof6:  state->cs = 6; goto _test_eof; 
	_test_eof7:  state->cs = 7; goto _test_eof; 
	_test_eof8:  state->cs = 8; goto _test_eof; 
	_test_eof9:  state->cs = 9; goto _test_eof; 
	_test_eof19:  state->cs = 19; goto _test_eof; 
	_test_eof10:  state->cs = 10; goto _test_eof; 
	_test_eof11:  state->cs = 11; goto _test_eof; 
	_test_eof12:  state->cs = 12; goto _test_eof; 
	_test_eof13:  state->cs = 13; goto _test_eof; 
	_test_eof14:  state->cs = 14; goto _test_eof; 
	_test_eof15:  state->cs = 15; goto _test_eof; 
	_test_eof16:  state->cs = 16; goto _test_eof; 
	_test_eof17:  state->cs = 17; goto _test_eof; 
	_test_eof18:  state->cs = 18; goto _test_eof; 

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
#line 29 "build/lm048lib.rl"
	{
		state->on_error((_ps), (*p));
	}
	break;
#line 466 "build/lm048lib.c"
	}
	}

	_out: {}
	}

#line 242 "build/lm048lib.rl"

		*length = *length - (size_t)(p - data);
	}

	if(state->cs >= 19){
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
}

