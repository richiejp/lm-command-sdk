/** Copyright LM Technologies 2016
 *
 * This file is licensed under the terms of the Adaptive Public License 
 * set forth in licenses.txt in the root directory of the LM Command SDK 
 * repository.
 *
 * Author(s): Richard Palethorpe <richard@lm-technologies.com>
 */

#include "lm048lib.h"
#include <string.h>
#include <stdio.h>

#define CR LM048_COMMAND_DELIMETER
#define CRLF LM048_RESPONSE_DELIMETER
#define ATP "AT+"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

%%{
	machine skipline;

	main := (^'\r')* '\r' @{ fbreak; };
	
	write data;
}%%

LM048_API enum LM048_STATUS lm048_skip_line(char *const data,
					    size_t *const length)
{
	if(*length > 0){
		int cs;
		char *p = data;
		char *pe = p + *length;

		%% write init;
		%% write exec;

		*length -= (size_t)(p - data);

		if(cs == %%{ write first_final; }%%){
			return LM048_COMPLETED;
		}
	}

	return LM048_OK;
}

#define PTYPE(t) pkt->type = t
#define PMOD(m) pkt->modifier = m
%%{
	machine atcmd;

	access state->;

	action on_ok_resp {
		PTYPE(LM048_AT_OK);
		state->on_ok_response();
	}

	action on_error_resp {
		PTYPE(LM048_AT_ERROR);
		state->on_error_response();
	}

	action on_error {
		state->on_error(fcurs, fc);
	}

	action on_completed {
		if(state->on_completed != NULL){
			state->on_completed();
		}else{
			fbreak;
		}
	}

	action on_at_at { PTYPE(LM048_AT_AT); }
	action on_ver { PTYPE(LM048_AT_VER); }
	action on_ver_resp { PTYPE(LM048_AT_VER_RESPONSE); }
	action on_value_resp { PTYPE(LM048_AT_VALUE_RESPONSE); }
	action on_pin { PTYPE(LM048_AT_PIN); }
	action on_baud_cmd { PTYPE(LM048_AT_BAUD); }
	action on_baud_resp { PTYPE(LM048_AT_BAUD_RESPONSE); }
	action on_reset { PTYPE(LM048_AT_RESET); }
	action on_stop_bits_cmd { PTYPE(LM048_AT_STOP_BITS); }
	action on_stop_bits_resp { PTYPE(LM048_AT_STOP_BITS_RESPONSE); }
	action on_par_cmd { PTYPE(LM048_AT_PAR); }
	action on_par_resp { PTYPE(LM048_AT_PAR_RESPONSE); }
	action on_flow_cmd { PTYPE(LM048_AT_FLOW); }
	action on_flow_resp { PTYPE(LM048_AT_FLOW_RESPONSE); }
	action on_echo_cmd { PTYPE(LM048_AT_ECHO); }
	action on_echo_resp { PTYPE(LM048_AT_ECHO_RESPONSE); }
	action on_resp_cmd { PTYPE(LM048_AT_RESP); }
	action on_resp_resp { PTYPE(LM048_AT_RESP_RESPONSE); }
	action on_modem_cmd { PTYPE(LM048_AT_MODEM); }
	action on_modem_resp { PTYPE(LM048_AT_MODEM_RESPONSE); }
	action on_role_cmd { PTYPE(LM048_AT_ROLE); }
	action on_role_resp { PTYPE(LM048_AT_ROLE_RESPONSE); }
	action on_addr_cmd { PTYPE(LM048_AT_ADDR); }
	action on_rssi_cmd { PTYPE(LM048_AT_RSSI); }
	action on_rssi_resp { PTYPE(LM048_AT_RSSI_RESPONSE); }
	action on_name_cmd { PTYPE(LM048_AT_NAME); }

	action on_enable_mod { PMOD(LM048_ATM_ENABLE); }
	action on_disable_mod { PMOD(LM048_ATM_DISABLE); }
	action on_get_mod { PMOD(LM048_ATM_GET); }
	action on_set_mod { PMOD(LM048_ATM_SET); }

	action on_connect_evnt { PTYPE(LM048_AT_CONNECT_EVNT); }
	action on_disconnect_evnt { PTYPE(LM048_AT_DISCONNECT_EVNT); }

	action add_to_payload { payload_add(pkt, *p); }
	action clear_payload { pkt->payload_length = 0; }

	action resp_context {
		if(resp != NULL){
			switch(resp->type){
			case LM048_AT_VALUE_RESPONSE:
				pkt->payload_length = 0;
				fgoto value_resp;
			case LM048_AT_ANY_EVNT:
			case LM048_AT_CONNECT_EVNT:
			case LM048_AT_DISCONNECT_EVNT:
				fgoto events;
			}
		}
	}

	cr = '\r';
	lf = '\n';
	crlf = cr lf;
	at = [aA][tT];
	ok = 'OK' crlf; 
	error = 'ERROR' crlf;

	on = '+' @on_enable_mod;
	off = '-' @on_disable_mod;
	get = '?' @on_get_mod;
	set = '=' @on_set_mod;

	command_resp = (ok @on_ok_resp | error @on_error_resp);
	
	ver = [vV][eE][rR] %on_ver;
	ver_resp = 'F/W VERSION:' ' '{1,5} 'v' @clear_payload .
		   (digit{1,2} '.' digit{2,3}) $add_to_payload .
		   ' '{,5} crlf ok @on_ver_resp;

	pin = [pP][iI][nN] .
		(on | off | get | 
		 set @clear_payload .
		 (alnum | punct | [\t\v\f ])+ @add_to_payload) %on_pin;

	baud = [bB][aA][uU][dD] @clear_payload .
		(get |
		 '10' @{ PMOD(LM048_ATM_BAUD_1200); } |
		 '11' @{ PMOD(LM048_ATM_BAUD_2400); } |
		 '12' @{ PMOD(LM048_ATM_BAUD_4800); } |
		 '13' @{ PMOD(LM048_ATM_BAUD_9600); } |
		 '14' @{ PMOD(LM048_ATM_BAUD_19200); } |
		 '15' @{ PMOD(LM048_ATM_BAUD_38400); } |
		 '16' @{ PMOD(LM048_ATM_BAUD_57600); } |
		 '17' @{ PMOD(LM048_ATM_BAUD_115200); } |
		 '18' @{ PMOD(LM048_ATM_BAUD_230400); } |
		 '19' @{ PMOD(LM048_ATM_BAUD_460800); } |
		 '20' @{ PMOD(LM048_ATM_BAUD_921600); });

	baud_cmd = baud %on_baud_cmd;
	baud_resp = baud crlf ok @on_baud_resp;

	reset = [rR][eE][sS][eE][tT] %on_reset;

	stop_bits = [sS][tT][oO][pP] .
		(get | 
		 '1' @{ PMOD(LM048_ATM_ONE_STOPBIT); } |
		 '2' @{ PMOD(LM048_ATM_TWO_STOPBIT); });
	stop_bits_cmd = stop_bits %on_stop_bits_cmd;
	stop_bits_resp = stop_bits crlf ok @on_stop_bits_resp;

	par = [pP][aA][rR] .
		(get | 
		 '0' @{ PMOD(LM048_ATM_NO_PARITY); } |
		 '1' @{ PMOD(LM048_ATM_ODD_PARITY); } |
		 '2' @{ PMOD(LM048_ATM_EVEN_PARITY); });
	par_cmd = par %on_par_cmd;
	par_resp = par crlf ok @on_par_resp;

	flow = [fF][lL][oO][wW] (get | on | off); 
	flow_cmd = flow %on_flow_cmd;
	flow_resp = flow crlf ok @on_flow_resp;

	echo = [eE][cC][hH][oO] (get | on | off);
	echo_cmd = echo %on_echo_cmd;
	echo_resp = echo crlf ok @on_echo_resp;

	resp = [rR][eE][sS][pP] (get | on | off);
	resp_cmd = resp %on_resp_cmd;
	resp_resp = resp crlf ok @on_resp_resp;

	modem = [mM][oO][dD][eE][mM] @clear_payload 
		(get | off |
		 [lL] @{ PMOD(LM048_ATM_LOCAL_LOOPBACK); } |
		 [rR] @{ PMOD(LM048_ATM_REMOTE_TRANSFER); });
	modem_cmd = modem %on_modem_cmd;
	modem_resp = modem crlf ok @on_modem_resp;

	role = [rR][oO][lL][eE] @clear_payload 
		(get |
		 [mM] @{ PMOD(LM048_ATM_MASTER); } |
		 [sS] @{ PMOD(LM048_ATM_SLAVE); });
	role_cmd = role %on_role_cmd;
	role_resp = role crlf ok @on_role_resp;

	addr_cmd = [aA][dD][dD][rR] get %on_addr_cmd;

	#at+find? would go here, but it is not supported yet.

	rssi_cmd = [rR][sS][sS][iI] %on_rssi_cmd;
	rssi_resp = 'STRONG' @{ PMOD(LM048_ATM_STRONG_RSSI); } |
		    'AVERAGE' @{ PMOD(LM048_ATM_AVERAGE_RSSI); } |
		    'WEAK' @{ PMOD(LM048_ATM_WEAK_RSSI); } .
		    crlf ok @on_rssi_resp;

	name_cmd = [nN][aA][mM][eE] 
		(get | 
		 set @clear_payload 
		 ((alnum (alnum | ' ' | '-'){0,14} alnum) | alnum) 
		 $add_to_payload) %on_name_cmd; 

	bluetooth_addr = 
		(alnum{4} '-' alnum{2} '-' alnum{6}) $add_to_payload; 

	connect_evnt = 'CONNECT' ' '{0,2} '"' @clear_payload
		       bluetooth_addr  '"' crlf @on_connect_evnt;
	disconnect_evnt = 'DISCONNECT' ' '{0,2} '"' @clear_payload
			bluetooth_addr '"' crlf @on_disconnect_evnt;

	value_resp := ((any - cr)+ $add_to_payload crlf ok @on_value_resp) 
			     @!on_error %~on_completed;
	events := (connect_evnt | disconnect_evnt) 
			@!on_error %~on_completed;

	#The @resp_context action can jump to value_resp or events
	responses = lf @resp_context (command_resp | 
				      ver_resp | 
				      baud_resp |
				      stop_bits_resp |
				      par_resp |
				      flow_resp |
				      echo_resp |
				      modem_resp |
				      role_resp |
				      rssi_resp );

	commands = at (cr @on_at_at | 
		       ('+' (ver | 
			     pin | 
			     baud_cmd |
			     reset | 
			     stop_bits_cmd |
			     par_cmd |
			     flow_cmd |
			     echo_cmd |
			     resp_cmd |
			     modem_cmd |
			     role_cmd |
			     addr_cmd |
			     rssi_cmd |
			     name_cmd) 
			cr));

	main := (cr* (commands | responses)) %~on_completed @!on_error;
	
	write data;
}%%

void lm048_no_op(){}
void lm048_no_op_e(int cs, char c){}

#ifdef __clang__
#pragma clang diagnostic pop
#endif

static struct lm048_packet
default_queue_array[LM048_DEFAULT_QUEUE_LENGTH][2];

static struct lm048_queue default_queue = {
	.array = default_queue_array,
	.front = 0,
	.back = 0,
	.length = LM048_DEFAULT_QUEUE_LENGTH
};

static char default_payload[LM048_DEFAULT_PAYLOAD_LENGTH];
LM048_API struct lm048_parser lm048_default_state = {
	.cs = %%{ write start; }%%,
	.on_ok_response = lm048_no_op,
	.on_error_response = lm048_no_op,
	.on_error = lm048_no_op_e,
	.on_completed = NULL,
	.queue = &default_queue,
	.current.payload = default_payload,
	.current.payload_length = 0,
	.current.payload_capacity = LM048_DEFAULT_PAYLOAD_LENGTH
};

static enum LM048_STATUS payload_add(struct lm048_packet *const pkt, char c)
{
	if(pkt->payload_length < pkt->payload_capacity){
		pkt->payload[pkt->payload_length] = c;
		pkt->payload_length += 1;
		return LM048_COMPLETED;
	}

	return LM048_FULL;
}

LM048_API enum LM048_STATUS lm048_packet_init(struct lm048_packet *const pkt,
				char *const payload,
				size_t payload_capacity)
{
	pkt->type = LM048_AT_NONE;
	pkt->modifier = LM048_ATM_NONE;
	pkt->payload = payload;
	pkt->payload_length = 0;
	pkt->payload_capacity = payload_capacity;

	return LM048_COMPLETED;
}

LM048_API enum LM048_STATUS lm048_enqueue(struct lm048_queue *const queue,
				struct lm048_packet const command,
				struct lm048_packet const response)
{
	struct lm048_queue *que = queue;

	if(que == NULL){
		que = &default_queue;
	}
	que->array[que->back][0] = command;
	que->array[que->back][1] = response;
	if(que->length <= ++que->back){
		que->back = 0;
		if(que->back == que->front){
			return LM048_FULL;
		}
	}

	return LM048_COMPLETED;
}

static enum LM048_STATUS dequeue(struct lm048_queue *const queue,
				 struct lm048_packet const *const received){
	if(queue->front == queue->back){
		return LM048_COMPLETED;
	}

	struct lm048_packet *cmd_echo = &(queue->array[queue->front][0]);
	struct lm048_packet *expected = &(queue->array[queue->front][1]);

	if(cmd_echo->type == received->type){
		return LM048_OK;
	}

	if(queue->length <= ++queue->front){
		queue->front = 0;
	}

	if(expected->type != received->type){
		return LM048_UNEXPECTED;
	}

	if(lm048_packet_has_modifier(expected)
	   && expected->modifier != received->modifier){
		return LM048_UNEXPECTED;
	}

	if(lm048_packet_has_payload(expected)
	   && expected->payload_length != 0
	   && (expected->payload_length != received->payload_length
	       || strncmp(expected->payload,
			  received->payload,
			  expected->payload_length)))
	{
		return LM048_UNEXPECTED;
	}

	return LM048_DEQUEUED;
}

LM048_API enum LM048_STATUS
lm048_queue_front(struct lm048_queue const *const queue,
		  struct lm048_packet const **cmd,
		  struct lm048_packet const **resp)
{
	struct lm048_queue const *que = &default_queue;
	if(queue != NULL){
		que = queue;
	}

	enum LM048_STATUS ret;

	if(que->front == que->back){
		if(cmd != NULL)
			*cmd = NULL;
		if(resp != NULL)
			*resp = NULL;
		ret = LM048_EMPTY;
	}else{
		if(cmd != NULL)
			*cmd = &(que->array[que->front][0]);
		if(resp != NULL)
			*resp = &(que->array[que->front][1]);
		ret = LM048_COMPLETED;
	}

	return ret;
}

LM048_API struct lm048_queue
lm048_queue_init(struct lm048_packet (*const array)[2],
		 size_t const length)
{
	struct lm048_queue q = {
		.array = array,
		.front = 0,
		.back = 0,
		.length = length
	};
	return q;
}

LM048_API int lm048_packet_has_modifier(struct lm048_packet const *const pkt)
{
	switch(pkt->type){
	case LM048_AT_NONE:
	case LM048_AT_OK:
	case LM048_AT_ERROR:
	case LM048_AT_AT:
	case LM048_AT_VALUE_RESPONSE:
	case LM048_AT_VER:
	case LM048_AT_VER_RESPONSE:
	case LM048_AT_RESET:
	case LM048_AT_ENQ:
	case LM048_AT_CONNECT_EVNT:
	case LM048_AT_DISCONNECT_EVNT:
		return 0;
	case LM048_AT_PIN:
	case LM048_AT_BAUD:
	case LM048_AT_BAUD_RESPONSE:
		return 1;
	default:
		return -1;
	}
}

LM048_API int lm048_packet_has_payload(struct lm048_packet const *const pkt)
{
	switch(pkt->type){

	//-Never has payload
	case LM048_AT_NONE:
	case LM048_AT_OK:
	case LM048_AT_ERROR:
	case LM048_AT_AT:
	case LM048_AT_VER:
	case LM048_AT_RESET:
	case LM048_AT_ENQ:
	case LM048_AT_BAUD:
	case LM048_AT_BAUD_RESPONSE:
	case LM048_AT_PAR:
	case LM048_AT_PAR_RESPONSE:
	case LM048_AT_FLOW:
	case LM048_AT_FLOW_RESPONSE:
	case LM048_AT_ECHO:
	case LM048_AT_ECHO_RESPONSE:
	case LM048_AT_RESP:
	case LM048_AT_RESP_RESPONSE:
	case LM048_AT_MODEM:
	case LM048_AT_MODEM_RESPONSE:
	case LM048_AT_ROLE:
	case LM048_AT_ROLE_RESPONSE:
	case LM048_AT_DCOV:
	case LM048_AT_DCOV_RESPONSE:
	case LM048_AT_DROP:
	case LM048_AT_ACON:
	case LM048_AT_ACON_RESPONSE:
	case LM048_AT_ESC:
	case LM048_AT_ESC_RESPONSE:
	case LM048_AT_AUTO:
	case LM048_AT_RCFG:
	case LM048_AT_RCFG_RESPONSE:
	case LM048_AT_SLEEP:
	case LM048_AT_SLEEP_RESPONSE:
	case LM048_AT_DPIN:
	case LM048_AT_DPIN_RESPONSE:
	case LM048_AT_IOTYPE:
	case LM048_AT_IOTYPE_RESPONSE:
	case LM048_AT_MITM:
	case LM048_AT_MITM_RESPONSE:
	case LM048_AT_CONNECT_FAIL_EVNT:
	case LM048_AT_RSSI:
		return 0;

	//-Always has payload
	case LM048_AT_VALUE_RESPONSE:
	case LM048_AT_VER_RESPONSE:
	case LM048_AT_CONNECT_EVNT:
	case LM048_AT_DISCONNECT_EVNT:
	case LM048_AT_CONN:
	case LM048_AT_PASSCFM:
	case LM048_AT_PASSKEY:
	case LM048_AT_PINREQ_EVNT:
	case LM048_AT_PASSKEYCFM_EVNT:
	case LM048_AT_PASSKEYDSP_EVNT:
	case LM048_AT_PASSKEYREQ_EVNT:
		return 1;

	//-Depends on modifier
	case LM048_AT_PIN:
	case LM048_AT_ADDR:
	case LM048_AT_NAME:
	case LM048_AT_BOND:
		break;
	default:
		return -1;
	}

	switch(pkt->modifier){
	case LM048_ATM_ENABLE:
	case LM048_ATM_DISABLE:
	case LM048_ATM_GET:
		return 0;
	case LM048_ATM_SET:
		return 1;
	}
	return -1;
}

LM048_API enum LM048_STATUS
lm048_write_packet(struct lm048_packet const *const packet,
		   char *const buf,
		   size_t *const length)
{
	size_t len = 0;
	char const *cmd;
	char const *mod;
	char const *end = CR;

	switch(packet->type){
	case LM048_AT_NONE:
		return LM048_OK;
	case LM048_AT_OK:
		cmd = CRLF "OK";
		end = CRLF;
		break;
	case LM048_AT_ERROR:
		cmd = CRLF "ERROR";
		end = CRLF;
		break;
	case LM048_AT_AT:
		cmd = "AT";
		break;
	case LM048_AT_VALUE_RESPONSE:
		cmd = CRLF;
		end = CRLF "OK" CRLF;
		break;
	case LM048_AT_VER:
		cmd = ATP "VER";
		break;
	case LM048_AT_VER_RESPONSE:
		cmd = CRLF "F/W VERSION:";
		end = CRLF "OK" CRLF;
		break;
	case LM048_AT_PIN:
		cmd = ATP "PIN";
		break;
	case LM048_AT_BAUD:
		cmd = ATP "BAUD";
		break;
	case LM048_AT_BAUD_RESPONSE:
		cmd = CRLF "BAUD";
		end = CRLF "OK" CRLF;
		break;
	case LM048_AT_RESET:
		cmd = ATP "RESET";
		break;
	case LM048_AT_ENQ:
		cmd = ATP "ENQ";
		break;
	case LM048_AT_STOP_BITS:
		cmd = ATP "STOP";
		break;
	case LM048_AT_STOP_BITS_RESPONSE:
	case LM048_AT_PAR:
	case LM048_AT_PAR_RESPONSE:
	case LM048_AT_FLOW:
	case LM048_AT_FLOW_RESPONSE:
	case LM048_AT_ECHO:
	case LM048_AT_ECHO_RESPONSE:
	case LM048_AT_RESP:
	case LM048_AT_RESP_RESPONSE:
	case LM048_AT_MODEM:
	case LM048_AT_MODEM_RESPONSE:
	case LM048_AT_ROLE:
	case LM048_AT_ROLE_RESPONSE:
	case LM048_AT_ADDR:
	case LM048_AT_RSSI:
	case LM048_AT_NAME:
	case LM048_AT_DCOV:
	case LM048_AT_DCOV_RESPONSE:
	case LM048_AT_CONN:
	case LM048_AT_DROP:
	case LM048_AT_BOND:
	case LM048_AT_ACON:
	case LM048_AT_ACON_RESPONSE:
	case LM048_AT_ESC:
	case LM048_AT_ESC_RESPONSE:
	case LM048_AT_AUTO:
	case LM048_AT_RCFG:
	case LM048_AT_RCFG_RESPONSE:
	case LM048_AT_SLEEP:
	case LM048_AT_SLEEP_RESPONSE:
	case LM048_AT_DPIN:
	case LM048_AT_DPIN_RESPONSE:
	case LM048_AT_IOTYPE:
	case LM048_AT_IOTYPE_RESPONSE:
	case LM048_AT_MITM:
	case LM048_AT_MITM_RESPONSE:
	case LM048_AT_PASSCFM:
	case LM048_AT_PASSKEY:
	default:
		*length = 0;
		return LM048_ERROR;
	}

	switch(packet->modifier){
	case LM048_ATM_ENABLE:
		mod = "+";
		break;
	case LM048_ATM_DISABLE:
		mod = "-";
		break;
	case LM048_ATM_GET:
		mod = "?";
		break;
	case LM048_ATM_SET:
		mod = "=";
		break;
	case LM048_ATM_BAUD_1200:
		mod = "10";
		break;
	case LM048_ATM_BAUD_2400:
		mod = "11";
		break;
	case LM048_ATM_BAUD_4800:
		mod = "12";
		break;
	case LM048_ATM_BAUD_9600:
		mod = "13";
		break;
	case LM048_ATM_BAUD_19200:
		mod = "14";
		break;
	case LM048_ATM_BAUD_38400:
		mod = "15";
		break;
	case LM048_ATM_BAUD_57600:
		mod = "16";
		break;
	case LM048_ATM_BAUD_115200:
		mod = "17";
		break;
	case LM048_ATM_BAUD_230400:
		mod = "18";
		break;
	case LM048_ATM_BAUD_460800:
		mod = "19";
		break;
	case LM048_ATM_BAUD_921600:
		mod = "20";
		break;
	case LM048_ATM_NO_PARITY:
	case LM048_ATM_DISPLAY_ONLY:
		mod = "0";
		break;
	case LM048_ATM_ONE_STOPBIT:
	case LM048_ATM_ODD_PARITY:
	case LM048_ATM_DISPLAY_YES_NO:
		mod = "1";
		break;
	case LM048_ATM_TWO_STOPBIT:
	case LM048_ATM_EVEN_PARITY:
	case LM048_ATM_KEYBOARD_ONLY:
		mod = "2";
		break;
	case LM048_ATM_NO_DISPLAY_KEYBOARD:
		mod = "3";
		break;
	case LM048_ATM_REJECT_BT21:
		mod = "4";
		break;
	case LM048_ATM_LOCAL_LOOPBACK:
		mod = "L";
		break;
	case LM048_ATM_REMOTE_TRANSFER:
		mod = "R";
		break;
	case LM048_ATM_MASTER:
		mod = "M";
		break;
	case LM048_ATM_SLAVE:
		mod = "S";
		break;
	case LM048_ATM_NONE:
		break;
	}

	len = strlen(cmd) + strlen(end);
	if(lm048_packet_has_modifier(packet))
		len += strlen(mod);
	if(lm048_packet_has_payload(packet))
		len += packet->payload_length;

	if(len >= *length){
		*length = len;
		return LM048_FULL;
	}

	*buf = '\0';
	strcat(buf, cmd);
	if(lm048_packet_has_modifier(packet))
		strcat(buf, mod);
	if(lm048_packet_has_payload(packet))
		strncat(buf, packet->payload, packet->payload_length);
	strcat(buf, end);

	*length = len;
	return LM048_COMPLETED;
}

LM048_API enum LM048_STATUS
lm048_write_front_command(struct lm048_queue const *const queue,
			 char *const buffer,
			 size_t *const length)
{
	struct lm048_packet const *cmd, *resp;
	enum LM048_STATUS s = lm048_queue_front(queue, &cmd, &resp);
	if(s != LM048_COMPLETED){
		*length = 0;
		return s;
	}

	return lm048_write_packet(cmd, buffer, length);
}


LM048_API enum LM048_STATUS lm048_inputs(struct lm048_parser *const state,
				         char const *const data,
				         size_t *const length)
{
	struct lm048_packet const *resp;
	struct lm048_packet *const pkt = &state->current;

	lm048_queue_front(state->queue, NULL, &resp);

	if(*length > 0 && data != NULL){
		char const *p = data;
		char const *pe = p + *length;
		char const *eof = NULL;

		%% write init nocs;
		%% write exec;

		*length = *length - (size_t)(p - data);
	}

	if(state->cs >= %%{ write first_final; }%%){
		state->cs = %%{ write start; }%%;
		return dequeue(state->queue, pkt);
	}else if(state->cs == %%{ write error; }%%){
		state->cs = %%{ write start; }%%;
		return LM048_ERROR;
	}else{
		return LM048_OK;
	}
}

LM048_API enum LM048_STATUS
lm048_input(char const *const data, size_t *const length)
{
	enum LM048_STATUS s =
		lm048_inputs(&lm048_default_state, data, length);
	return s;
}

LM048_API void lm048_restart(struct lm048_parser *state)
{
	if(state == NULL){
		state = &lm048_default_state;
	}

	state->cs = %%{ write start; }%%;
}

LM048_API void lm048_init(struct lm048_parser *state)
{
	state->cs = %%{ write start; }%%;
	state->on_ok_response = lm048_no_op;
	state->on_error_response = lm048_no_op;
	state->on_error = lm048_no_op_e;
	state->on_completed = NULL;
	state->queue = &default_queue;

	lm048_packet_init(&state->current,
			  default_payload,
			  LM048_DEFAULT_PAYLOAD_LENGTH);
}

LM048_API enum LM048_STATUS
lm048_packet_payload(struct lm048_packet const *const packet,
		     char *const buffer,
		     size_t const length)
{
	struct lm048_packet const *pkt =
		&(lm048_default_state.current);

	if(packet != NULL){
		pkt = packet;
	}

	if(length <= pkt->payload_capacity){
		return LM048_FULL;
	}

	if(pkt->payload_length < 1){
		return LM048_OK;
	}

	memcpy(buffer, pkt->payload, pkt->payload_length);
	buffer[pkt->payload_length] = '\0';

	return LM048_COMPLETED;
}
