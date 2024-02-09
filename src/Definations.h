#ifndef __AT_Command_Definations__
#define __AT_Command_Definations__

	// Include Arduino Library
	#ifndef Arduino_h
		#include <Arduino.h>
	#endif

	// Function Mode Definitions
	// -------------------------
	#define SET						    		(bool)true
	#define GET						    		(bool)false

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

	// AT Command Output Definitions
	// -----------------------------
	#define _AT_TIMEOUT_				    	(uint8_t)0
	#define _AT_OK_			    		    	(uint8_t)1
	#define _AT_ERROR_					    	(uint8_t)2
	#define _AT_CME_					    	(uint8_t)3
	#define _AT_SD_PROMPT_                  	(uint8_t)4

	// Define HTTP Method
	// ------------------
	#define HTTP_RESPONSE			    		(uint8_t)0
	#define HTTP_GET				    		(uint8_t)1
	#define HTTP_POST				    		(uint8_t)2
	#define HTTP_PUT				    		(uint8_t)3
	#define HTTP_DELETE				    		(uint8_t)4

	// Define HTTP Headers
	// -------------------
	#define HTTP_OK					    		(uint16_t)200
	#define HTTP_Accepted			    		(uint16_t)202
	#define HTTP_NoContent			    		(uint16_t)204
	#define HTTP_BadRequest			    		(uint16_t)400
	#define HTTP_NotFound			    		(uint16_t)404
	#define HTTP_MethodNotAllowed	    		(uint16_t)405
	#define HTTP_NotAcceptable		    		(uint16_t)406

	// Define FOTA Status
	// ------------------
	#define FOTA_Download_OK					(uint8_t)1
	#define FOTA_Download_TimeOut				(uint8_t)20
	#define FOTA_Download_Size_Error			(uint8_t)21
	#define FOTA_Download_Not_Save				(uint8_t)21
	#define FOTA_SD_Error						(uint8_t)30




#endif /* defined(__AT_Command_Definations__) */