// Include Arduino Library
#ifndef Arduino_h
	#include <Arduino.h>
#endif

// Function Mode Definitions
// -------------------------
#define _AT_SET_				    		(bool)true
#define _AT_GET_				    		(bool)false

// Manufacturer Definitions
// -----------------------
#define _MODEM_MANUFACTURER_UNKNOWN_		(uint8_t)0
#define _MODEM_MANUFACTURER_TELIT_			(uint8_t)1

// Telit Model Definitions
// -----------------------
#define _MODEM_MODEL_UNKNOWN_				(uint8_t)0
#define _MODEM_MODEL_GE910_QUAD_			(uint8_t)11
#define _MODEM_MODEL_GE910_QUAD_V3_			(uint8_t)12
#define _MODEM_MODEL_GE910_GNSS_			(uint8_t)13
#define _MODEM_MODEL_LE910S1_EA_			(uint8_t)21
#define _MODEM_MODEL_LE910S1_EAG_			(uint8_t)22
#define _MODEM_MODEL_LE910R1_EU_			(uint8_t)23
#define _MODEM_MODEL_LE910C1_EUX_			(uint8_t)24

// Modem Status Definitions
// ------------------------
#define _MODEM_INITIALIZED_					(bool)true
#define _MODEM_NOT_INITIALIZED_				(bool)false
#define _MODEM_CONNECTED_					(bool)true
#define _MODEM_NOT_CONNECTED_				(bool)false

// SIM PIN Definitions
// -------------------
#define _SIM_UNKNOWN_						(uint8_t)0
#define _SIM_READY_							(uint8_t)1
#define _SIM_PIN_							(uint8_t)2
#define _SIM_PUK_							(uint8_t)3

// SIM Status Definitions
// -----------------------
#define _SIM_NOT_INSERTED_					(bool)false
#define _SIM_INSERTED_						(bool)true

// Socket Status Definitions
// --------------------------
#define _SOCKET_CLOSED_						(uint8_t)0
#define _SOCKET_ACTIVE_						(uint8_t)1
#define _SOCKET_SUSPENDED_					(uint8_t)2
#define _SOCKET_PENDING_					(uint8_t)3
#define _SOCKET_LISTENING_					(uint8_t)4
#define _SOCKET_WAITING_					(uint8_t)5
#define _SOCKET_PROCESS_					(uint8_t)6

// AT Command Output Definitions
// -----------------------------
#define _AT_TIMEOUT_				    	(uint8_t)0
#define _AT_OK_			    		    	(uint8_t)1
#define _AT_ERROR_					    	(uint8_t)2
#define _AT_CME_					    	(uint8_t)3
#define _AT_SD_PROMPT_                  	(uint8_t)4
#define _AT_SRING_                  		(uint8_t)5

// Define Connection Protocol
// --------------------------
#define _AT_TCP_							(uint8_t)0
#define _AT_UDP_							(uint8_t)1

// Define Connection Mode
// ----------------------
#define _CONNECTION_ONLINE_					(uint8_t)0
#define _CONNECTION_COMMAND_				(uint8_t)1
#define _CONNECTION_AUTO_CLOSE_				(uint8_t)0
#define _CONNECTION_MANUAL_CLOSE_			(uint8_t)255

// Define Connection Status
// ------------------------
#define _CONNECTION_UNKNOWN_				(uint8_t)0
#define _CONNECTION_2G_						(uint8_t)1
#define _CONNECTION_3G_						(uint8_t)2
#define _CONNECTION_4G_						(uint8_t)3
#define _CONNECTION_TDSCDMA_				(uint8_t)4

// Define HTTP Method
// ------------------
#define _HTTP_RESPONSE_			    		(uint8_t)0
#define _HTTP_GET_				    		(uint8_t)1
#define _HTTP_POST_				    		(uint8_t)2
#define _HTTP_PUT_				    		(uint8_t)3
#define _HTTP_DELETE_			    		(uint8_t)4

// Define HTTP Headers
// -------------------
#define _HTTP_Unknown_			    		(uint16_t)0
#define _HTTP_OK_				    		(uint16_t)200
#define _HTTP_Created_			    		(uint16_t)201
#define _HTTP_Accepted_			    		(uint16_t)202
#define _HTTP_NoContent_		    		(uint16_t)204
#define _HTTP_BadRequest_		    		(uint16_t)400
#define _HTTP_Unauthorized_		    		(uint16_t)401
#define _HTTP_Forbidden_		    		(uint16_t)403
#define _HTTP_NotFound_			    		(uint16_t)404
#define _HTTP_MethodNotAllowed_	    		(uint16_t)405
#define _HTTP_NotAcceptable_	    		(uint16_t)406
#define _HTTP_RequestTimeout_	    		(uint16_t)408

// Define FOTA Status
// ------------------
#define FOTA_Download_OK					(uint8_t)1
#define FOTA_Download_TimeOut				(uint8_t)20
#define FOTA_Download_Size_Error			(uint8_t)21
#define FOTA_Download_Not_Save				(uint8_t)22
#define FOTA_SD_Error						(uint8_t)30
#define FOTA_Download_MD5_Error				(uint8_t)99
#define FOTA_Server_Error					(uint8_t)23
#define FOTA_Download_CME_Error				(uint8_t)24
