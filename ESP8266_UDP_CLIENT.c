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

//LOCAL LIBRARY VARIABLES/////////////////////////////////////
//DEBUG RELATED
static uint8_t _esp8266_udp_client_debug;

//UDP RELATED
static struct espconn _esp8266_udp_client_espconn;
static struct esp_udp _esp8266_udp_client_user_udp;

//IP / HOSTNAME RELATED
static const char* _esp8266_udp_client_host_name;
static const char* _esp8266_udp_client_host_ip;
static ip_addr_t _esp8266_udp_client_resolved_host_ip;
static uint16_t _esp8266_udp_client_host_port;
static uint16_t _esp8266_udp_client_local_port;

//TIMER RELATED
static volatile os_timer_t _esp8266_udp_client_dns_timer;

//COUNTERS
static uint16_t _esp8266_udp_client_dns_retry_count;

//UDP OBJECT STATE
static char* _esp8266_udp_client_data_buffer;
static ESP8266_UDP_CLIENT_STATE _esp8266_udp_client_state;

//CALLBACK FUNCTION VARIABLES
static void (*_esp8266_udp_client_dns_cb_function)(ip_addr_t*);
static void (*_esp8266_udp_client_udp_recv_cb)(void*, char*, unsigned short);
static void (*_esp8266_udp_client_udp_user_data_ready_cb)(char*, uint16_t);
//END LOCAL LIBRARY VARIABLES/////////////////////////////////


void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_SetDebug(uint8_t debug_on)
{
    //SET DEBUG PRINTF ON(1) OR OFF(0)
    
    _esp8266_udp_client_debug = debug_on;
}

void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_Initialize(const char* hostname,
													const char* host_ip,
													uint16_t host_port)
{
    //INITIALIZE UDP CONNECTION PARAMETERS
	//HOSTNAME (RESOLVED THROUGH DNS IF HOST IP = NULL)
	//HOST IP
	//HOST PORT
    
    _esp8266_udp_client_host_name = hostname;
    _esp8266_udp_client_host_ip = host_ip;
    _esp8266_udp_client_host_port = host_port;
    
    _esp8266_udp_client_dns_retry_count = 0;

	_esp8266_udp_client_state = ESP8266_UDP_CLIENT_STATE_OK;
	
	//SET DEBUG ON
	_esp8266_udp_client_debug = 1;
	return;
}

void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_SetDnsServer(char num_dns, ip_addr_t* dns)
{
    //SET DNS SERVER RESOLVE HOSTNAME TO IP ADDRESS
	//MAX OF 2 DNS SERVER SUPPORTED (num_dns)
    
    if(num_dns == 1 || num_dns == 2)
	{
		espconn_dns_setserver(num_dns, dns);
	}
	return;
}

void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_SetCallbackFunctions(void (*user_data_ready_cb)(void*, char*, unsigned short))
{
    //HOOK FOR THE USER TO PROVIDE CALLBACK FUNCTIONS FOR
	//VARIOUS INTERNAL UDP OPERATION
	//SET THE CALLBACK FUNCTIONS FOR THE EVENTS:
	//  (1) UDP USER DATA READY
	
	//UDP DATA READY USER CB
	_esp8266_udp_client_udp_user_data_ready_cb = user_data_ready_cb;
}

const char* ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_GetSourceHost(void)
{
    //RETURN HOST NAME STRING
    
    return _esp8266_udp_client_host_name;
}

uint16_t ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_GetRemotePort(void)
{
    //RETURN HOST REMOTE PORT
    
    return _esp8266_udp_client_host_port;
}

uint16_t ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_GetLocalPort(void)
{
    //RETURN THE LOCAL PORT USED BY LIBRARY TO DO UDP COMMUNICATION
    
    return _esp8266_udp_client_local_port;
}

ESP8266_UDP_CLIENT_STATE ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_GetState(void)
{
    //RETURN THE INTERNAL ESP8266 UDP STATE VARIABLE VALUE
    
    return _esp8266_udp_client_state;
}

void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_ResolveHostName(void (*user_dns_cb_fn)(ip_addr_t*))
{
    //RESOLVE PROVIDED HOSTNAME USING THE SUPPLIED DNS SERVER
	//AND CALL THE USER PROVIDED DNS DONE CB FUNCTION WHEN DONE

	//DONE ONLY IF THE HOSTNAME SUPPLIED IN INITIALIZATION FUNCTION
	//IS NOT NULL. IF NULL, USER SUPPLIED IP ADDRESS IS USED INSTEAD
	//AND NO DNS REOSLUTION IS DONE
	
	//SET USER DNS RESOLVE CB FUNCTION
	_esp8266_udp_client__dns_cb_function = user_dns_cb_fn;

	//SET DNS RETRY COUNTER TO ZERO
	_esp8266_udp_client_dns_retry_count = 0;
	
	if(_esp8266_udp_client_host_name != NULL)
	{
		//NEED TO DO DNS RESOLUTION

		//START THE DNS RESOLVING PROCESS AND TIMER
		struct espconn temp;
		_esp8266_udp_client_resolved_host_ip.addr = 0;
		espconn_gethostbyname(&temp, _esp8266_udp_client_host_name, &_esp8266_udp_client_resolved_host_ip, _esp8266_udp_client_dns_found_cb);
		os_timer_setfn(&_esp8266_udp_cleint_dns_timer, (os_timer_func_t*)_esp8266_udp_client_dns_timer_cb, &temp);
		os_timer_arm(&_esp8266_udp_client_dns_timer, 1000, 0);
		return;
	}
	
	//NO NEED TO DO DNS RESOLUTION. USE USER SUPPLIED IP ADDRESS STRING
	_esp8266_udp_client_resolved_host_ip.addr = ipaddr_addr(_esp8266_udp_client_host_ip);
	
	_esp8266_udp_client_state = ESP8266_UDP_CLIENT_STATE_DNS_RESOLVED;
	
	//CALL USER SUPPLIED DNS RESOLVE CB FUNCTION
	(*_esp8266_udp_client_dns_cb_function)(&_esp8266_udp_client_resolved_host_ip);
}

void ICACHE_FLASH_ATTR ESP8266_UDP_CLIENT_SendData(uint8_t* data, uint16_t data_len)
{
    //FORM A UDP CONNECTION
    //REGISTER RECEIVE CALLBACK (LIBRARY INTERNAL)
    //SEND USER SUPPLIED DATA THROUGH THE CONNECTION
    //DELETE UDP CONNECTION
    
    _esp8266_udp_client_espconn.type = ESPCONN_UDP;
	_esp8266_udp_client_espconn.state = ESPCONN_NONE;
	_esp8266_udp_client_user_udp.local_port = espconn_port();
	_esp8266_udp_client_user_udp.remote_port = _esp8266_udp_client_host_port;
    _esp8266_udp_client_espconn.proto.udp = &_esp8266_udp_client_user_udp;
    
    //STORE THE LOCAL PORT USED FOR THIS TRANSACTION
    _esp8266_udp_client_local_port = _esp8266_udp_client_user_udp.local_port;
    
    //CREATE UDP CONNECTION
    espconn_create(&_esp8266_udp_client_espconn);
    
    //REGISTER RECEIVE CALLBACK FUNCTION
    espconn_regist_recvcb(&_esp8266_udp_client_espconn, _esp8266_udp_client_udp_recv_cb);
    
    //SEND DATA
    int8_t error = espconn_send(&_esp8266_udp_client_espconn, data, data_len);
    
    if(_esp8266_udp_client_debug)
    {
        if(error == 0)
        {
	        //UDP SENDING OK
	        os_printf("ESP8266 : UDP : Data Sent : Length = %d, Local Port = %d\n", data_len, _esp8266_udp_client_local_port);
        }
        else
        {
            //UDP SENDING ERROR
            os_printf("ESP8266 : UDP : Data Sent Error: Code = %d\n", error);
        }
    }
}

void ICACHE_FLASH_ATTR _esp8266_udp_client_dns_timer_cb(void* arg)
{
	//ESP8266 DNS CHECK TIMER CALLBACK FUNCTIONS
	//TIME PERIOD = 1 SEC

	//DNS TIMER CB CALLED IE. DNS RESOLUTION DID NOT WORK
	//DO ANOTHER DNS CALL AND RE-ARM THE TIMER

	_esp8266_udp_client_dns_retry_count++;
	if(_esp8266_udp_client_dns_retry_count == ESP8266_UDP_CLIENT_DNS_MAX_TRIES)
	{
		//NO MORE DNS TRIES TO BE DONE
		//STOP THE DNS TIMER
		os_timer_disarm(&_esp8266_udp_client_dns_timer);

		if(_esp8266_udp_client_debug)
		{
		    os_printf("DNS Max retry exceeded. DNS unsuccessfull\n");
		}

		_esp8266_udp_client_state = ESP8266_UDP_CLIENT_STATE_ERROR;
		//CALL USER DNS CB FUNCTION WILL NULL ARGUMENT)
		if(*_esp8266_udp_client_dns_cb_function != NULL)
		{
			(*_esp8266_udp_client_dns_cb_function)(NULL);
		}
		return;
	}

	if(_esp8266_udp_client_debug)
	{
	    os_printf("DNS resolve timer expired. Starting another timer of 1 second...\n");
	}

	struct espconn *pespconn = arg;
	espconn_gethostbyname(pespconn, _esp8266_udp_client_host_name, &_esp8266_udp_client_resolved_host_ip, _esp8266_udp_client_dns_found_cb);
	os_timer_arm(&_esp8266_udp_client_dns_timer, 1000, 0);
}

void ICACHE_FLASH_ATTR _esp8266_udp_client_dns_found_cb(const char* name, ip_addr_t* ipAddr, void* arg)
{
	//ESP8266 UDP DNS RESOLVING DONE CALLBACK FUNCTION

	//DISABLE THE DNS TIMER
	os_timer_disarm(&_esp8266_udp_client_dns_timer);

	if(ipAddr == NULL)
	{
		//HOST NAME COULD NOT BE RESOLVED
		if(_esp8266_udp_client_debug)
		{
		    os_printf("hostname : %s, could not be resolved\n", _esp8266_udp_client_host_name);
		}

		_esp8266_udp_client_state = ESP8266_UDP_CLIENT_STATE_ERROR;
		
		//CALL USER PROVIDED DNS CB FUNCTION WITH NULL PARAMETER
		if(*_esp8266_udp_client_dns_cb_function != NULL)
		{
			(*_esp8266_udp_client_dns_cb_function)(NULL);
		}
		return;
	}

	//DNS GOT IP
	_esp8266_udp_client_resolved_host_ip.addr = ipAddr->addr;
	if(_esp8266_udp_client_debug)
	{
	    os_printf("hostname : %s, resolved. IP = %d.%d.%d.%d\n", _esp8266_udp_client_host_name,
																    *((uint8_t*)&_esp8266_udp_client_resolved_host_ip.addr),
																    *((uint8_t*)&_esp8266_udp_client_resolved_host_ip.addr + 1),
																    *((uint8_t*)&_esp8266_udp_client_resolved_host_ip.addr + 2),
																    *((uint8_t*)&_esp8266_udp_client_resolved_host_ip.addr + 3));
	}

	_esp8266_udp_client_state = ESP8266_UDP_CLIENT_STATE_DNS_RESOLVED;

	//CALL USER PROVIDED DNS CB FUNCTION WITH RESOLVED IP AS ARGUMENT
	if(*_esp8266_udp_client_dns_cb_function != NULL)
	{
		(*_esp8266_udp_client_dns_cb_function)(&_esp8266_udp_client_resolved_host_ip);
	}
}

void ICACHE_FLASH_ATTR _esp8266_udp_client_udp_recv_cb(void* arg, char* pusrdata, unsigned short length)
{
    //INTERNAL UDP DATA RECEIVED CB
    
    if(_esp8266_udp_client_debug)
    {
	    os_printf("ESP8266 : UDP : DATA RECEIVED : LEN = %d", length);
	}
	
	//CALL USER PROVIDED DATA RECEIVED CB
	if(*_esp8266_udp_client_udp_user_data_ready_cb != NULL)
	{
		(*_esp8266_udp_client_udp_user_data_ready_cb)(arg, pusrdata, length);
	}
}
