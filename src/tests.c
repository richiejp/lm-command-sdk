#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lm951lib.h"

#define TESTCOUNT 2

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
	char* at = "at";
	char* At = "At";
	char* aT = "aT";
	char* AT = "AT";
	char* ta = "ta";

	return lm951_input(at, 2) == LM951_COMPLETED
		&& lm951_input(At, 2) == LM951_COMPLETED
		&& lm951_input(aT, 2) == LM951_COMPLETED
		&& lm951_input(AT, 2) == LM951_COMPLETED
		&& lm951_input(ta, 2) == LM951_OK;
}

int main(){
    int failed = 0;
    char *name = "unnamed!";
    
    printf("\n --==[ Running Tests ]==--\n\n");
    
    tests[0] = test_macro_test;
    tests[1] = parse_at;
    
    for(int i = 0; i < TESTCOUNT; i++) {
        if((*tests[i])(&name) == true) {
            printf("[Test %d]Passed %s\n", i+1, name);
        } else {
            printf("\n[Test %d]Failed %s!!!!!!!\n", i+1, name);
        }
    }
    
    printf("\n -{ %d tests failed out of %d }-\n", failed, TESTCOUNT);
    return 0;
}
