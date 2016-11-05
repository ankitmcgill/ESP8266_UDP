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

void ESP8266_UDP_delete(struct ESP8266_UDP_HANDLE* handle)
{
	//DELETE THE SPECIFIED UDP HANDLE (CONTAINING THE
	//ESP8266 UDP CONNECTION VARIABLE

	espconn_delete(&(handle->esp_connection_variable));
}
