/** Copyright LM Technologies 2016
 *
 * This file is licensed under the terms of the Adaptive Public License 
 * set forth in licenses.txt in the root directory of the LM Command SDK 
 * repository.
 *
 * Author(s): Richard Palethorpe <richard@lm-technologies.com>
 */

#ifndef LM048LIB_H
#define LM048LIB_H

#if defined _WIN32 || defined __CYGWIN__
  #define LM048_HELPER_DLL_IMPORT __declspec(dllimport)
  #define LM048_HELPER_DLL_EXPORT __declspec(dllexport)
  #define LM048_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4 || defined __CLANG__
    #define LM048_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define LM048_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define LM048_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define LM048_HELPER_DLL_IMPORT
    #define LM048_HELPER_DLL_EXPORT
    #define LM048_HELPER_DLL_LOCAL
  #endif
#endif

#ifdef LM048_DLL
  //- defined if we are building the LM048 DLL (instead of using it)
  #ifdef LM048_DLL_EXPORTS 
    #define LM048_API LM048_HELPER_DLL_EXPORT
  #else
    #define LM048_API LM048_HELPER_DLL_IMPORT
  #endif // LM048_DLL_EXPORTS
  #define LM048_LOCAL LM048_HELPER_DLL_LOCAL
#else
  #define LM048_API
  #define LM048_LOCAL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#ifndef BOOL
#define BOOL int
#endif

#define LM048_COMMAND_DELIMETER "\x0D"
#define LM048_RESPONSE_DELIMETER "\x0D\x0A"
#define LM048_DEFAULT_QUEUE_LENGTH 100
#define LM048_DEFAULT_PAYLOAD_LENGTH 40
#define LM048_MINIMUM_WRITE_BUFFER 125

#ifdef DEBUG
#include <stdio.h>
#define LM_PRINTDBG printdbg
static void lm_printdbg(char const *const str)
{
	for(char const *c = str; *c != '\0'; c++){
		if(*c > 0x1F && *c < 0x7F)
			putchar(*c);
		else
			printf("\\x%02X", (unsigned)*c);
	}
	putchar('\n');
}
#else
#define LM_PRINTDBG
#endif


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
	//A common form of response containing a value.
	//Many query/get commands respond with <crlf><value><crlf>OK.
	//Unfortunately there is ambiguity between this response and all
	//other types of response, so this response will only be 
	//successfully parsed when the library is expecting it.
	// <lm048_queue::lm048_enqueue> sets the necessary context.
	LM048_AT_VALUE_RESPONSE = 3,
	//The AT+VER firmware version command
	LM048_AT_VER = 4,
	//The AT+VER firmware version response
	LM048_AT_VER_RESPONSE = 5,
	//The AT+PIN code command
	LM048_AT_PIN
};

//Enumeration of AT command modifiers i.e +, -, ? and =
enum LM048_ATM{
	//AT command does not have modifiers
	LM048_ATM_NONE,
	//AT+COMMAND+
	LM048_ATM_ENABLE,
	//AT+COMMAND-
	LM048_ATM_DISABLE,
	//AT+COMMAND?
	LM048_ATM_GET,
	//AT+COMMAND=<value>
	LM048_ATM_SET
};

//Encapsulates a discrete AT command or response
struct lm048_packet {
	//The type of AT command or response
	enum LM048_AT type;

	//The sub type or variant
	enum LM048_ATM modifier;

	//Variable length data sent as part of some commands and responses.
	//Always ensure that <payload_capacity> and <payload_length> are set 
	//correctly especially as payload may be populated from an 
	//untrustworthy source.
	char *payload;

	//The number of meaningful bytes in <payload>
	size_t payload_length;

	//The amount of memory, in bytes, allocated to the <payload> array
	size_t payload_capacity;
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
	// <lm048_input>(s) will return without processing anymore data.
	//Assigning a callback will stop this behaviour, which may have
	//unexpected effects e.g. it will sometimes stop items from being
	//dequeued.
	void (*on_completed)();

	//A queue of expected commands and responses.
	//
	//When an item is processed by <lm048_inputs> it is compared 
	//against the command-response at the front of this queue.
	struct lm048_queue *queue;

	//The packet being parsed or just finished parsing
	struct lm048_packet current;
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

//The default parser state
//
//Used when NULL is passed to a <lm048_parser> function argument.
extern LM048_API struct lm048_parser lm048_default_state;

LM048_LOCAL void lm048_no_op(void);
LM048_LOCAL void lm048_no_op_e(int cs, char c);

LM048_API
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

LM048_API
/* Parse the data
 * @data The text/bytes to parse
 * @length The length of the <data>
 *
 * Performs the same purpose as <lm048_parser::lm048_inputs>, but with 
 * automatically managed state.
 *
 * @return see <lm048_parser::lm048_inputs>
 */
enum LM048_STATUS lm048_input(char const *const data,
					size_t *const length);

LM048_API
/* Finds the next newline (carriage return) character
 * @data The text/bytes to parse
 * @length The length of the data on input and the amount left to process on
 *	   output
 *
 * Sometimes you may want to ignore all the data leading up to the next
 * carriage return or process it via a different method. Use <length> to
 * determine where the newline occurred.
 *
 * @return <LM048_COMPLETED> if a newline was found otherwise <LM048_OK>.
 */
enum LM048_STATUS lm048_skip_line(char *const data,
					    size_t *const length);

LM048_API
/* Set the parser to the begining state
 * @state A parser's state or NULL
 *
 * This sets the cs member of <state> to the start value. If <state>
 * is NULL then the default global state variable is used.
 */
void lm048_restart(struct lm048_parser *const state);

LM048_API
/* Sets the members of <lm048_parser> to safe defaults 
 * @state An uninitialised state
 *
 * This will overwrite all the members of the structure.
 */
void lm048_init(struct lm048_parser *const state);

LM048_API
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
enum LM048_STATUS
lm048_enqueue(struct lm048_queue *const queue,
	      struct lm048_packet const command,
	      struct lm048_packet const response);

LM048_API
/* Get the command-response pair at the front of the queue*/
enum LM048_STATUS
lm048_queue_front(struct lm048_queue const *const queue,
		  struct lm048_packet const **command,
		  struct lm048_packet const **response);

LM048_API
/* Initialise a new queue structure
 * @array A pointer to a two-dimensional array of <lm048_packets>
 * @length The row count of <array>
 *
 * @return The new queue
 */
struct lm048_queue
lm048_queue_init(struct lm048_packet (*const array)[2],
		 size_t const length);

LM048_API
/* Check's if packet type has modifiers
 * @pkt The packet to check
 *
 * @return 0 for no, 1 for yes and -1 for error
 */
BOOL lm048_packet_has_modifier(struct lm048_packet const *const pkt);

LM048_API
/* Check's if packet should have a payload
 * @pkt The packet to check
 *
 * Inspects the packet's type and modifiers to determine if there should be
 * a payload value. **Does not check if payload data is actually present**.
 *
 * @return 0 for no, 1 for yes and -1 for error
 */
BOOL lm048_packet_has_payload(struct lm048_packet const *const pkt);

LM048_API
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
lm048_write_packet(struct lm048_packet const *const packet,
		   char *const buffer,
		   size_t *const length);

LM048_API
//Initialize packet struct
enum LM048_STATUS
lm048_packet_init(struct lm048_packet *const packet,
		  char *const payload,
		  size_t payload_capacity);

LM048_API
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

LM048_API
/* Copies the <lm048_packet::payload> to a null terminated string
 * @packet The packet to get the payload from or NULL to use the
 *	   <lm048_parser::current> packet in the <lm048_default_state> 
 * @buffer An array with a length which is one greater than the capacity
 *	   of the payload being copied
 * @length The length of <buffer>
 *
 * The payload inside of the <lm048_packet> structure is not null
 * terminated and often the same packet structure is overwritten on 
 * successive calls to <lm048_parser::lm048_inputs>. This function copies
 * the payload to a place of your choosing and adds null termination.
 *
 * @return <LM048_COMPLETED> on success, <LM048_OK> if there is nothing
 *	   to do and <LM048_FULL> if there is not enough space in the
 *	   target buffer
 */
enum LM048_STATUS
lm048_packet_payload(struct lm048_packet const *const packet,
		     char *const buffer,
		     size_t const length);

#ifdef __cplusplus
}
#endif

#endif //LM048LIB_H
