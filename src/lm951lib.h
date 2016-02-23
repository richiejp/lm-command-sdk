#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* State of the parser */
enum LM951_ISTATUS {
	//A response or command was processed
	LM951_COMPLETED = 0,
	//No errors occurred, but needs more data
	LM951_OK = 1,
	//A lexing error occurred, can not continue
	LM951_ERROR = 2
};

/* Parser state and callbacks
 * 
 * This structure encapsulates the parser's state machine's... uh, state.
 * It also contains function pointers to callbacks which the user provides.
 * This allows you to respond to events in the state machin as they happen.
 */
struct lm951_parser {
	/* The current state ID of the Ragel state machine */
	int cs;

	//- We have not used these yet
	//- int[] stack;
	//- int top;
	//- int act;
	//- char *ts;
	//- char *te
	
	void (*on_ok_response)();
	void (*on_error_response)();
	void (*on_error)(int current_state, char current_char);
	void (*on_completed)();
};

extern struct lm951_parser default_state;

void lm951_no_op(void);
void lm951_no_op_e(int cs, char c);

/* Parse the data 
 * @state The current state and callbacks
 * @data The text/bytes to parse
 * @length The length of the <data>
 *
 * When data is received, feed it into this function. The <state> argument
 * will be updated accordingly to a new state based on the contents of
 * <data>.
 *
 * @return TODO
 */
enum LM951_ISTATUS lm951_inputs(struct lm951_parser *state, 
				   char *data, 
				   size_t *length);

/* Parse the data
 * @data The text/bytes to parse
 * @length The length of the <data>
 *
 * Performs the same purpose as lm951_inputs, but with automatically
 * managed state.
 *
 * @return TODO
 */
enum LM951_ISTATUS lm951_input(char *data, size_t *length);

/* Set the parser to the begining state
 * @state A parser's state or NULL
 *
 * This sets the cs member of <state> to the start value. If <state>
 * is NULL then the default global state variable is used.
 */
void lm951_restart(struct lm951_parser *state);

#ifdef __cplusplus
}
#endif
