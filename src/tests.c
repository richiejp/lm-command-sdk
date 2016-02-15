#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define TESTCOUNT 1

//Copied from https://github.com/richiejp/roth/blob/master/src/tests.c
#define TEST(n)	static bool _##n(char **name);	\
static bool n(char **name)			\
{						\
	*name = #n;				\
	return _##n;				\
}						\
static bool _##n()

//Array of test function pointers: bool test(char *name)
static bool (*tests[TESTCOUNT])(char **);

TEST(test_macro_test)
{
	return true;
}

int main(){
    int failed = 0;
    char *name = "unnamed!";
    
    roth_printf("\n --==[ Running Tests ]==--\n\n");
    
    tests[0] = test_macro_test;
    
    for(int i = 0; i < TESTCOUNT; i++) {
        if((*tests[i])(&name)) {
            roth_printf("[Test %d]Passed %s\n", i+1, name);
        } else {
            roth_printf("\n[Test %d]Failed %s!!!!!!!\n", i+1, name);
        }
    }
    
    roth_printf("\n -{ %d tests failed out of %d }-\n", failed, TESTCOUNT);
    return 0;
}
