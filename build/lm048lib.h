#ifndef LM048LIB_H
#define LM048LIB_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define LM048_COMMAND_DELIMETER "\x0D"
#define LM048_RESPONSE_DELIMETER "\x0D\x0A"
#define LM048_DEFAULT_QUEUE_LENGTH 100
#define LM048_MINIMUM_WRITE_BUFFER 125

/* Function return statuses */
enum LM048_STATUS {
	//A response, command or action was processed/completed
	LM048_COMPLETED = 0,
	//No errors occurred, however the primary action was not completed
	LM048_OK = 1,
	//An error occurred while processing input or performing some other
	//action. This does not indicate that the AT error response was
	//processed
	LM048_ERROR = 2,
	//Storage of some type is full
	LM048_FULL = 3,
	//An expected response has been received and removed from the queue
	LM048_DEQUEUED = 4,
	//An unexpected response has been received; the expected response
	//has also been dequeued
	LM048_UNEXPECTED = 5,
	//Storage of some type is empty
	LM048_EMPTY = 6
};

//Enumeration of AT commands and responses
enum LM048_AT{
	//The absence of any command or response
	LM048_AT_NONE = -1,
	//The OK response
	LM048_AT_OK = 0,
	//The ERROR response
	LM048_AT_ERROR = 1,
	//The AT<cr> command
	LM048_AT_AT = 2,
	//The AT+VER? firmware version command
	LM048_AT_VER = 3
};

//Enumeration of AT command modifiers i.e +, -, ? and =
enum LM048_ATM{
	//AT+COMMAND+
	LM048_ATM_ENABLE,
	//AT+COMMAND-
	LM048_ATM_DISABLE,
	//AT+COMMAND?
	LM048_ATM_QUERY,
	//AT+COMMAND=<value>
	LM048_ATM_EQUALS
};

//Encapsulates a discrete AT command or response
struct lm048_packet {
	//The type of AT command or response
	enum LM048_AT type;
	//The sub type or variant
	enum LM048_ATM modifier;
	//Variable length data sent as part of some commands and responses
	char *payload;
};

//A queue of command, response pairs. 
//
//Various access functions are provided to avoid unexpected behaviour. It is
//not recommened to access the members directly.
struct lm048_queue {
	//A pointer to a two dimensional array
	struct lm048_packet (*array)[2];
	//The index of the item at the front of the queue
	size_t front;
	//The index of the first *empty* slot at the back of the queue
	size_t back;
	//The storage capacity of the array, *not* the current number
	//of items in the queue
	size_t length;
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

/* Parser state and callbacks
 * 
 * This structure encapsulates the parser's state machine's... state.
 * It also contains function pointers to callbacks which the user provides.
 * This allows you to inject code into the state machine if your desire to 
 * do so. In many cases you do not need to create your own parser struct,
 * as there is a default one embedded in the library.
 *
 * Call <lm048_init> on the lm048_parser struct before using it.
 */
struct lm048_parser {
	//The current state ID of the Ragel state machine. Changing this
	//variable directly requires carefull examination of the resultant 
	//state machine.
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
	//<lm048_input>(s) will return without processing anymore data.
	//Assigning a callback will stop this behaviour, which may have
	//unexpected effects e.g. it will sometimes stop items from being
	//dequeued.
	void (*on_completed)();

	//A queue of expected commands and responses.
	//
	//When an item is processed by <lm048_inputs> it is compared 
	//against the command-response at the front of this queue.
	struct lm048_queue *queue;
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

//The default parser state
//
//Used when NULL is passed to a <lm048_parser> function argument.
extern struct lm048_parser lm048_default_state;

void lm048_no_op(void);
void lm048_no_op_e(int cs, char c);

/* Parse the data 
 * @state The current state and callbacks or NULL
 * @data The text/bytes to parse
 * @length The length of the <data> on input and the length of the data 
 *         left to parse on output
 *
 * When data is received, feed it into this function. The <state> argument
 * will be updated accordingly to a new state based on the contents of
 * <data>. The behaviour of this function is dependant on what callbacks
 * have been registered and whether there are any items in the 
 * <lm048_parser::queue>.
 *
 * @return Either <LM048_OK> when no items were fully completed, but no
 *         errors occurred either, <LM048_ERROR> when invalid data or some
 *         other problem is encountered and <LM048_COMPLETED> when an item
 *         is fully processed. Also <LM048_DEQUEUED> and <LM048_UNEXPECTED>
 *         can occure when a queue is being used, see <lm048_queue>.
 */
enum LM048_STATUS lm048_inputs(struct lm048_parser *const state, 
			       char const *const data, 
			       size_t *const length);

/* Parse the data
 * @data The text/bytes to parse
 * @length The length of the <data>
 *
 * Performs the same purpose as <lm048_parser::lm048_inputs>, but with 
 * automatically managed state.
 *
 * @return see <lm048_parser::lm048_inputs>
 */
enum LM048_STATUS lm048_input(char const *const data, size_t *const length);

/* Set the parser to the begining state
 * @state A parser's state or NULL
 *
 * This sets the cs member of <state> to the start value. If <state>
 * is NULL then the default global state variable is used.
 */
void lm048_restart(struct lm048_parser *const state);

/* Sets the members of <lm048_parser> to safe defaults 
 * @state An uninitialised state
 *
 * This will overwrite all the members of the structure.
 */
void lm048_init(struct lm048_parser *const state);

/* Add a command-response pair to the end of <queue>
 * @queue The queue
 * @command The command to be sent, set <lm048_packet::type> to <LM048_AT_NONE>
 *	    if no command is to be sent
 * @response The expected response, set <lm048_packet::type> to <LM048_AT_NONE>
 *	     if no response is expected
 *
 * Adds a <command> to send and an expected <response> to the end of a <queue>.
 * Multiple requests and associated expected responses can be queued up for
 * later processing.
 *
 * @return <LM048_COMPLETED> on success, or <LM048_FULL> if there is no space
 *	   left in the queue.
 */
enum LM048_STATUS lm048_enqueue(struct lm048_queue *const queue,
				struct lm048_packet const command,
				struct lm048_packet const response);

/* Get the command-response pair at the front of the queue*/
enum LM048_STATUS
lm048_queue_front(struct lm048_queue const *const queue,
		  struct lm048_packet const **command,
		  struct lm048_packet const **response);

/* Initialise a new queue structure
 * @array A pointer to a two-dimensional array of <lm048_packets>
 * @length The row count of <array>
 *
 * @return The new queue
 */
struct lm048_queue
lm048_queue_init(struct lm048_packet (*const array)[2],
		 size_t const length);

/* Writes the bytes comprising an AT command or response to <buffer>
 * @buffer An array of at least <LM048_MINIMUM_WRITE_BUFFER> if possible
 * @length The length of the buffer on input and how many bytes were used/needed
 *	   on output
 * @packet The packet structure describing the AT command/response to be written
 *
 * This generates the textual representation of an AT command or response
 * from a <lm048_packet> ensuring the command is syntactically correct.
 *
 * @return <LM048_COMPLETED> on success, <LM048_FULL> if there was not enough
 *	   space in <buffer> and <LM048_ERROR> if <packet> is invalid.
 */
enum LM048_STATUS
lm048_write_packet(char *const buffer,
		   size_t *const length,
		   struct lm048_packet const *const packet);

/* Write the command at the front of <queue> to <buffer>
 * @queue The queue to use or NULL to use the default one
 * @buffer An array of at least <LM048_MINIMUM_WRITE_BUFFER> if possible
 * @length The length of the <buffer> on input and how many bytes were
 *	   used/needed on output
 *
 * This does not result in the front item from being dequeued. 
 *
 * TODO Commands expecting no response should probably be dequeued
 *
 * @return see <lm048_queue_front> and <lm048_write_packet>
 */
enum LM048_STATUS
lm048_write_front_command(struct lm048_queue const *const queue,
			 char *const buffer,
			 size_t *const length);

#ifdef __cplusplus
}
#endif

#endif //LM048LIB_H
