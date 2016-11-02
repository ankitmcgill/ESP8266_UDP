/*************************************************
* ESP8266 UDP LIBRARY
*
* NOVEMBER 1 2016
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
* ***********************************************/


#include "ESP8266_UDP.h"

struct ESP8266_UDP_HANDLE ESP8266_UDP_create_listener(uint16_t udp_port, espconn_recv_callback callback)
{
	//CREATE A UDP LISTENER ON THE SPECIFIED PORT
	//AND REGISTER A CALLBACK WITH THE SPECIFIED
	//FUNCTION POINTER

	struct ESP8266_UDP_HANDLE handle;
	handle.esp_connection_variable;
	esp_udp esp_udp_variable;

	int8_t error;

	//SET UDP PARAMETERS
	handle.esp_connection_variable.type = ESPCONN_UDP;
	handle.esp_connection_variable.state = ESPCONN_NONE;
	esp_udp_variable.local_port = udp_port;
	handle.esp_connection_variable.proto.udp = &esp_udp_variable;

	//CREATE THE UDP CONNECTION
	error = espconn_create(&(handle.esp_connection_variable));

	//REGISTER CALLBACK FUNCTION
	error = espconn_regist_recvcb(&(handle.esp_connection_variable), callback);

	return handle;
}

void ESP8266_UDP_delete(struct ESP8266_UDP_HANDLE handle)
{
	//DELETE THE SPECIFIED UDP HANDLE (CONTAINING THE
	//ESP8266 UDP CONNECTION VARIABLE

	espconn_delete(&(handle.esp_connection_variable));
}
