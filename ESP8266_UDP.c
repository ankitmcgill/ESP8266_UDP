/*************************************************
* ESP8266 UDP LIBRARY
*
* NOVEMBER 1 2016
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
* ***********************************************/


#include "ESP8266_UDP.h"

void ESP8266_UDP_create_listener(uint16_t udp_port, espconn_recv_callback callback, struct ESP8266_UDP_HANDLE* h)
{
	//CREATE A UDP LISTENER ON THE SPECIFIED PORT
	//AND REGISTER A CALLBACK WITH THE SPECIFIED
	//FUNCTION POINTER

	//NOTE WHEN CREATING AND PASSING A POINTER TO STRUCTURE ESP8266_UDP_HANDLE FROM OTHER FILE, USE MALLOC/ZALLOC TO CREATE
	//THE STRUCTURE AND PASS THE POINTER LIKE THIS
	//struct ESP8266_UDP_HANDLE *h = (struct ESP8266_UDP_HANDLE*)os_zalloc(sizeof(struct ESP8266_UDP_HANDLE));
	//ELSE THE LIBRARY WILL FAIL

	int8_t error;

	//SET UDP PARAMETERS
	h->esp_connection_variable.type = ESPCONN_UDP;
	h->esp_connection_variable.state = ESPCONN_NONE;
	(h->esp_udp_variable).local_port = udp_port;
	h->esp_connection_variable.proto.udp = &(h->esp_udp_variable);

	//CREATE THE UDP CONNECTION
	error = espconn_create(&(h->esp_connection_variable));

	//REGISTER CALLBACK FUNCTION
	error = espconn_regist_recvcb(&(h->esp_connection_variable), callback);
}

void ESP8266_UDP_send_data_ip(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port, uint8_t* data, uint16_t data_len, struct ESP8266_UDP_HANDLE* h)
{
	//SEND THE SPECIFIED DATA OF THE SPECIFIED LENGTH TO THE REMOTE IP: PORT
	//SPECIFIED IN THE UDP HANDLE STRUCTURE

	h->esp_connection_variable.type = ESPCONN_UDP;
	h->esp_connection_variable.state = ESPCONN_NONE;
	h->esp_connection_variable.proto.udp = &(h->esp_udp_variable);
	IP4_ADDR((ip_addr_t*)h->esp_connection_variable.proto.udp->remote_ip, ip1, ip2, ip3, ip4);
	h->esp_connection_variable.proto.udp->remote_port = port;

	espconn_create(&(h->esp_connection_variable));
	espconn_send(&(h->esp_connection_variable), data, data_len);
	espconn_delete(&(h->esp_connection_variable));
	os_free(h);
}

void ESP8266_UDP_delete(struct ESP8266_UDP_HANDLE* handle)
{
	//DELETE THE SPECIFIED UDP HANDLE (CONTAINING THE
	//ESP8266 UDP CONNECTION VARIABLE

	espconn_delete(&(handle->esp_connection_variable));
	os_free(handle);
}
