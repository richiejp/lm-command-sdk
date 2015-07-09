
//Ragel state machine variables which we need to define
struct ragelState {
	/** cs - The state of the machine, can be preserved between invocations */
	int currentState;

	/* The following should not be preserved between invocations */
	//char *dataPointer;
	//char *endDataPointer;
	//char *eofPointer;

	/* We have not used these yet */
	//int[] stack;
	//int stackTop;
	//int lastMatch;
	//char *ts;
	//char *te
};

int lm951InputWith(struct ragelState *state, char *data, int length);
int lm951Input(char *data, int length);
