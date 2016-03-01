#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lm951lib.h"

#define TESTCOUNT 12

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
	lm951_default_state.on_error = print_error;
}

TEST(test_macro_test)
{
	return true;
}

TEST(parse_cr)
{
	char* cr = "\x0d";
	size_t l = 1;

	enum LM951_ISTATUS s = lm951_input(cr, &l);
	lm951_restart(NULL);
	return s == LM951_OK;
}

TEST(parse_at)
{
	char* at = "at\x0d";
	char* At = "At\x0d";
	char* aT = "aT\x0d";
	char* AT = "AT\x0d";
	size_t l1 = 3, l2 = 3, l3 = 3, l4 = 3;

	return lm951_input(at, &l1) == LM951_COMPLETED
		&& lm951_input(At, &l2) == LM951_COMPLETED
		&& lm951_input(aT, &l3) == LM951_COMPLETED
		&& lm951_input(AT, &l4) == LM951_COMPLETED;
}

TEST(parse_at_at)
{
	char* at = "at\x0d""at\x0d";
	size_t l = 6;

	enum LM951_ISTATUS s = lm951_input(at, &l);
	if(s != LM951_COMPLETED){
		printf("parse_at_at: l = %zu, s = %d\n", l, s);
		return false;
	}
	if(l != 3){
		printf("parse_at_at: l = %zu", l);
		return false;
	}

	return lm951_input(at + l, &l) == LM951_COMPLETED;
}


TEST(parse_at_typo)
{
	char* ta = "ta";
	size_t l = 2;

	return lm951_input(ta, &l) == LM951_ERROR;
}

TEST(parse_at_partial)
{
	char* a = "a";
	size_t l = 1;

	return lm951_input(a, &l) == LM951_OK;
}
//Continues from the last test, very naughty.
TEST(parse_at_in_parts)
{
	char* t = "t\x0d";
	size_t l = 2;

	return lm951_input(t, &l) == LM951_COMPLETED;
}

TEST(parse_ok_response)
{
	char ok[6] = {0x0D, 0x0A, 'O', 'K', 0x0D, 0x0A};
	size_t l = 6;

	return lm951_input(ok, &l) == LM951_COMPLETED;
}

TEST(parse_error_response)
{
	char* error = "\x0d\x0a""ERROR\x0d\x0a";
	size_t l = 9;

	return lm951_input(error, &l) == LM951_COMPLETED;
}

TEST(init_state)
{
	struct lm951_parser state;

	lm951_init(&state);
	return state.cs == lm951_default_state.cs;
}

TEST(parse_at_with_state)
{
	char* at = "at\x0d";
	size_t l = 3;
	struct lm951_parser state;

	lm951_init(&state);
	return lm951_inputs(&state, at, &l) == LM951_COMPLETED;
}

TEST(parse_ver)
{
	size_t l = 7;
	return lm951_input("at+ver\x0d", &l) == LM951_COMPLETED;
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
		parse_ver
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
	}

	printf("\n -{ %d tests failed out of %d }-\n", failed, TESTCOUNT);
	return 0;
}
