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


#include "ESP8266_UDP_CLIENT.h"

void ESP8266_UDP_create_listener(uint16_t udp_port, espconn_recv_callback callback, struct ESP8266_UDP_HANDLE* h)
{
	//CREATE A UDP LISTENER ON THE SPECIFIED PORT AND REGISTER A CALLBACK WITH THE SPECIFIED
	//FUNCTION POINTER
	//THIS FUNCTION IS UNIDIRECTIONAL. ONLY TO RECEIVE DATA

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

void ESP8266_UDP_send_data_ip(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port_remote, uint16_t port_local, uint8_t* data, uint16_t data_len, struct ESP8266_UDP_HANDLE* h)
{
	//SEND THE SPECIFIED DATA OF THE SPECIFIED LENGTH TO REMOTE IP: PORT THROUGH THE LOCAL PORT PASSED AS AN ARGUMENT
	//SPECIFIED IN THE UDP HANDLE STRUCTURE
	//THIS FUNCTION IS UNIDIRECTIONAL - ONLY TO SEND DATA
	//UDP OBJECT DELETED AFTER JOB DONE

	h->esp_connection_variable.type = ESPCONN_UDP;
	h->esp_connection_variable.state = ESPCONN_NONE;
	h->esp_connection_variable.proto.udp = &(h->esp_udp_variable);
	IP4_ADDR((ip_addr_t*)h->esp_connection_variable.proto.udp->remote_ip, ip1, ip2, ip3, ip4);
	h->esp_connection_variable.proto.udp->local_port = port_local;
	h->esp_connection_variable.proto.udp->remote_port = port_remote;

	espconn_create(&(h->esp_connection_variable));
	espconn_send(&(h->esp_connection_variable), data, data_len);
	espconn_delete(&(h->esp_connection_variable));
	os_free(h);
}

void ESP8266_UDP_send_receive_data_ip(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port_remote, uint16_t port_local, uint8_t* data, uint16_t data_len, espconn_recv_callback callback, struct ESP8266_UDP_HANDLE* h)
{
	//SEND THE SPECIFIED DATA OF THE SPECIFIED LENGTH TO REMOTE IP: PORT THROUGH THE LOCAL PORT PASSED AS AN ARGUMENT
	//SPECIFIED IN THE UDP HANDLE STRUCTURE. THEN ALSO REGISTER A CALLBACK FUNCTION FOR THE REPLY ON THE SAME LOCAL PORT
	//AS USED FOR SENDING
	//THIS IS FOR BIDIRECTIONAL UDP
	//UDP OBJCT NOT DELETED AFTER JOB DONE

	h->esp_connection_variable.type = ESPCONN_UDP;
	h->esp_connection_variable.state = ESPCONN_NONE;
	h->esp_connection_variable.proto.udp = &(h->esp_udp_variable);
	IP4_ADDR((ip_addr_t*)h->esp_connection_variable.proto.udp->remote_ip, ip1, ip2, ip3, ip4);
	h->esp_connection_variable.proto.udp->local_port = port_local;
	h->esp_connection_variable.proto.udp->remote_port = port_remote;

	espconn_create(&(h->esp_connection_variable));

	//REGISTER CALLBACK FUNCTION
	espconn_regist_recvcb(&(h->esp_connection_variable), callback);

	espconn_send(&(h->esp_connection_variable), data, data_len);
}

void ESP8266_UDP_delete(struct ESP8266_UDP_HANDLE* handle)
{
	//DELETE THE SPECIFIED UDP HANDLE (CONTAINING THE
	//ESP8266 UDP CONNECTION VARIABLE

	espconn_delete(&(handle->esp_connection_variable));
	os_free(handle);
}
