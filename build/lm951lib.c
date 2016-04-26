
#line 1 "build/lm951lib.rl"
/** Copyright LM Technologies 2016
 *
 * This file is licensed under the terms of the Adaptive Public License 
 * set forth in licenses.txt in the root directory of the LM Command SDK 
 * repository.
 *
 * Author(s): Richard Palethorpe <richard@lm-technologies.com>
 */

#include "lm951lib.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif


#line 22 "build/lm951lib.c"
static const int atcmd_start = 1;
static const int atcmd_first_final = 19;
static const int atcmd_error = 0;

static const int atcmd_en_main = 1;


#line 61 "build/lm951lib.rl"


void lm951_no_op(){}
void lm951_no_op_e(int cs, char c){}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

struct lm951_parser lm951_default_state = {
	.cs = 1,
	.on_ok_response = lm951_no_op,
	.on_error_response = lm951_no_op,
	.on_error = lm951_no_op_e,
	.on_completed = NULL
};

enum LM951_ISTATUS lm951_inputs(struct lm951_parser *state, 
				   char const *data, 
				   size_t *length)
{
	if(*length > 0){
		char const *p = data;
		char const *pe = p + *length;
		char const *eof = NULL;

		
#line 58 "build/lm951lib.c"
	{
	}

#line 88 "build/lm951lib.rl"
		
#line 64 "build/lm951lib.c"
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
#line 31 "build/lm951lib.rl"
	{
		state->on_error((_ps), (*p));
	}
	goto st0;
#line 85 "build/lm951lib.c"
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
#line 27 "build/lm951lib.rl"
	{
		state->on_error_response();
	}
	goto st19;
tr14:
#line 23 "build/lm951lib.rl"
	{
		state->on_ok_response();
	}
	goto st19;
st19:
#line 35 "build/lm951lib.rl"
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
#line 179 "build/lm951lib.c"
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
		case 13: goto st19;
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
		goto st19;
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
#line 31 "build/lm951lib.rl"
	{
		state->on_error((_ps), (*p));
	}
	break;
#line 310 "build/lm951lib.c"
	}
	}

	_out: {}
	}

#line 89 "build/lm951lib.rl"

		*length = *length - (size_t)(p - data);
	}

	if(state->cs >= 19){
		state->cs = 1;
		return LM951_COMPLETED;
	}else if(state->cs == 0){
		state->cs = 1;
		return LM951_ERROR;
	}else{
		return LM951_OK;
	}
}

enum LM951_ISTATUS lm951_input(char const *data, size_t *length){
	enum LM951_ISTATUS s = 
		lm951_inputs(&lm951_default_state, data, length);
	return s;
}

void lm951_restart(struct lm951_parser *state){
	if(state == NULL){
		state = &lm951_default_state;
	}

	state->cs = 1;
}

void lm951_init(struct lm951_parser *state){
	state->cs = 1;
	state->on_ok_response = lm951_no_op;
	state->on_error_response = lm951_no_op;
	state->on_error = lm951_no_op_e;
	state->on_completed = NULL;
}
