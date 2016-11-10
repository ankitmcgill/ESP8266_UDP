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
#include "mem.h"
#include <ets_sys.h>
#include <espconn.h>


struct ESP8266_UDP_HANDLE
{
	struct espconn esp_connection_variable;
	esp_udp esp_udp_variable;
};

void ESP8266_UDP_create_listener(uint16_t udp_port, espconn_recv_callback callback, struct ESP8266_UDP_HANDLE* h);
void ESP8266_UDP_send_data_ip(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port_remote, uint16_t port_local, uint8_t* data, uint16_t data_len, struct ESP8266_UDP_HANDLE* h);
void ESP8266_UDP_send_receive_data_ip(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port_remote, uint16_t port_local, uint8_t* data, uint16_t data_len, espconn_recv_callback callback, struct ESP8266_UDP_HANDLE* h);
void ESP8266_UDP_delete(struct ESP8266_UDP_HANDLE* handle);

#endif
