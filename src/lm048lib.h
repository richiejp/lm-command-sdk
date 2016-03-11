#ifndef LM048LIB_H
#define LM048LIB_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define LM048_COMMAND_DELIMETER "\x0D"

/* State of the parser */
enum LM048_ISTATUS {
	//A response or command was processed
	LM048_COMPLETED = 0,
	//No errors occurred, but needs more data
	LM048_OK = 1,
	//A lexing error occurred, can not continue
	LM048_ERROR = 2
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

/* Parser state and callbacks
 * 
 * This structure encapsulates the parser's state machine's... uh, state.
 * It also contains function pointers to callbacks which the user provides.
 * This allows you to respond to events in the state machin as they happen.
 */
struct lm048_parser {
	/* The current state ID of the Ragel state machine */
	int cs;

	//- We have not used these yet
	//- int[] stack;
	//- int top;
	//- int act;
	//- char *ts;
	//- char *te
	
	//Action to take when OK response is parsed
	void (*on_ok_response)();

	//Action to take when ERROR response is parsed
	void (*on_error_response)();

	//Action to take when a lexing error occures
	void (*on_error)(int current_state, char current_char);

	//Action to take when a command or response is parsed
	//
	//The default action is to break out of the parsing loop, meaning
	//lm048_input(s) will return without processing anymore data.
	void (*on_completed)();
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

extern struct lm048_parser lm048_default_state;

void lm048_no_op(void);
void lm048_no_op_e(int cs, char c);

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
enum LM048_ISTATUS lm048_inputs(struct lm048_parser *state, 
				   char const *data, 
				   size_t *length);

/* Parse the data
 * @data The text/bytes to parse
 * @length The length of the <data>
 *
 * Performs the same purpose as <lm048_inputs>, but with automatically
 * managed state.
 *
 * @return TODO
 */
enum LM048_ISTATUS lm048_input(char const *data, size_t *length);

/* Set the parser to the begining state
 * @state A parser's state or NULL
 *
 * This sets the cs member of <state> to the start value. If <state>
 * is NULL then the default global state variable is used.
 */
void lm048_restart(struct lm048_parser *state);

void lm048_init(struct lm048_parser *state);

#ifdef __cplusplus
}
#endif

#endif //LM048LIB_H