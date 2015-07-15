#ifdef __cplusplus
extern "C" {
#endif

/**
 * Parser state and callbacks
 *
 * \var cs The state of the Ragel machine, can be preserved between 
 *	   invocations 
 */
struct lm951_parser {
	/** 
	int cs;

	/* We have not used these yet */
	//int[] stack;
	//int top;
	//int act;
	//char *ts;
	//char *te
};

/**
 * Parse the data
int lm951_input(struct lm951_parser *state, char *data, int length);
int lm951_input(char *data, int length);

#ifdef __cplusplus
}
#endif
