#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lm951lib.h"

#define TESTCOUNT 7

//Copied from https://github.com/richiejp/roth/blob/master/src/tests.c
//with permission of author :-)
#define TEST(n)	static bool _##n();	\
static bool n(char **name)		\
{					\
	*name = #n;			\
	return _##n();			\
}					\
static bool _##n()

//Array of test function pointers: bool test(char *name)
static bool (*tests[TESTCOUNT])(char **);

TEST(test_macro_test)
{
	return true;
}

TEST(parse_at)
{
	char* at = "at\x0d";
	char* At = "At\x0d";
	char* aT = "aT\x0d";
	char* AT = "AT\x0d";

	return lm951_input(at, 3) == LM951_COMPLETED
		&& lm951_input(At, 3) == LM951_COMPLETED
		&& lm951_input(aT, 3) == LM951_COMPLETED
		&& lm951_input(AT, 3) == LM951_COMPLETED;
}

TEST(parse_at_typo)
{
	char* ta = "ta";

	return lm951_input(ta, 2) == LM951_ERROR;
}

TEST(parse_at_partial)
{
	char* a = "a";

	return lm951_input(a, 1) == LM951_OK;
}
//Continues from the last test, very naughty.
TEST(parse_at_in_parts)
{
	char* t = "t\x0d";

	return lm951_input(t, 2) == LM951_COMPLETED;
}

TEST(parse_ok_response)
{
	char ok[6] = {0x0D, 0x0A, 'O', 'K', 0x0D, 0x0A};

	return lm951_input(ok, 6) == LM951_COMPLETED;
}

TEST(parse_error_response)
{
	char* error = "\x0d\x0a""ERROR\x0d\x0a";

	return lm951_input(error, 9) == LM951_COMPLETED;
}

int main(){
	int failed = 0;
	char *name = "unnamed!";

	printf("\n --==[ Running Tests ]==--\n\n");

	tests[0] = test_macro_test;
	tests[1] = parse_at;
	tests[2] = parse_at_typo;
	tests[3] = parse_at_partial;
	tests[4] = parse_at_in_parts;
	tests[5] = parse_ok_response;
	tests[6] = parse_error_response;

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
