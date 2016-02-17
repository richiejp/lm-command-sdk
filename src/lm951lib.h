#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

enum LM951_ISTATUS {
	LM951_COMPLETED,
	LM951_OK,
	LM951_ERROR
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
};

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
				   int length);
enum LM951_ISTATUS lm951_input(char *data, int length);

#ifdef __cplusplus
}
#endif
