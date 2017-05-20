/****************************************************************
* ESP8266 UDP CLIENT LIBRARY
*
* MAY 20 2017
*
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
*
* REFERENCES
* ------------
* 	(1) https://espressif.com/en/support/explore/sample-codes
****************************************************************/

#ifndef _ESP8266_UDP_CLIENT_H_
#define _ESP8266_UDP_CLIENT_H_


#include <ets_sys.h>
#include <espconn.h>
#include "user_interface.h"
#include "ip_addr.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"

#define ESP8266_UDP_CLIENT_DEBUG_ON
#define ESP8266_UDP_CLIENT_DNS_MAX_TRIES    5

//CUSTOM VARIABLE STRUCTURES/////////////////////////////
typedef enum
{
	ESP8266_UDP_CLIENT_STATE_DNS_RESOLVED,
	ESP8266_UDP_CLIENT_STATE_ERROR,
	ESP8266_UDP_CLIENT_STATE_OK
} ESP8266_UDP_CLIENT_STATE;
//END CUSTOM VARIABLE STRUCTURES/////////////////////////

//FUNCTION PROTOTYPES/////////////////////////////////////
//CONFIGURATION FUNCTIONS
void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_Initialize(const char* hostname,
													const char* host_ip,
													uint16_t host_port);
void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_SetDnsServer(char num_dns, ip_addr_t* dns);
void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_SetCallbackFunctions(void (*udp_con_cb)(void*, char*, unsigned short));

//GET PARAMETERS FUNCTIONS
const char* ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_GetSourceHost(void);
uint16_t ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_GetRemotePort(void);
uint16_t ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_GetLocalPort(void);
ESP8266_UDP_CLIENT_STATE ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_GetState(void);

//CONTROL FUNCTIONS
void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_ResolveHostName(void (*user_dns_cb_fn)(ip_addr_t*));
void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_SendData(uint8_t* data, uint16_t data_len);

//INTERNAL CALLBACK FUNCTIONS
void ICACHE_FLASH_ATTR _esp8266_udp_client_dns_timer_cb(void* arg);
void ICACHE_FLASH_ATTR _esp8266_udp_client_dns_found_cb(const char* name, ip_addr_t* ipAddr, void* arg);
void ICACHE_FLASH_ATTR _esp8266_udp_cleint_usd_rcv_cb()void* arg, char* pusrdata, unsigned short length);

//END FUNCTION PROTOTYPES/////////////////////////////////
#endif
