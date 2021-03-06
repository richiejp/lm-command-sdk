/** Copyright LM Technologies 2016
 *
 * This file is licensed under the terms of the Adaptive Public License 
 * set forth in licenses.txt in the root directory of the LM Command SDK 
 * repository.
 *
 * Author(s): Richard Palethorpe <richard@lm-technologies.com>
 */

#ifndef LM951LIB_H
#define LM951LIB_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define LM951_COMMAND_DELIMETER "\x0D"

/* State of the parser */
enum LM951_ISTATUS {
	//A response or command was processed
	LM951_COMPLETED = 0,
	//No errors occurred, but needs more data
	LM951_OK = 1,
	//A lexing error occurred, can not continue
	LM951_ERROR = 2
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
struct lm951_parser {
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
	//lm951_input(s) will return without processing anymore data.
	void (*on_completed)();
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

extern struct lm951_parser lm951_default_state;

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
				   char const *data, 
				   size_t *length);

/* Parse the data
 * @data The text/bytes to parse
 * @length The length of the <data>
 *
 * Performs the same purpose as <lm951_inputs>, but with automatically
 * managed state.
 *
 * @return TODO
 */
enum LM951_ISTATUS lm951_input(char const *data, size_t *length);

/* Set the parser to the begining state
 * @state A parser's state or NULL
 *
 * This sets the cs member of <state> to the start value. If <state>
 * is NULL then the default global state variable is used.
 */
void lm951_restart(struct lm951_parser *state);

void lm951_init(struct lm951_parser *state);

#ifdef __cplusplus
}
#endif

#endif //LM951LIB_H
