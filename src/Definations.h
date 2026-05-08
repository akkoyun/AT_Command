/* *******************************************************************************
 *  Copyright (C) 2014-2026 Mehmet Gunce Akkoyun Can not be copied and/or
 *	distributed without the express permission of Mehmet Gunce Akkoyun.
 *
 *	Library				: AT_Command
 *	Version				: 02.00.01
 *
 *********************************************************************************/

#pragma once

// Include Arduino Library
#ifndef Arduino_h
	#include <Arduino.h>
#endif

// Function Mode Definitions
// -------------------------
constexpr bool _AT_SET_  = true;
constexpr bool _AT_GET_  = false;

// Download Mode Definitions
// -------------------------
constexpr bool _FOTA_GET_ = true;
constexpr bool _FOTA_FTP_ = false;

// Manufacturer Definitions
// ------------------------
constexpr uint8_t _MODEM_MANUFACTURER_UNKNOWN_ = 0;
constexpr uint8_t _MODEM_MANUFACTURER_TELIT_   = 1;

// Telit Model Definitions
// -----------------------
constexpr uint8_t _MODEM_MODEL_UNKNOWN_       = 0;
constexpr uint8_t _MODEM_MODEL_GE910_QUAD_    = 11;
constexpr uint8_t _MODEM_MODEL_GE910_QUAD_V3_ = 12;
constexpr uint8_t _MODEM_MODEL_GE910_GNSS_    = 13;
constexpr uint8_t _MODEM_MODEL_LE910S1_EA_    = 21;
constexpr uint8_t _MODEM_MODEL_LE910S1_EAG_   = 22;
constexpr uint8_t _MODEM_MODEL_LE910R1_EU_    = 23;
constexpr uint8_t _MODEM_MODEL_LE910C1_EUX_   = 24;

// Modem Status Definitions
// ------------------------
constexpr bool _MODEM_INITIALIZED_     = true;
constexpr bool _MODEM_NOT_INITIALIZED_ = false;
constexpr bool _MODEM_CONNECTED_       = true;
constexpr bool _MODEM_NOT_CONNECTED_   = false;

// SIM PIN Definitions
// -------------------
constexpr uint8_t _SIM_UNKNOWN_ = 0;
constexpr uint8_t _SIM_READY_   = 1;
constexpr uint8_t _SIM_PIN_     = 2;
constexpr uint8_t _SIM_PUK_     = 3;

// SIM Status Definitions
// ----------------------
constexpr bool _SIM_NOT_INSERTED_ = false;
constexpr bool _SIM_INSERTED_     = true;

// Socket Status Definitions
// -------------------------
constexpr uint8_t _SOCKET_CLOSED_    = 0;
constexpr uint8_t _SOCKET_ACTIVE_    = 1;
constexpr uint8_t _SOCKET_SUSPENDED_ = 2;
constexpr uint8_t _SOCKET_PENDING_   = 3;
constexpr uint8_t _SOCKET_LISTENING_ = 4;
constexpr uint8_t _SOCKET_WAITING_   = 5;
constexpr uint8_t _SOCKET_PROCESS_   = 6;

// AT Command Response Definitions
// --------------------------------
constexpr uint8_t _AT_TIMEOUT_   = 0;
constexpr uint8_t _AT_OK_        = 1;
constexpr uint8_t _AT_ERROR_     = 2;
constexpr uint8_t _AT_CME_       = 3;
constexpr uint8_t _AT_SD_PROMPT_ = 4;
constexpr uint8_t _AT_SRING_     = 5;

// Connection Protocol Definitions
// --------------------------------
constexpr uint8_t _AT_TCP_ = 0;
constexpr uint8_t _AT_UDP_ = 1;

// Connection Mode Definitions
// ---------------------------
constexpr uint8_t _CONNECTION_ONLINE_       = 0;
constexpr uint8_t _CONNECTION_COMMAND_      = 1;
constexpr uint8_t _CONNECTION_AUTO_CLOSE_   = 0;
constexpr uint8_t _CONNECTION_MANUAL_CLOSE_ = 255;

// Connection Status Definitions
// -----------------------------
constexpr uint8_t _CONNECTION_UNKNOWN_ = 0;
constexpr uint8_t _CONNECTION_2G_      = 1;
constexpr uint8_t _CONNECTION_3G_      = 2;
constexpr uint8_t _CONNECTION_4G_      = 3;
constexpr uint8_t _CONNECTION_TDSCDMA_ = 4;

// HTTP Method Definitions
// -----------------------
constexpr uint8_t _HTTP_RESPONSE_ = 0;
constexpr uint8_t _HTTP_GET_      = 1;
constexpr uint8_t _HTTP_POST_     = 2;
constexpr uint8_t _HTTP_PUT_      = 3;
constexpr uint8_t _HTTP_DELETE_   = 4;

// HTTP Status Code Definitions
// ----------------------------
constexpr uint16_t _HTTP_Unknown_          = 0;
constexpr uint16_t _HTTP_OK_               = 200;
constexpr uint16_t _HTTP_Created_          = 201;
constexpr uint16_t _HTTP_Accepted_         = 202;
constexpr uint16_t _HTTP_NoContent_        = 204;
constexpr uint16_t _HTTP_BadRequest_       = 400;
constexpr uint16_t _HTTP_Unauthorized_     = 401;
constexpr uint16_t _HTTP_Forbidden_        = 403;
constexpr uint16_t _HTTP_NotFound_         = 404;
constexpr uint16_t _HTTP_MethodNotAllowed_ = 405;
constexpr uint16_t _HTTP_NotAcceptable_    = 406;
constexpr uint16_t _HTTP_RequestTimeout_   = 408;

// FOTA Status Definitions
// -----------------------
constexpr uint8_t FOTA_Download_Unknown    = 0;
constexpr uint8_t FOTA_Download_OK         = 1;
constexpr uint8_t FOTA_Download_TimeOut    = 20;
constexpr uint8_t FOTA_Download_Size_Error = 21;
constexpr uint8_t FOTA_Download_Not_Save   = 22;
constexpr uint8_t FOTA_Server_Error        = 23;
constexpr uint8_t FOTA_Download_CME_Error  = 24;
constexpr uint8_t FOTA_SD_Error            = 30;
constexpr uint8_t FOTA_FTP_Login_Error     = 50;
constexpr uint8_t FOTA_FTP_Connect_Error   = 51;
constexpr uint8_t FOTA_FTP_Config_Error    = 52;
constexpr uint8_t FOTA_FTP_File_Not_Found  = 53;
constexpr uint8_t FOTA_FTP_File_Size_Error = 54;
constexpr uint8_t FOTA_Download_MD5_Error  = 99;
