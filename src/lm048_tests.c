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

#define TESTCOUNT 31

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

TEST(parse_ver_response)
{
	char* version = "\x0d\x0a""F/W VERSION: v6.61\x0d\x0a""OK\x0d\x0a";
	size_t l = 26;

	return lm048_input(version, &l) == LM048_COMPLETED;
}

TEST(parse_ver_response_payload)
{
	char* version = "\x0d\x0a""F/W VERSION: v6.61\x0d\x0a""OK\x0d\x0a";
	size_t l = 26;

	lm048_input(version, &l);
	char buf[LM048_DEFAULT_PAYLOAD_LENGTH + 1];
	lm048_packet_payload(NULL, buf, LM048_DEFAULT_PAYLOAD_LENGTH + 1);

	printf("Payload(%d): %s\n",
	       lm048_default_state.current.payload_length,
	       buf);

	return strcmp("6.61", buf) == 0;
}

TEST(parse_pin_get)
{
	char *pin = "AT+PIN?\x0d";
	size_t l = strlen(pin);

	if(lm048_input(pin, &l) != LM048_COMPLETED){
		return false;
	}

	return lm048_default_state.current.type == LM048_AT_PIN;
}

TEST(parse_pin_enable)
{
	char *pin = "AT+PIN+\x0d";
	size_t l = strlen(pin);

	lm048_input(pin, &l);

	return lm048_default_state.current.modifier == LM048_ATM_ENABLE;
}

TEST(parse_pin_set)
{
	char *pin = "AT+Pin==% 1ff\x0d";
	size_t l = strlen(pin);
	lm048_input(pin, &l);
	char buf[LM048_DEFAULT_PAYLOAD_LENGTH + 1];
	lm048_packet_payload(NULL, buf, LM048_DEFAULT_PAYLOAD_LENGTH + 1);

	return strcmp("=% 1ff", buf) == 0;
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

TEST(expected_one_ver)
{
	char const *const ver = "AT+VER\x0D";
	size_t la = strlen(ver);
	char const *const version =
		"\x0d\x0a""F/W VERSION: v6.61\x0d\x0a""OK\x0d\x0a";
	size_t l = strlen(version);

	struct lm048_packet cmd = {
		.type = LM048_AT_VER
	};

	struct lm048_packet resp = {
		.type = LM048_AT_VER_RESPONSE,
		.payload = "6.61",
		.payload_length = 4,
		.payload_capacity = 5
	};

	struct lm048_packet a[100][2];
	struct lm048_queue que = lm048_queue_init(a, 100);

	struct lm048_parser state;
	lm048_init(&state);
	state.queue = &que;

	lm048_enqueue(&que, cmd, resp);

	if(lm048_inputs(&state, ver, &la) != LM048_OK){
		return false;
	}
	return lm048_inputs(&state, version, &l) == LM048_DEQUEUED;
}

TEST(expected_one_pin_get)
{
	char const *const pin = "AT+PIN?\x0D";
	size_t la = strlen(pin);
	char const *const pin_value = "\x0d\x0a""1234\x0d\x0a""OK\x0d\x0a";
	size_t l = strlen(pin_value);

	struct lm048_packet cmd = {
		.type = LM048_AT_PIN,
		.modifier = LM048_ATM_GET
	};

	struct lm048_packet resp = {
		.type = LM048_AT_VALUE_RESPONSE,
		.payload = "1234",
		.payload_length = 4,
		.payload_capacity = 5
	};

	struct lm048_packet a[100][2];
	struct lm048_queue que = lm048_queue_init(a, 100);

	struct lm048_parser state;
	lm048_init(&state);
	state.queue = &que;

	lm048_enqueue(&que, cmd, resp);

	if(lm048_inputs(&state, pin, &la) != LM048_OK){
		return false;
	}
	return lm048_inputs(&state, pin_value, &l) == LM048_DEQUEUED;
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
		.type = LM048_AT_AT,
		//-AT_AT command has no mod or payload, but should ignore
		//-these fields if they are set
		.modifier = LM048_ATM_SET,
		.payload_length = 8
	};

	char buf[LM048_MINIMUM_WRITE_BUFFER];
	size_t length;

	lm048_write_packet(&cmd, buf, &length);

	return memcmp(buf, at, length) == 0;
}

TEST(write_pin_set)
{
	char const *const pin = "AT+PIN=abcd\r";
	struct lm048_packet cmd = {
		.type = LM048_AT_PIN,
		.modifier = LM048_ATM_SET,
		.payload = "abcd",
		.payload_length = 4,
		.payload_capacity = 5
	};

	char buf[LM048_MINIMUM_WRITE_BUFFER];
	size_t l;

	lm048_write_packet(&cmd, buf, &l);

	if(strlen(pin) != l){
		printf("Packet length %lu\n", l);
		return false;
	}

	if(memcmp(buf, pin, strlen(pin)) != 0){
		buf[l] = '\0';
		printf("%s != %s\n", buf, pin);
		return false;
	}

	return true;
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

	return s == LM048_COMPLETED && memcmp(at, buf, length) == 0;
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
		parse_ver_response,
		parse_ver_response_payload,
		parse_pin_enable,
		parse_pin_get,
		parse_pin_set,
		skip_line,
		enqueue_one,
		enqueue_many,
		expected_one,
		expected_one_echo,
		expected_one_ver,
		expected_one_pin_get,
		expected_many_echo,
		unexpected_one,
		queue_front_empty,
		queue_front,
		write_at_at,
		write_pin_set,
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
