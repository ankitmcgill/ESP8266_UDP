/*************************************************
* ESP8266 UDP LIBRARY
*
* NOVEMBER 1 2016
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
* ***********************************************/

#ifndef _ESP8266_UDP_H_
#define _ESP8266_UDP_H_

#include "user_interface.h"
#include <ets_sys.h>
#include <espconn.h>


struct ESP8266_UDP_HANDLE
{
	struct espconn esp_connection_variable;
	esp_udp esp_udp_variable;
};


void ESP8266_UDP_create_listener(uint16_t udp_port, espconn_recv_callback callback, struct ESP8266_UDP_HANDLE* h);
void ESP8266_UDP_delete(struct ESP8266_UDP_HANDLE* handle);

#endif
