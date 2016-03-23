/** Copyright LM Technologies 2016
 *
 * This file is licensed under the terms of the Adaptive Public License 
 * set forth in licenses.txt in the root directory of the LM Command SDK 
 * repository.
 *
 * Author(s): Richard Palethorpe <richard@lm-technologies.com>
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lm048lib.h"

#define TESTCOUNT 23

static void print_error(int cs, char c);
static void setup();

//Copied from https://github.com/richiejp/roth/blob/master/src/tests.c
//with permission of author :-)
#define TEST(n)	static bool _##n();	\
static bool n(char **name)		\
{					\
	*name = #n;			\
	return _##n();			\
}					\
static bool _##n()

static void print_error(int cs, char c){
	printf("Parser error in state %d on %c\n", cs, c);
}

static void setup(){
	lm048_default_state.on_error = print_error;
}

TEST(test_macro_test)
{
	return true;
}

TEST(parse_cr)
{
	char* cr = "\x0d";
	size_t l = 1;

	enum LM048_STATUS s = lm048_input(cr, &l);
	lm048_restart(NULL);
	return s == LM048_OK;
}

TEST(parse_at)
{
	char* at = "at\x0d";
	char* At = "At\x0d";
	char* aT = "aT\x0d";
	char* AT = "AT\x0d";
	size_t l1 = 3, l2 = 3, l3 = 3, l4 = 3;

	return lm048_input(at, &l1) == LM048_COMPLETED
		&& lm048_input(At, &l2) == LM048_COMPLETED
		&& lm048_input(aT, &l3) == LM048_COMPLETED
		&& lm048_input(AT, &l4) == LM048_COMPLETED;
}

TEST(parse_at_at)
{
	char* at = "at\x0d""at\x0d";
	size_t l = 6;

	enum LM048_STATUS s = lm048_input(at, &l);
	if(s != LM048_COMPLETED){
		printf("parse_at_at: l = %zu, s = %d\n", l, s);
		return false;
	}
	if(l != 3){
		printf("parse_at_at: l = %zu", l);
		return false;
	}

	return lm048_input(at + l, &l) == LM048_COMPLETED;
}


TEST(parse_at_typo)
{
	char* ta = "ta";
	size_t l = 2;

	return lm048_input(ta, &l) == LM048_ERROR;
}

TEST(parse_at_partial)
{
	char* a = "a";
	size_t l = 1;

	return lm048_input(a, &l) == LM048_OK;
}
//Continues from the last test, very naughty.
TEST(parse_at_in_parts)
{
	char* t = "t\x0d";
	size_t l = 2;

	return lm048_input(t, &l) == LM048_COMPLETED;
}

TEST(parse_ok_response)
{
	char ok[6] = {0x0D, 0x0A, 'O', 'K', 0x0D, 0x0A};
	size_t l = 6;

	return lm048_input(ok, &l) == LM048_COMPLETED;
}

TEST(parse_error_response)
{
	char* error = "\x0d\x0a""ERROR\x0d\x0a";
	size_t l = 9;

	return lm048_input(error, &l) == LM048_COMPLETED;
}

TEST(init_state)
{
	struct lm048_parser state;

	lm048_init(&state);
	return state.cs == lm048_default_state.cs;
}

TEST(parse_at_with_state)
{
	char* at = "at\x0d";
	size_t l = 3;
	struct lm048_parser state;

	lm048_init(&state);
	return lm048_inputs(&state, at, &l) == LM048_COMPLETED;
}

TEST(parse_ver)
{
	size_t l = 7;
	return lm048_input("at+ver\x0d", &l) == LM048_COMPLETED;
}

TEST(skip_line)
{
	char *lines = "lsjdf sdksj lsdf\x0d""123";
	size_t l = strlen(lines);

	if(lm048_skip_line(lines, &l) != LM048_COMPLETED){
	   return false;
	}

	return l == 3;
}

TEST(enqueue_one)
{
	struct lm048_packet cmd = {
		.type = LM048_AT_AT
	};

	struct lm048_packet resp = {
		.type = LM048_AT_OK
	};

	return lm048_enqueue(NULL, cmd, resp) == LM048_COMPLETED;
}

TEST(enqueue_many)
{
	struct lm048_packet cmd = {
		.type = LM048_AT_AT
	};

	struct lm048_packet resp = {
		.type = LM048_AT_OK
	};

	struct lm048_packet a[100][2];
	struct lm048_queue que = lm048_queue_init(a, 100);

	for(int i = 0; i < 100 - 1; i++){
		if(lm048_enqueue(&que, cmd, resp) != LM048_COMPLETED){
			return false;
		}
	}

	return lm048_enqueue(&que, cmd, resp) == LM048_FULL;
}

TEST(expected_one)
{
	char const *const ok = "\x0d\x0a""OK\x0d\x0a";
	size_t l = 6;

	struct lm048_packet cmd = {
		.type = LM048_AT_AT
	};

	struct lm048_packet resp = {
		.type = LM048_AT_OK
	};

	struct lm048_packet a[100][2];
	struct lm048_queue que = lm048_queue_init(a, 100);

	struct lm048_parser state;
	lm048_init(&state);
	state.queue = &que;

	lm048_enqueue(&que, cmd, resp);

	return lm048_inputs(&state, ok, &l) == LM048_DEQUEUED;
}

TEST(expected_one_echo)
{
	char const *const at = "AT\x0D";
	char const *const ok = "\x0d\x0a""OK\x0d\x0a";
	size_t l = 3;

	struct lm048_packet cmd = {
		.type = LM048_AT_AT
	};

	struct lm048_packet resp = {
		.type = LM048_AT_OK
	};

	struct lm048_packet a[100][2];
	struct lm048_queue que = lm048_queue_init(a, 100);

	struct lm048_parser state;
	lm048_init(&state);
	state.queue = &que;

	lm048_enqueue(&que, cmd, resp);

	if(lm048_inputs(&state, at, &l) != LM048_OK){
		return false;
	}
	l = 6;
	return lm048_inputs(&state, ok, &l) == LM048_DEQUEUED;
}

TEST(expected_many_echo)
{
	char const *const at = "AT\x0D";
	char const *const ok = "\x0d\x0a""OK\x0d\x0a";
	size_t l = 3;

	struct lm048_packet cmd = {
		.type = LM048_AT_AT
	};

	struct lm048_packet resp = {
		.type = LM048_AT_OK
	};

	struct lm048_packet a[100][2];
	struct lm048_queue que = lm048_queue_init(a, 100);

	struct lm048_parser state;
	lm048_init(&state);
	state.queue = &que;

	for(int i = 0; i < 10; i++){
		lm048_enqueue(&que, cmd, resp);
	}

	for(int i = 0; i < 10; i++){
		l = 3;
		if(lm048_inputs(&state, at, &l) != LM048_OK){
			return false;
		}
		l = 6;
		if(lm048_inputs(&state, ok, &l) != LM048_DEQUEUED){
			return false;
		}
	}

	return true;
}

TEST(unexpected_one)
{
	char const *const ok = "\x0d\x0a""OK\x0d\x0a";
	size_t l = 6;

	struct lm048_packet cmd = {
		.type = LM048_AT_AT
	};

	struct lm048_packet resp = {
		.type = LM048_AT_ERROR
	};

	struct lm048_packet a[100][2];
	struct lm048_queue que = lm048_queue_init(a, 100);

	struct lm048_parser state;
	lm048_init(&state);
	state.queue = &que;

	lm048_enqueue(&que, cmd, resp);

	return lm048_inputs(&state, ok, &l) == LM048_UNEXPECTED;
}

TEST(queue_front_empty)
{
	struct lm048_packet a[100][2];
	struct lm048_queue que = lm048_queue_init(a, 100);
	struct lm048_packet const *fcmd;
	struct lm048_packet const *fresp;

	enum LM048_STATUS s = lm048_queue_front(&que, &fcmd, &fresp);

	return s == LM048_EMPTY && fcmd == NULL && fresp == NULL;
}

TEST(queue_front)
{
	struct lm048_packet cmd = {
		.type = LM048_AT_AT
	};

	struct lm048_packet resp = {
		.type = LM048_AT_ERROR
	};

	struct lm048_packet a[100][2];
	struct lm048_queue que = lm048_queue_init(a, 100);
	struct lm048_packet const *fcmd;
	struct lm048_packet const *fresp;

	lm048_enqueue(&que, cmd, resp);

	enum LM048_STATUS s = lm048_queue_front(&que, &fcmd, &fresp);

	return s == LM048_COMPLETED && fcmd->type == cmd.type
		&& fresp->type == resp.type;
}

TEST(write_at_at)
{
	char const *const at = "AT\x0d";
	struct lm048_packet cmd = {
		.type = LM048_AT_AT
	};

	char buf[LM048_MINIMUM_WRITE_BUFFER];
	size_t length;

	lm048_write_packet(buf, &length, &cmd);

	return strncmp(buf, at, length) == 0;
}

TEST(write_front_command)
{
	char const *const at = "AT\x0d";
	struct lm048_packet cmd = {
		.type = LM048_AT_AT
	};

	struct lm048_packet resp = {
		.type = LM048_AT_ERROR
	};

	struct lm048_packet a[100][2];
	struct lm048_queue que = lm048_queue_init(a, 100);
	char buf[LM048_MINIMUM_WRITE_BUFFER];
	size_t length = LM048_MINIMUM_WRITE_BUFFER;

	lm048_enqueue(&que, cmd, resp);

	enum LM048_STATUS s = lm048_write_front_command(&que, buf, &length);

	return s == LM048_COMPLETED && strncmp(at, buf, length) == 0;
}

int main(){
	int failed = 0;
	char *name = "unnamed!";
	//Array of test function pointers: bool test(char *name)
	bool (*tests[TESTCOUNT])(char **) = {
		test_macro_test,
		parse_cr,
		parse_at,
		parse_at_at,
		parse_at_typo,
		parse_at_partial,
		parse_at_in_parts,
		parse_ok_response,
		parse_error_response,
		init_state,
		parse_at_with_state,
		parse_ver,
		skip_line,
		enqueue_one,
		enqueue_many,
		expected_one,
		expected_one_echo,
		expected_many_echo,
		unexpected_one,
		queue_front_empty,
		queue_front,
		write_at_at,
		write_front_command
	};

	setup();
	printf("\n --==[ Running Tests ]==--\n\n");
	for(int i = 0; i < TESTCOUNT; i++) {
		if((*tests[i])(&name) == true) {
			printf("[Test %d]Passed %s\n", i+1, name);
		} else {
			failed++;
			printf("[Test %d]Failed %s!!!!!!!\n", i+1, name);
		}
		fflush(stdin);
	}

	printf("\n -{ %d tests failed out of %d }-\n", failed, TESTCOUNT);
	return 0;
}
