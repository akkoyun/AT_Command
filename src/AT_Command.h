/* *******************************************************************************
 *  Copyright (C) 2014-2026 Mehmet Gunce Akkoyun Can not be copied and/or
 *	distributed without the express permission of Mehmet Gunce Akkoyun.
 *
 *	Library				: AT_Command
 *	Code Developer		: Mehmet Gunce Akkoyun (akkoyun@me.com)
 *	Version				: 02.01.00
 *
 *********************************************************************************/

#pragma once

	// Include Arduino Library
	#ifndef Arduino_h
		#include <Arduino.h>
	#endif

	// Include Definations
	#include "Definations.h"
	#include "Config.h"

	// Telit LE910C1_EUX Class
	#define _LE910C1_EUX_
	class LE910C1_EUX {

		// Private Context
		private:

			// GSM Serial Stream Definition
			Stream * GSM_Serial;

			// Shared IO buffer — single allocation, reused by every AT command function.
			// Moves all large per-call stack allocations to the object instance instead.
			static constexpr uint16_t _IO_Buffer_Size = 1024;
			char _IO_Buffer[_IO_Buffer_Size];

			// Serial Buffer Variable Structure Definition
			struct Serial_Buffer {
				uint8_t 			Response;
				uint16_t 			Read_Order;
				uint16_t 			Data_Order;
				const uint16_t 		Time_Out;
				const uint16_t 		Size;
			};

			// Clear Serial Buffer Function
			void Clear_UART_Buffer(void) {

				// Clear UART Buffer
				while (GSM_Serial->available()) {

					// Read GSM Buffer
					GSM_Serial->read();

					// Read Delay
					delay(1);

				}

			}

			// Read Serial Buffer Function
			bool Read_UART_Buffer(Serial_Buffer * _Buffer) {

				// Response Wait Delay
				delay(_AT_UART_READ_DELAY_);

				// Reset state and clear shared buffer
				_Buffer->Read_Order = 0;
				_Buffer->Response = _AT_TIMEOUT_;
				const uint16_t _Limit = (_Buffer->Size < _IO_Buffer_Size) ? _Buffer->Size : _IO_Buffer_Size;
				memset(_IO_Buffer, '\0', _Limit);

				// Read Current Time
				const uint32_t _Current_Time = millis();

				// Read UART Response
				while (!_Buffer->Response) {

					// Read Serial Char
					_IO_Buffer[_Buffer->Read_Order] = GSM_Serial->read();

					// Control for Response
					if (this->Find(_AT_OK_,        _IO_Buffer, _Buffer->Read_Order)) _Buffer->Response = _AT_OK_;
					if (this->Find(_AT_ERROR_,     _IO_Buffer, _Buffer->Read_Order)) _Buffer->Response = _AT_ERROR_;
					if (this->Find(_AT_CME_,       _IO_Buffer, _Buffer->Read_Order)) _Buffer->Response = _AT_CME_;
					if (this->Find(_AT_SD_PROMPT_, _IO_Buffer, _Buffer->Read_Order)) _Buffer->Response = _AT_SD_PROMPT_;

					// Advance position only while no response detected
					// (preserves Read_Order at the last matched byte index)
					if (!_Buffer->Response) {
						if (isAscii(_IO_Buffer[_Buffer->Read_Order])) _Buffer->Read_Order++;
						if (_IO_Buffer[_Buffer->Read_Order] == '\r') _Buffer->Read_Order++;
						if (_IO_Buffer[_Buffer->Read_Order] == '\n') _Buffer->Read_Order++;

						// Prevent buffer overflow
						if (_Buffer->Read_Order >= _Limit - 1) break;
					}

					// Handle for timeout
					if (millis() - _Current_Time >= _Buffer->Time_Out) break;

				}

				// Control for Response
				return (_Buffer->Response != _AT_TIMEOUT_);

			}

			// Binary-safe UART buffer read — advances Read_Order for every byte (no ASCII filter).
			// Required for WebSocket frame headers that contain non-ASCII byte values.
			bool Read_UART_Buffer_Raw(Serial_Buffer * _Buffer) {

				// Response Wait Delay
				delay(_AT_UART_READ_DELAY_);

				// Reset state and clear shared buffer
				_Buffer->Read_Order = 0;
				_Buffer->Response = _AT_TIMEOUT_;
				const uint16_t _Limit = (_Buffer->Size < _IO_Buffer_Size) ? _Buffer->Size : _IO_Buffer_Size;
				memset(_IO_Buffer, '\0', _Limit);

				// Read Current Time
				const uint32_t _Current_Time = millis();

				// Read UART Response
				while (!_Buffer->Response) {

					// Read Serial Char
					_IO_Buffer[_Buffer->Read_Order] = GSM_Serial->read();

					// Control for Response
					if (this->Find(_AT_OK_,    _IO_Buffer, _Buffer->Read_Order)) _Buffer->Response = _AT_OK_;
					if (this->Find(_AT_ERROR_, _IO_Buffer, _Buffer->Read_Order)) _Buffer->Response = _AT_ERROR_;
					if (this->Find(_AT_CME_,   _IO_Buffer, _Buffer->Read_Order)) _Buffer->Response = _AT_CME_;

					// Advance for every byte regardless of value
					if (!_Buffer->Response) {
						_Buffer->Read_Order++;
						if (_Buffer->Read_Order >= _Limit - 1) break;
					}

					// Handle for timeout
					if (millis() - _Current_Time >= _Buffer->Time_Out) break;

				}

				// Control for Response
				return (_Buffer->Response != _AT_TIMEOUT_);

			}

			// OK Find Function
			bool Find(const uint8_t _Type, const char * _Buffer, uint16_t _Size) {

				// Select Find Type
				if (_Type == _AT_OK_ && _Size > 4) {

					// \r\nOK\r\n
					if (
						(_Buffer[_Size - 5] == '\r') &&
						(_Buffer[_Size - 4] == '\n') &&
						(_Buffer[_Size - 3] == 'O' ) &&
						(_Buffer[_Size - 2] == 'K' ) &&
						(_Buffer[_Size - 1] == '\r') &&
						(_Buffer[_Size    ] == '\n')
					) return(true);

				} else if (_Type == _AT_ERROR_ && _Size > 7) {

					// \r\nERROR\r\n
					if (
						(_Buffer[_Size - 8] == '\r') &&
						(_Buffer[_Size - 7] == '\n') &&
						(_Buffer[_Size - 6] == 'E' ) &&
						(_Buffer[_Size - 5] == 'R' ) &&
						(_Buffer[_Size - 4] == 'R' ) &&
						(_Buffer[_Size - 3] == 'O' ) &&
						(_Buffer[_Size - 2] == 'R' ) &&
						(_Buffer[_Size - 1] == '\r') &&
						(_Buffer[_Size    ] == '\n')
					) return(true);

				} else if (_Type == _AT_CME_ && _Size > 15) {

					// \r\n+CME ERROR: nnn\r\n
					if (
						(_Buffer[_Size - 18] == '\r') &&
						(_Buffer[_Size - 17] == '\n') &&
						(_Buffer[_Size - 16] == '+' ) &&
						(_Buffer[_Size - 15] == 'C' ) &&
						(_Buffer[_Size - 14] == 'M' ) &&
						(_Buffer[_Size - 13] == 'E' ) &&
						(_Buffer[_Size - 1 ] == '\r') &&
						(_Buffer[_Size     ] == '\n')
					) return(true);

				} else if (_Type == _AT_SD_PROMPT_ && _Size > 2) {

					// \r\n> (space)
					if (
						(_Buffer[_Size - 3] == '\r') &&
						(_Buffer[_Size - 2] == '\n') &&
						(_Buffer[_Size - 1] == '>' ) &&
						(_Buffer[_Size    ] == ' ' )
					) return(true);

				} 

				// End Function
				return(false);

			}

			// Get Parsed Number Function
			uint32_t Handle_Number(const char * _Buffer, const char _Start_Char, const uint8_t _Start_Times, const char _End_Char, const uint8_t _End_Times) {

				uint16_t _Start = this->Find_Char(_Buffer, _Start_Char, _Start_Times);
				uint16_t _End   = this->Find_Char(_Buffer, _End_Char,   _End_Times);

				if (_Start == UINT16_MAX || _End == UINT16_MAX || _End <= _Start) return 0;
				uint16_t _Size = _End - _Start - 1;
				if (_Size == 0 || _Size > 31) return 0;

				char _Temp_Buffer[32];
				memset(_Temp_Buffer, '\0', 32);
				memcpy(_Temp_Buffer, &_Buffer[_Start + 1], _Size);

				return (uint32_t)strtoul(_Temp_Buffer, NULL, 10);

			}

			// Get Parsed HEX Function
			uint32_t Handle_HEX(const char * _Buffer, const char _Start_Char, const uint8_t _Start_Times, const char _End_Char, const uint8_t _End_Times) {

				uint16_t _Start = this->Find_Char(_Buffer, _Start_Char, _Start_Times);
				uint16_t _End   = this->Find_Char(_Buffer, _End_Char,   _End_Times);

				if (_Start == UINT16_MAX || _End == UINT16_MAX || _End <= _Start) return 0;
				uint16_t _Size = _End - _Start - 1;
				if (_Size == 0 || _Size > 31) return 0;

				char _Temp_Buffer[32];
				memset(_Temp_Buffer, '\0', 32);
				memcpy(_Temp_Buffer, &_Buffer[_Start + 1], _Size);

				return (uint32_t)strtoul(_Temp_Buffer, NULL, 16);

			}

			// ASCII Find Function — returns UINT16_MAX if not found
			uint16_t Find_Char(const char * _Buffer, const char _Char, const uint8_t _Count) {

				uint16_t _Counter = 0;

				for (uint16_t i = 0; _Buffer[i] != '\0'; ++i) {

					if (_Buffer[i] == _Char) {
						_Counter++;
						if (_Counter == _Count) return i;
					}

				}

				return UINT16_MAX;

			}

			// RSSI to Signal Quality Function (returns 0 for invalid/unknown)
			uint8_t RSSI_to_Signal_Quality(const int8_t _RSSI) {

				if (_RSSI >= 0)  return 0;  // invalid / not known
				if (_RSSI >= -65) return 5;
				if (_RSSI >= -75) return 4;
				if (_RSSI >= -85) return 3;
				if (_RSSI >= -95) return 2;
				return 1;

			}

		// Protected Context 
		protected:

			// Callback Function Definition
			typedef void (*Callback_JSON_Parse)(uint8_t);

			// AT Command
			bool AT(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_AT_, 11};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Set Connection Mode Function
			bool FCLASS(const uint8_t _FCLASS = 0) {

				// Control for Parameter
				if (_FCLASS != 0 && _FCLASS != 8) return(false);

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+FCLASS="));
				GSM_Serial->print(_FCLASS);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_FCLASS_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Get Manufacturer Function
			bool CGMI(uint8_t & _Manufacturer) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CGMI"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CGMI_, 35};

				this->Read_UART_Buffer(&_Buffer);

				// Set No Manufacturer
				_Manufacturer = _MODEM_MANUFACTURER_UNKNOWN_;

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// AT+CGMI\r\n
					// \r\nTelit\r\n\r\nOK\r\n

					// Declare Variable
					char _Manufacturer_Name[10];

					// Clear Variable
					memset(_Manufacturer_Name, '\0', 10);

					// Handle Manufacturer Name
					if (sscanf(_IO_Buffer, "\r\n%9s\r\n\r\nOK\r\n", _Manufacturer_Name) != 1) return false;

					// Control for Manufacturer Name
					if (strstr(_Manufacturer_Name, "Telit") != NULL) {

						// Set Manufacturer
						_Manufacturer = _MODEM_MANUFACTURER_TELIT_;

						// Return Response
						return(true);

					}

				}

				// End Function
				return(false);

			}

			// Get Model Function
			bool CGMM(uint8_t & _Model) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CGMM"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CGMM_, 35};

				this->Read_UART_Buffer(&_Buffer);

				// Set No Model
				_Model = _MODEM_MODEL_UNKNOWN_;

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// Declare Variable
					char _Model_Name[15];

					// Clear Variable
					memset(_Model_Name, '\0', 15);

					// Get Model Name
					if (sscanf(_IO_Buffer, "\r\n%14s\r\n\r\nOK\r\n", _Model_Name) != 1) return false;

					// Check longer names first to avoid substring false-matches
					if      (strstr(_Model_Name, "GE910-QUAD-V3") != NULL) _Model = _MODEM_MODEL_GE910_QUAD_V3_;
					else if (strstr(_Model_Name, "GE910-GNSS")    != NULL) _Model = _MODEM_MODEL_GE910_GNSS_;
					else if (strstr(_Model_Name, "GE910-QUAD")    != NULL) _Model = _MODEM_MODEL_GE910_QUAD_;
					else if (strstr(_Model_Name, "LE910S1-EAG")   != NULL) _Model = _MODEM_MODEL_LE910S1_EAG_;
					else if (strstr(_Model_Name, "LE910S1-EA")    != NULL) _Model = _MODEM_MODEL_LE910S1_EA_;
					else if (strstr(_Model_Name, "LE910R1-EU")    != NULL) _Model = _MODEM_MODEL_LE910R1_EU_;
					else if (strstr(_Model_Name, "LE910C1-EUX")   != NULL) _Model = _MODEM_MODEL_LE910C1_EUX_;
					else                                                    _Model = _MODEM_MODEL_UNKNOWN_;

					// Return Response
					if (_Model != _MODEM_MODEL_UNKNOWN_) return(true);

				}

				// End Function
				return(false);

			}

			// Get Firmware Function
			bool SWPKGV(char * _Firmware) {

				/**
				 * @brief Get Firmware Version of the Modem Function
				 * @author Mehmet Günce Akkoyun
				 * @version 1.0.0
				 * 
				 * --> AT#SWPKGV\r\n [11]
				 * <-- \r\n25.30.226-P0F.225200\r\nM0F.223006\r\nP0F.225200\r\nA0F.223006\r\n\r\nOK\r\n [66]
				 * 
				 */

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SWPKGV"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SWPKGV_, 70};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// Clear Variable
					memset(_Firmware, '\0', 15);

					// Handle Firmware
					strncpy(_Firmware, &_IO_Buffer[2], 9);

					// End Function
					return (true);

				}

				// End Function
				return (false);

			}

			// Get IMEI Function
			bool CGSN(char * _IMEI) {

				/**
				 * @brief Get IMEI Number of the Modem Function
				 * @author Mehmet Günce Akkoyun
				 * @version 1.0.0
				 * 
				 * --> AT+CGSN=1\r\n [11]
				 * <-- \r\n+CGSN: 354485417617003\r\n\r\nOK\r\n [32]
				 * 
				 */

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CGSN=1"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CGSN_, 35};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// Clear Buffer Variable (IMEI = 15 digits + null terminator)
					memset(_IMEI, '\0', 16);

					// Control for Numeric
					for (uint8_t i = 0; i < _Buffer.Size && _Buffer.Data_Order < 15; i++) {

						// Control for Numeric
						if (isdigit(_IO_Buffer[i])) {

							// Set IMEI
							_IMEI[_Buffer.Data_Order++] = _IO_Buffer[i];

						}

					}

					// End Function
					return (true);

				}

				// End Function
				return (false);

			}

			// Set Power Mode Function
			bool CFUN(const uint8_t _Fun) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CFUN="));
				GSM_Serial->print(_Fun);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CFUN_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// Set CMEE Function
			bool CMEE(const uint8_t _CMEE = 1) {

				// Control for Parameter
				if (_CMEE != 0 && _CMEE != 1 && _CMEE != 2) return(false);

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CMEE="));
				GSM_Serial->print(_CMEE);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CMEE_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// Get Error Code Function
			bool CEER(uint16_t & _Code) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#CEER"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CEER_, 21};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n#CEER: 999\r\n\r\nOK\r\n
					if (_Buffer.Read_Order == 19 && isdigit(_IO_Buffer[9]) && isdigit(_IO_Buffer[10]) && isdigit(_IO_Buffer[11])) {

						_Code = (uint16_t)((_IO_Buffer[9] - '0') * 100)
						      + (uint16_t)((_IO_Buffer[10] - '0') * 10)
						      + (uint16_t)(_IO_Buffer[11] - '0');
						return (true);

					}

					// \r\n#CEER: 99\r\n\r\nOK\r\n
					else if (_Buffer.Read_Order == 18 && isdigit(_IO_Buffer[9]) && isdigit(_IO_Buffer[10])) {

						_Code = (uint16_t)((_IO_Buffer[9] - '0') * 10)
						      + (uint16_t)(_IO_Buffer[10] - '0');
						return (true);

					}

					// \r\n#CEER: 9\r\n\r\nOK\r\n
					else if (_Buffer.Read_Order == 17 && isdigit(_IO_Buffer[9])) {

						_Code = (uint16_t)(_IO_Buffer[9] - '0');
						return (true);

					}

				}

				// End Function
				return (false);

			}

			// ATE Command
			bool ATE(const bool _ECHO = false) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("ATE"));
				GSM_Serial->print(_ECHO);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_ATE_, 13};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// Flow Control Function
			bool K(const uint8_t _K = 0) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT&K"));
				GSM_Serial->print(_K);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_K_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// Get SIM PIN Status Function
			bool CPIN(uint8_t & _Code) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CPIN?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CPIN_, 30};

				this->Read_UART_Buffer(&_Buffer);

				// Set SIM State
				_Code = _SIM_UNKNOWN_;

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n+CPIN: READY\r\n\r\nOK\r\n

					// Declare Handle Variables
					char _PIN_Response[10];

					// Clear PIN Response
					memset(_PIN_Response, '\0', 10);

					// Handle Variables
					if (sscanf(_IO_Buffer, "\r\n+CPIN: %09s\r\n\r\nOK\r\n", _PIN_Response) != 1) return false;

					// Control for SIM State
					if (strstr(_PIN_Response, "READY")) _Code = _SIM_READY_;
					else if (strstr(_PIN_Response, "SIM PIN")) _Code = _SIM_PIN_;
					else if (strstr(_PIN_Response, "SIM PUK")) _Code = _SIM_PUK_;
					else _Code = _SIM_UNKNOWN_;

					// Handle Status
					return (_Code == _SIM_READY_);

				}

				// End Function
				return(false);

			}

			// Get ICCID Function
			bool CCID(char * _ICCID) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#CCID"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CCID_, 37};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// 8990011936290169339
					// 89 : Industry Identifier
					// 9001 : Country Code
					// 1936290169339 : SIM Serial Number

					// Clear ICCID Variable
					memset(_ICCID, '\0', 21);

					// Handle for Response
					for (uint8_t i = 0; i < _Buffer.Size; i++) {

						// Handle ICCID
						if (isdigit(_IO_Buffer[i])) {

							// Set ICCID Variable
							_ICCID[_Buffer.Data_Order++] = _IO_Buffer[i];

						}

					}

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// SIMDET Function
			bool SIMDET(const bool _Function_Type, uint8_t _Mode, bool & _SIM_in_Pin_State) {

				// SET Function
				if (_Function_Type == _AT_SET_) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT#SIMDET="));
					GSM_Serial->print(_Mode);
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SIMDET_, 23};

					this->Read_UART_Buffer(&_Buffer);

					// Handle for Response
					return (_Buffer.Response == _AT_OK_);

				}

				// GET Function
				if (_Function_Type == _AT_GET_) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT#SIMDET?"));
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SIMDET_, 23};

					this->Read_UART_Buffer(&_Buffer);

					// Clear Variables
					_SIM_in_Pin_State = false;

					// Handle for Response
					if (_Buffer.Response == _AT_OK_) {

						// \r\n#SIMDET: 2,1\r\n\r\nOK\r\n
						// \r\n#SIMDET: 2,1\r\n\r\nOK\r\n

						// Define Handle Variable
						uint8_t _SIMDET_Mode = 0;
						uint8_t _SIMDET_State = 0;

						// Handle Query Answer
						uint8_t _Parsed_Variable = sscanf(_IO_Buffer, "\r\n#SIMDET: %01hhu,%01hhu\r\n\r\nOK\r\n", &_SIMDET_Mode, &_SIMDET_State);

						// Assign SIM State
						if (_Parsed_Variable == 2) {

							// Assign State
							if (_SIMDET_State == 0) _SIM_in_Pin_State = false;
							if (_SIMDET_State == 1) _SIM_in_Pin_State = true;

							// End Function
							return (true);

						}

						// End Function
						return (false);

					}

				}

				// End Function
				return (false);

			}

			// Set GPIO Function
			bool GPIO(const bool _Function_Type = _AT_SET_, const uint8_t _Pin = 1, const uint8_t _Mode = 0, const uint8_t _Direction = 2) {

				// SET Function
				if (_Function_Type == _AT_SET_) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT#GPIO="));
					GSM_Serial->print(_Pin);
					GSM_Serial->print(F(","));
					GSM_Serial->print(_Mode);
					GSM_Serial->print(F(","));
					GSM_Serial->print(_Direction);
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_GPIO_, 7};

					this->Read_UART_Buffer(&_Buffer);

					// Handle for Response
					return (_Buffer.Response == _AT_OK_);

				}

				// End Function
				return(false);

			}

			// Set SLED Function
			bool SLED(const uint8_t _SLED = 2) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SLED="));
				GSM_Serial->print(_SLED);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SLED_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// Execution command saves STAT_LED setting in NVM.
			bool SLEDSAV(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SLEDSAV"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SLEDSAV_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// Set Socket Listen Ring Indicator Function
			bool E2SLRI(const uint16_t _Pulse_Duration = 50) {

				// Control for Parameter
				if (_Pulse_Duration < 50 || _Pulse_Duration > 1150) return(false);

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command (Pulse Duration: 0-65535)
				GSM_Serial->print(F("AT#E2SLRI="));
				GSM_Serial->print(_Pulse_Duration);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_E2SLRI_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// CREG Function
			bool CREG(const bool _Function_Type, uint8_t & _Mode, uint8_t & _Stat) {

				// SET Function
				if (_Function_Type == _AT_SET_) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT+CREG="));
					GSM_Serial->print(_Mode);
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CREG_, 25};

					this->Read_UART_Buffer(&_Buffer);

					// Handle for Response
					return (_Buffer.Response == _AT_OK_);

				}

				// GET Function
				if (_Function_Type == _AT_GET_) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT+CREG?"));
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CREG_, 25};

					this->Read_UART_Buffer(&_Buffer);

					// Handle for Response
					if (_Buffer.Response == _AT_OK_) {

						// Handle Variables
						if (sscanf(_IO_Buffer, "\r\n+CREG: %hhu,%hhu\r\n\r\nOK\r\n", &_Mode, &_Stat) != 2) return false;

						// End Function
						return (true);

					}

				}

				// End Function
				return (false);

			}

			// Set CGDCONT Function
			bool CGDCONT(const uint8_t _Cid, const char * _PDP_Type, const char * _APN) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CGDCONT="));
				GSM_Serial->print(_Cid);
				GSM_Serial->print(F(",\""));
				GSM_Serial->print(_PDP_Type);
				GSM_Serial->print(F("\",\""));
				GSM_Serial->print(_APN);
				GSM_Serial->print(F("\""));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CGDCONT_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// Set SGACT Function
			bool SGACT(const uint8_t _Cid, const bool _Stat, uint8_t _IP_Segment[4]) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SGACT="));
				GSM_Serial->print(_Cid);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Stat);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SGACT_, 35};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n#SGACT: 000.000.000.000\r\n\r\nOK\r\n

					// Handle IP
					uint8_t _Variable_Count = sscanf(_IO_Buffer, "\r\n#SGACT: %03hhu.%03hhu.%03hhu.%03hhu\r\n\r\nOK\r\n", &_IP_Segment[0], &_IP_Segment[1], &_IP_Segment[2], &_IP_Segment[3]);

					// Control for IP
					return (_Variable_Count == 4);

				}

				// End Function
				return (false);

			}

			// WS46 Function
			bool WS46(const bool _Function_Type, uint8_t & _Mode) {

				// GET Function
				if (_Function_Type == _AT_GET_) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT+WS46?"));
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_WS46_, 20};

					this->Read_UART_Buffer(&_Buffer);

					// Handle for Response
					if (_Buffer.Response == _AT_OK_) {

						// Declare Variables
						uint8_t _Connection_Mode = _CONNECTION_UNKNOWN_;

						// Reset Mode
						_Mode = _CONNECTION_UNKNOWN_;

						// Handle Variables
						if (sscanf(_IO_Buffer, "\r\n+WS46: %02hhu\r\n\r\nOK\r\n", &_Connection_Mode) != 1) return false;

						// Map WS46 mode to connection type — 3GPP TS 27.007 §7.2
						// 12=GSM(2G), 22=UTRAN(3G), 25=E-UTRAN(4G), 28=UTRAN+E-UTRAN, 29=GERAN+UTRAN, 30=GERAN+E-UTRAN, 31=GERAN+UTRAN+E-UTRAN
						if (_Connection_Mode == 12) _Mode = _CONNECTION_2G_;
						if (_Connection_Mode == 22 || _Connection_Mode == 29) _Mode = _CONNECTION_3G_;
						if (_Connection_Mode == 25 || _Connection_Mode == 28 || _Connection_Mode == 30 || _Connection_Mode == 31) _Mode = _CONNECTION_4G_;

						// Handle Response
						return (_Mode != _CONNECTION_UNKNOWN_);

					}

				}

				// SET Function
				if (_Function_Type == _AT_SET_) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT+WS46="));
					GSM_Serial->print(_Mode);
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_WS46_, 20};

					this->Read_UART_Buffer(&_Buffer);

					// Handle for Response
					return (_Buffer.Response == _AT_OK_);

				}

				// End Function
				return (false);

			}

			// RFSTS Function
			bool RFSTS(const uint8_t _Connection_Type, uint16_t & _MCC, uint16_t & _MNC, uint16_t & _RSSI, uint8_t & _Signal_Level, uint16_t & _TAC, uint32_t & _CID) {

				// Control for Connection Type
				if (_Connection_Type == _CONNECTION_UNKNOWN_) return(false);

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);
				
				// Send UART Command
				GSM_Serial->print(F("AT#RFSTS"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_RFSTS_, 120};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// Control for Connection Type
					if (_Connection_Type == _CONNECTION_2G_ || _Connection_Type == _CONNECTION_3G_) {

						// 2G/3G RFSTS parsing not implemented
						_MCC = 0; _MNC = 0; _RSSI = 0; _Signal_Level = 0; _TAC = 0; _CID = 0;
						return(false);

					} else if (_Connection_Type == _CONNECTION_4G_) {

						// AT#RFSTS\r\n
						// \r\n#RFSTS: "286 01",1795,-101,-67,-15,2242,,128,3,1,0B5D121,"286016339612498","Turkcell",3,3,108\r\n\r\nOK\r\n
						// \r\n#RFSTS: "286 01",1651,-101,-66,-15,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,121\r\n\r\nOK\r\n
						// \r\n#RFSTS: "286 01",1651,-99,-62,-16,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,103\r\n\r\nOK\r\n

						// #RFSTS:<PLMN>,<EARFCN>,<RSRP>,<RSSI>,<RSRQ>,<TAC>,<RAC>,[<TXPWR>],<DR X>, <MM>,<RRC>,<CID>,<IMSI>,[<NetNameAsc>],<SD>,<ABND>,<T3402>,<T3412>,<SI NR>

						// <PLMN> 		- Country code and operator code(MCC, MNC)						+ "286 01"
						// <RSSI> 		- Received Signal Strength Indication							+ -66
						// <TAC> 		- Tracking Area Code											+ 2242
						// <CID> 		- Cell ID														+ 859315

						//              MCC MNC          RSSI     TAC            CID
						//               |   |             |       |              |
						// \r\n#RFSTS: "286 01",1651,-99,-62,-16,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,103\r\n\r\nOK\r\n

						// Read MCC
						_MCC = (uint16_t)this->Handle_Number(_IO_Buffer, '\"', 1, ' ', 2);

						// Read MNC
						_MNC = (uint16_t)this->Handle_Number(_IO_Buffer, ' ', 2, '\"', 2);

						// Read RSSI
						_RSSI = (uint16_t)this->Handle_Number(_IO_Buffer, '-', 2, ',', 4);

						// Calculate Signal Level
						_Signal_Level = 0;
						_Signal_Level = this->RSSI_to_Signal_Quality(_RSSI * -1);

						// Read TAC
						_TAC = (uint16_t)this->Handle_HEX(_IO_Buffer, ',', 5, ',', 6);

						// Read CID
						_CID = (uint32_t)this->Handle_HEX(_IO_Buffer, ',', 10, ',', 11);

						// End Function
						return(true);

					} else {

						// Clear Variables
						_MCC = 0;
						_MNC = 0;
						_RSSI = 0;
						_Signal_Level = 0;
						_TAC = 0;
						_CID = 0;

						// End Function
						return(false);

					}

				}

				// End Function
				return(false);

			}

			// Signal Quality Function
			bool CSQ(uint16_t & _RSSI) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CSQ"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CSQ_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n+CSQ: 9,9\r\n\r\nOK\r\n
					// \r\n+CSQ: 9,99\r\n\r\nOK\r\n
					// \r\n+CSQ: 99,9\r\n\r\nOK\r\n
					// \r\n+CSQ: 99,99\r\n\r\nOK\r\n
					// \r\n+CSQ: 999,9\r\n\r\nOK\r\n
					// \r\n+CSQ: 999,99\r\n\r\nOK\r\n

					// Read CSQ value
					uint8_t _CSQ = this->Handle_Number(_IO_Buffer, ' ', 1, ',', 1);

					// Map CSQ to RSSI (dBm) per 3GPP TS 27.007 §8.5
					// 0→-113, 1→-111, 2..30→-109+(n-2)*2, 31→-51, 99=unknown
					if (_CSQ == 0) _RSSI = 113;
					else if (_CSQ == 1) _RSSI = 111;
					else if (_CSQ <= 30) _RSSI = 109 - (_CSQ * 2);
					else if (_CSQ == 31) _RSSI = 51;
					else if (_CSQ == 99) _RSSI = 0;

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// Socket Configuration Function
			bool SCFG(const uint8_t _Conn_ID, const uint8_t _Cid, const uint16_t _Pkt_Sz, const uint16_t _Max_To, const uint16_t _Conn_To, const uint8_t _TX_To) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SCFG="));
				GSM_Serial->print(_Conn_ID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Cid);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Pkt_Sz);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Max_To);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Conn_To);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_TX_To);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SCFG_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Extended Socket Configuration Function
			bool SCFGEXT(const uint8_t _Conn_ID, const uint8_t _Sr_Mode, const bool _Recv_Data_Mode, const uint8_t _Keep_Alive, const bool _Listen_Auto_Rsp, const bool _Send_Data_Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SCFGEXT="));
				GSM_Serial->print(_Conn_ID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Sr_Mode);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Recv_Data_Mode);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Keep_Alive);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Listen_Auto_Rsp);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Send_Data_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SCFGEXT_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Extended 2 Socket Configuration Function
			bool SCFGEXT2(const uint8_t _Conn_ID, const bool _Buffer_Start, const bool _Abort_Conn_Attempt, const uint8_t _No_Carrier_Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SCFGEXT2="));
				GSM_Serial->print(_Conn_ID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Buffer_Start);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Abort_Conn_Attempt);
				GSM_Serial->print(F(","));
				GSM_Serial->print(0);
				GSM_Serial->print(F(","));
				GSM_Serial->print(0);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_No_Carrier_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SCFGEXT2_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Extended 3 Socket Configuration Function
			bool SCFGEXT3(const uint8_t _Conn_ID, const bool _immRsp, const bool _Closure_Type, const bool _Fast_Sring, const uint8_t _Linger_Time, const uint8_t _UDP_Socket_Mode, const bool _SSend_Timeout) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SCFGEXT3="));
				GSM_Serial->print(_Conn_ID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_immRsp);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Closure_Type);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Fast_Sring);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Linger_Time);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_UDP_Socket_Mode);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_SSend_Timeout);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SCFGEXT3_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Firewall Add/Remove Function
			bool FRWL(const bool _Function_Type, const uint8_t _Action, const char *_IP_Addr) {

				// SET Function
				if (_Function_Type == _AT_SET_) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT#FRWL="));
					GSM_Serial->print(_Action);

					// Print IP Segment
					if (_Action != 2) {
						GSM_Serial->print(F(",\""));
						GSM_Serial->print(_IP_Addr);
						GSM_Serial->print(F("\",\"255.255.255.255\""));
					}
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_FRWL_, 15};

					this->Read_UART_Buffer(&_Buffer);

					// End Function
					return(_Buffer.Response == _AT_OK_);

				}

				// End Function
				return (false);

			}

			// Ping Enable/Disable Function
			bool ICMP(const uint8_t _Mode = 2) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#ICMP="));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_ICMP_, 15};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Ping Function
			bool Ping(const char * _IP, uint16_t & _Time) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#PING=\""));
				GSM_Serial->print(_IP);
				GSM_Serial->print(F("\",1"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_Ping_, 250};

				this->Read_UART_Buffer(&_Buffer);

				// \r\n#PING: 01,"159.89.111.150",0,51\r\n
				// \r\nOK\r\n

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// Clear Time
					_Time = 0;

					// Get Response Time
					_Time = (uint16_t)this->Handle_Number(_IO_Buffer, ',', 2, '\r', 2);

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// Get Clock Function
			bool CCLK(uint8_t & _Year, uint8_t & _Month, uint8_t & _Day, uint8_t & _Hour, uint8_t & _Minute, uint8_t & _Second, uint8_t & _Time_Zone) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CCLK?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CCLK_, 40};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					/*
					--> AT+CCLK?\r\n
					<-- \r\n+CCLK: "YY/MM/DD,HH:MM:SS±ZZ"\r\n\r\nOK\r\n
					     Timezone offset in quarter-hours; sign may be + || -
					*/

					char _tz_sign = '+';
					if (sscanf(_IO_Buffer,
					    "\r\n+CCLK: \"%02hhu/%02hhu/%02hhu,%02hhu:%02hhu:%02hhu%c%02hhu\"\r\n\r\nOK\r\n",
					    &_Year, &_Month, &_Day, &_Hour, &_Minute, &_Second, &_tz_sign, &_Time_Zone) < 6) return false;

					// Encode negative timezone as two's complement in uint8_t
					if (_tz_sign == '-') _Time_Zone = (uint8_t)(0u - _Time_Zone);

					// Validate date/time ranges
					if (_Year > 99 || _Month > 12 || _Month == 0 || _Day > 31 || _Day == 0 ||
					    _Hour > 23 || _Minute > 59 || _Second > 59) return false;

					return (true);

				}

				// End Function
				return (false);

			}

			// Enable or Disable Automatic TimeZone Update Function
			bool CTZU(const bool _State = 1) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CTZU="));
				GSM_Serial->print(_State);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CTZU_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Enable or Disable Network Time UnSolicited Function
			bool NITZ(const uint8_t _Val = 1, const bool _Mode = 1) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#NITZ="));
				GSM_Serial->print(_Val);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_NITZ_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Enable the local time or the UTC time.
			bool CCLKMODE(const bool _Mode = 0) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#CCLKMODE="));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CCLKMODE_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Set Maximum TCP Window Size
			bool TCPMAXWIN(const uint16_t _WinSize = 0, const uint8_t _ScaleFactor = 0) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#TCPMAXWIN="));
				GSM_Serial->print(_WinSize);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_ScaleFactor);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_TCPMAXWIN_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Socket Dial Function
			bool ATSD(const uint8_t _Cid, const bool _Protocol, const char *_IP, const uint8_t _Port, const uint8_t _Closure_Type, uint16_t _IPort, const bool _Conn_Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SD="));
				GSM_Serial->print(_Cid);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Protocol);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Port);
				GSM_Serial->print(F(",\""));
				GSM_Serial->print(_IP);
				GSM_Serial->print(F("\","));
				GSM_Serial->print(_Closure_Type);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_IPort);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Conn_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SD_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Close Socket Function
			bool SH(const uint8_t _ConnID) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SH="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SH_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Socket Listen Function
			bool SL(const uint8_t _ConnID, const bool _Listen_State, const uint16_t _Listen_Port, const uint8_t _Closure_Type) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SL="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Listen_State);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Listen_Port);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Closure_Type);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SL_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Socket Answer Function
			bool SA(const uint8_t _ConnID, const uint8_t _ConnMode, uint16_t & _Length) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SA="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_ConnMode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// \r\nOK\r\n\r\nSRING: 2,86\r\n

				// Handle for Response
				if (this->SRING(_Length)) return(true);

				// End Function
				return(false);

			}

			// Socket Status Function
			bool SS(const uint8_t _ConnID, uint8_t & _State) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SS="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SS_, 60};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n#SS: 2,4,5.26.173.230,80\r\n\r\nOK\r\n
					// \r\n#SS: 2,2,5.26.173.230,80,213.14.250.214,54883\r\n\r\nOK\r\n
					// \r\n#SS: 2,2,000.000.000.000,80,000.000.000.000,54883\r\n\r\nOK\r\n

					// Handle State
					_State = isdigit(_IO_Buffer[9]) ? (_IO_Buffer[9] - '0') : _SOCKET_CLOSED_;

					// End Function
					return (true);

				}

				// End Function
				return(false);

			}

			// ReOpen Socket Function
			bool SO(const uint8_t _ConnID) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SO="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SO_, 20};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Socket Info Function
			bool SI(const uint8_t _ConnID, uint16_t & _Data_Buffer) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SI="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SI_, 50};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n#SI: 2,51,0,13900,0\r\n\r\nOK\r\n

					// Read Data Buffer
					_Data_Buffer = (uint16_t)this->Handle_Number(_IO_Buffer, ',', 3, ',', 4);

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// Socket Inactivity Timeout Function
			bool SKTTO(const uint16_t _TimeOut = 90) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SKTTO="));
				GSM_Serial->print(_TimeOut);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SKTTO_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Socket Pack Send Function
			bool SSEND(Callback_JSON_Parse _Parser, uint8_t _Pack_Type) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SSEND="));
				GSM_Serial->print(_PostMan_CID_);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SSEND_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_SD_PROMPT_) {

					// Send Delay
					delay(_AT_SD_PROMPT_DELAY_);

					// Send Data Pack
					if (_Parser) _Parser(_Pack_Type);

					// Print End Char
					GSM_Serial->print((char)26);

					// Declare Buffer Object
					Serial_Buffer _Buffer_Send = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SSEND_, 7};

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					this->Read_UART_Buffer(&_Buffer_Send);

					// End Function
					return(_Buffer_Send.Response == _AT_OK_);

				}

				// End Function
				return (false);

			}
			bool SSEND(const uint8_t _ConnID, const uint8_t _Method, const char * _Data_Pack) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SSEND="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SSEND_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_SD_PROMPT_) {

					// Send Delay
					delay(_AT_SD_PROMPT_DELAY_);

					// Print Header
					if (_Method == _HTTP_RESPONSE_) {

						// Print Header
						GSM_Serial->print(F("HTTP/1.1 "));
						GSM_Serial->print(F("200"));
						GSM_Serial->print(F(" OK\r\nConnection: close\r\nContent-Type: application/json\r\n\r\n"));

					} else if (_Method == _HTTP_POST_) {

						// Print Header
						GSM_Serial->print(F("POST "));
						GSM_Serial->print(_PostMan_EndPoint_);
						GSM_Serial->print(F(" HTTP/1.1\r\nHost: "));
						GSM_Serial->print(_PostMan_Server_);
						GSM_Serial->print(F("\r\nContent-Length: "));
						GSM_Serial->print(strlen(_Data_Pack));
						GSM_Serial->print(F("\r\nContent-Type: application/json\r\nUser-Agent: PostOffice\r\n\r\n"));

					} else if (_Method == _HTTP_GET_) {

						// Print Header
						GSM_Serial->print(F("GET "));
						GSM_Serial->print(_PostMan_EndPoint_);
						GSM_Serial->print(F(" HTTP/1.1\r\nHost: "));
						GSM_Serial->print(_PostMan_Server_);
						GSM_Serial->print(F("\r\nUser-Agent: PostOffice\r\n\r\n"));

					}

					// Send Data Pack
					GSM_Serial->print(_Data_Pack);

					// Print End Char
					GSM_Serial->print((char)26);

					// Declare Buffer Object
					Serial_Buffer _Buffer_Send = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SSEND_, 7};

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					this->Read_UART_Buffer(&_Buffer_Send);

					// End Function
					return(_Buffer_Send.Response == _AT_OK_);

				}

				// End Function
				return (false);

			}
			bool SSEND(const uint8_t _ConnID, const uint8_t _Method, const char * _Server, const char * _EndPoint, const char * _Data_Pack) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SSEND="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SSEND_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_SD_PROMPT_) {

					// Send Delay
					delay(_AT_SD_PROMPT_DELAY_);

					// Print Header
					if (_Method == _HTTP_RESPONSE_) {

						// Print Header
						GSM_Serial->print(F("HTTP/1.1 200 OK\r\n"));
						GSM_Serial->print(F("Connection: close\r\n"));
						GSM_Serial->print(F("Content-Type: application/json\r\n"));
						GSM_Serial->print(F("\r\n"));

					} else if (_Method == _HTTP_POST_) {

						// Print Header
						GSM_Serial->print(F("POST ")); GSM_Serial->print(_EndPoint); GSM_Serial->print(F(" HTTP/1.1\r\n"));
						GSM_Serial->print(F("Host: ")); GSM_Serial->print(_Server); GSM_Serial->print(F("\r\n"));
						GSM_Serial->print(F("Content-Length: ")); GSM_Serial->print(strlen(_Data_Pack)); GSM_Serial->print(F("\r\n"));
						GSM_Serial->print(F("Content-Type: application/json\r\n")); 
						GSM_Serial->print(F("User-Agent: PostOffice\r\n"));
						GSM_Serial->print(F("\r\n"));

					} else if (_Method == _HTTP_GET_) {

						// Print Header
						GSM_Serial->print(F("GET ")); GSM_Serial->print(_EndPoint); GSM_Serial->print(F(" HTTP/1.1\r\n"));
						GSM_Serial->print(F("Host: ")); GSM_Serial->print(_Server); GSM_Serial->print(F("\r\n"));
						GSM_Serial->print(F("Connection: keep-alive\r\n"));
						GSM_Serial->print(F("Cache-Control: no-cache\r\n"));
						GSM_Serial->print(F("User-Agent: PostOffice\r\n"));
						GSM_Serial->print(F("\r\n"));

					}

					// Send Data Pack
					GSM_Serial->print(_Data_Pack);

					// Print End Char
					GSM_Serial->print((char)26);

					// Declare Buffer Object
					Serial_Buffer _Buffer_Send = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SSEND_, 7};

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					this->Read_UART_Buffer(&_Buffer_Send);

					// End Function
					return(_Buffer_Send.Response == _AT_OK_);

				}

				// End Function
				return (false);

			}

			// Socket Recieve Function
			bool SRECV(const uint8_t _ConnID, const uint16_t _MaxByte, char * _Data) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_UART_READ_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SRECV="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_MaxByte);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SRECV_, 1023};

				this->Read_UART_Buffer(&_Buffer);

				// Copy received data to caller buffer (bounded by actual bytes read)
				if (_Buffer.Response == _AT_OK_) {
					memcpy(_Data, _IO_Buffer, _Buffer.Read_Order);
					_Data[_Buffer.Read_Order] = '\0';
				}

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Detect SRING Response.
			bool SRING(uint16_t &_Length) {

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, 50000, 30};

				// Clear IO Buffer
				memset(_IO_Buffer, '\0', 30);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// \r\nOK\r\n\r\nSRING: 1,999\r\n		--> 22 Char
				// \r\nOK\r\n\r\nSRING: 1,99\r\n		--> 21 Char
				// \r\nOK\r\n\r\nSRING: 1,9\r\n			--> 20 Char
				// \r\nSRING: 1,999\r\n					--> 16 Char
				// \r\nSRING: 1,99\r\n					--> 15 Char
				// \r\nSRING: 1,9\r\n					--> 14 Char

				// Read UART Response
				while (!_Buffer.Response) {

					// Read Delay
					delay(1);

					// Read Serial Char
					_IO_Buffer[_Buffer.Read_Order] = GSM_Serial->read();

					// Handle for Response
					if (_Buffer.Read_Order == 21 && _IO_Buffer[8] == 'S' && _IO_Buffer[20] == '\r') {

						// \r\nOK\r\n\r\nSRING: 1,999\r\n

						// Geth Length from Buffer
						uint8_t _Digit_1 = _IO_Buffer[17] - '0';
						uint8_t _Digit_2 = _IO_Buffer[18] - '0';
						uint8_t _Digit_3 = _IO_Buffer[19] - '0';

						// Calculate Length
						_Length = (_Digit_1 * 100) + (_Digit_2 * 10) + _Digit_3;

						// Set Response
						_Buffer.Response = _AT_SRING_;

						// End Function
						return(true);

					} else if (_Buffer.Read_Order == 20 && _IO_Buffer[8] == 'S' && _IO_Buffer[19] == '\r') {

						// \r\nOK\r\n\r\nSRING: 1,99\r\n

						// Geth Length from Buffer
						uint8_t _Digit_1 = _IO_Buffer[17] - '0';
						uint8_t _Digit_2 = _IO_Buffer[18] - '0';

						// Calculate Length
						_Length = (_Digit_1 * 10) + _Digit_2;

						// Set Response
						_Buffer.Response = _AT_SRING_;

						// End Function
						return(true);

					} else if (_Buffer.Read_Order == 19 && _IO_Buffer[8] == 'S' && _IO_Buffer[18] == '\r') {

						// \r\nOK\r\n\r\nSRING: 1,9\r\n

						// Geth Length from Buffer
						_Length = _IO_Buffer[17] - '0';

						// Set Response
						_Buffer.Response = _AT_SRING_;

						// End Function
						return(true);

					} else if (_Buffer.Read_Order == 15 && _IO_Buffer[2] == 'S' && _IO_Buffer[14] == '\r') {

						// \r\nSRING: 1,999\r\n

						// Geth Length from Buffer
						uint8_t _Digit_1 = _IO_Buffer[11] - '0';
						uint8_t _Digit_2 = _IO_Buffer[12] - '0';
						uint8_t _Digit_3 = _IO_Buffer[13] - '0';

						// Calculate Length
						_Length = (_Digit_1 * 100) + (_Digit_2 * 10) + _Digit_3;

						// Set Response
						_Buffer.Response = _AT_SRING_;

						// End Function
						return(true);

					} else if (_Buffer.Read_Order == 14 && _IO_Buffer[2] == 'S' && _IO_Buffer[13] == '\r') {

						// \r\nSRING: 1,99\r\n

						// Geth Length from Buffer
						uint8_t _Digit_1 = _IO_Buffer[11] - '0';
						uint8_t _Digit_2 = _IO_Buffer[12] - '0';

						// Calculate Length
						_Length = (_Digit_1 * 10) + _Digit_2;

						// Set Response
						_Buffer.Response = _AT_SRING_;

						// End Function
						return(true);

					} else if (_Buffer.Read_Order == 13 && _IO_Buffer[2] == 'S' && _IO_Buffer[12] == '\r') {

						// \r\nSRING: 1,9\r\n

						// Geth Length from Buffer
						_Length = _IO_Buffer[11] - '0';

						// Set Response
						_Buffer.Response = _AT_SRING_;

						// End Function
						return(true);

					}

					// Increase Read Order
					if (isAscii(_IO_Buffer[_Buffer.Read_Order]) || _IO_Buffer[_Buffer.Read_Order] == '\r' || _IO_Buffer[_Buffer.Read_Order] == '\n' || _IO_Buffer[_Buffer.Read_Order] == ' ') _Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= _Buffer.Time_Out) return(false);

				}

				// End Function
				return(false);

			}
			bool SRING(void) {

				// Declare Buffer Object
				Serial_Buffer Buffer = {_AT_TIMEOUT_, 0, 0, 1000, 20};

				// Clear IO Buffer
				memset(_IO_Buffer, '\0', 20);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// \r\nSRING: 3,108\r\n

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					_IO_Buffer[Buffer.Read_Order] = GSM_Serial->read();

					// Handle for Message End
					if (Buffer.Read_Order > 5 && _IO_Buffer[Buffer.Read_Order - 1] == '\r' && _IO_Buffer[Buffer.Read_Order] == '\n') Buffer.Response = true;

					// Increase Read Order
					if (_IO_Buffer[Buffer.Read_Order] > 31 && _IO_Buffer[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;
					if (_IO_Buffer[Buffer.Read_Order] == '\r') Buffer.Read_Order += 1;
					if (_IO_Buffer[Buffer.Read_Order] == '\n') Buffer.Read_Order += 1;

					// Prevent buffer overflow
					if (Buffer.Read_Order >= Buffer.Size - 1) return(false);

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Control for SRING
				if (strstr(_IO_Buffer, "\r\nSRING") != NULL) return(true);

				// End Function
				return(false);

			}

			// WebSocket Open Function
			// Opens a TCP socket then performs the HTTP/1.1 Upgrade handshake (RFC 6455 §4.1).
			bool WSOPEN(const uint8_t _ConnID, const char * _Host, const uint16_t _Port, const char * _Path) {

				// Step 1 — TCP connect (command mode, manual close)
				this->Clear_UART_Buffer();
				delay(_AT_WAIT_DELAY_);

				GSM_Serial->print(F("AT#SD="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->print(F(",0,"));
				GSM_Serial->print(_Port);
				GSM_Serial->print(F(",\""));
				GSM_Serial->print(_Host);
				GSM_Serial->print(F("\",255,0,1"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				Serial_Buffer _Conn = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SD_, 7};
				this->Read_UART_Buffer(&_Conn);
				if (_Conn.Response != _AT_OK_) return(false);

				// Step 2 — Send HTTP Upgrade request
				this->Clear_UART_Buffer();
				delay(_AT_WAIT_DELAY_);

				GSM_Serial->print(F("AT#SSEND="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				Serial_Buffer _Prompt = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SSEND_, 7};
				this->Read_UART_Buffer(&_Prompt);
				if (_Prompt.Response != _AT_SD_PROMPT_) return(false);

				delay(_AT_SD_PROMPT_DELAY_);

				GSM_Serial->print(F("GET "));
				GSM_Serial->print(_Path);
				GSM_Serial->print(F(" HTTP/1.1\r\nHost: "));
				GSM_Serial->print(_Host);
				GSM_Serial->print(F("\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n"));
				GSM_Serial->print(F("Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"));
				GSM_Serial->print(F("Sec-WebSocket-Version: 13\r\n\r\n"));
				GSM_Serial->write(0x1A);

				Serial_Buffer _Req = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SSEND_, 7};
				delay(_AT_WAIT_DELAY_);
				this->Read_UART_Buffer(&_Req);
				if (_Req.Response != _AT_OK_) return(false);

				// Step 3 — Read 101 Switching Protocols response
				this->Clear_UART_Buffer();
				delay(_AT_WAIT_DELAY_);

				GSM_Serial->print(F("AT#SRECV="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->print(F(",512"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				Serial_Buffer _Resp = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_WSOPEN_, 1023};
				this->Read_UART_Buffer(&_Resp);

				return (_Resp.Response == _AT_OK_ && strstr(_IO_Buffer, "101") != NULL);

			}

			// WebSocket Send Function
			// Sends a masked text frame (RFC 6455 §5.2). Selects a masking key such that
			// no wire byte equals 0x1A, preventing premature AT#SSEND termination.
			bool WSSEND(const uint8_t _ConnID, const char * _Data) {

				const uint16_t _Len = (uint16_t)strlen(_Data);
				if (_Len == 0 || _Len > 125) return(false);

				// Choose per-column masking key: for byte position k, find v where
				// no payload byte at column k XORed with v equals 0x1A.
				uint8_t _Mask[4] = {0, 0, 0, 0};
				for (uint8_t k = 0; k < 4; k++) {
					for (uint8_t v = 1; v != 0; v++) {
						if (v == 0x1A) continue;
						bool _Safe = true;
						for (uint16_t i = k; i < _Len; i += 4) {
							if (((uint8_t)_Data[i] ^ v) == 0x1A) { _Safe = false; break; }
						}
						if (_Safe) { _Mask[k] = v; break; }
					}
					if (_Mask[k] == 0) return(false);
				}

				this->Clear_UART_Buffer();
				delay(_AT_WAIT_DELAY_);

				GSM_Serial->print(F("AT#SSEND="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				Serial_Buffer _Prompt = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_WSSEND_, 7};
				this->Read_UART_Buffer(&_Prompt);
				if (_Prompt.Response != _AT_SD_PROMPT_) return(false);

				delay(_AT_SD_PROMPT_DELAY_);

				// Frame header: FIN=1, opcode=text(1), MASK=1, 7-bit length
				GSM_Serial->write((uint8_t)0x81);
				GSM_Serial->write((uint8_t)(0x80 | _Len));
				GSM_Serial->write(_Mask[0]);
				GSM_Serial->write(_Mask[1]);
				GSM_Serial->write(_Mask[2]);
				GSM_Serial->write(_Mask[3]);

				// Masked payload
				for (uint16_t i = 0; i < _Len; i++) {
					GSM_Serial->write((uint8_t)((uint8_t)_Data[i] ^ _Mask[i & 3]));
				}

				GSM_Serial->write(0x1A);

				Serial_Buffer _Send = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_WSSEND_, 7};
				delay(_AT_WAIT_DELAY_);
				this->Read_UART_Buffer(&_Send);

				return(_Send.Response == _AT_OK_);

			}

			// WebSocket Receive Function
			// Reads one server frame and returns the unmasked payload.
			// Server-to-client frames are never masked (RFC 6455 §5.1).
			bool WSRECV(const uint8_t _ConnID, char * _Data, const uint16_t _MaxLen, uint8_t & _Opcode) {

				this->Clear_UART_Buffer();
				delay(_AT_UART_READ_DELAY_);

				GSM_Serial->print(F("AT#SRECV="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_MaxLen + 16);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_WSRECV_, 1023};
				this->Read_UART_Buffer_Raw(&_Buffer);

				if (_Buffer.Response != _AT_OK_) return(false);

				// Skip "#SRECV: N,M\r\n" header — frame bytes follow the second \r\n
				const char * _Frame = strstr(_IO_Buffer, "\r\n#SRECV:");
				if (_Frame == NULL) return(false);
				_Frame = strchr(_Frame + 2, '\n');
				if (_Frame == NULL) return(false);
				_Frame++;

				// Parse RFC 6455 §5.2 frame header
				_Opcode              = (uint8_t)_Frame[0] & 0x0F;
				const uint16_t _PLen = (uint16_t)((uint8_t)_Frame[1] & 0x7F);

				// Copy payload (server frames are unmasked; offset = 2)
				const uint16_t _Copy = (_PLen < _MaxLen) ? _PLen : _MaxLen - 1;
				memcpy(_Data, _Frame + 2, _Copy);
				_Data[_Copy] = '\0';

				return(true);

			}

			// WebSocket Ping Function — sends RFC 6455 §5.5.2 masked ping control frame
			bool WSPING(const uint8_t _ConnID) {

				this->Clear_UART_Buffer();
				delay(_AT_WAIT_DELAY_);

				GSM_Serial->print(F("AT#SSEND="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				Serial_Buffer _Prompt = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_WSSEND_, 7};
				this->Read_UART_Buffer(&_Prompt);
				if (_Prompt.Response != _AT_SD_PROMPT_) return(false);

				delay(_AT_SD_PROMPT_DELAY_);

				// Ping frame: FIN|PING=0x89, MASK=1 + len=0 (empty payload)
				GSM_Serial->write((uint8_t)0x89);
				GSM_Serial->write((uint8_t)0x80);
				GSM_Serial->write((uint8_t)0x01);
				GSM_Serial->write((uint8_t)0x02);
				GSM_Serial->write((uint8_t)0x03);
				GSM_Serial->write((uint8_t)0x04);
				GSM_Serial->write(0x1A);

				Serial_Buffer _Send = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_WSSEND_, 7};
				delay(_AT_WAIT_DELAY_);
				this->Read_UART_Buffer(&_Send);

				return(_Send.Response == _AT_OK_);

			}

			// WebSocket Close Function — sends RFC 6455 §5.5.1 masked close frame then closes TCP socket
			bool WSCLOSE(const uint8_t _ConnID) {

				this->Clear_UART_Buffer();
				delay(_AT_WAIT_DELAY_);

				GSM_Serial->print(F("AT#SSEND="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				Serial_Buffer _Prompt = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_WSSEND_, 7};
				this->Read_UART_Buffer(&_Prompt);

				if (_Prompt.Response == _AT_SD_PROMPT_) {

					delay(_AT_SD_PROMPT_DELAY_);

					// Close frame: FIN|CLOSE=0x88, MASK=1 + len=0 (no status code)
					GSM_Serial->write((uint8_t)0x88);
					GSM_Serial->write((uint8_t)0x80);
					GSM_Serial->write((uint8_t)0x05);
					GSM_Serial->write((uint8_t)0x06);
					GSM_Serial->write((uint8_t)0x07);
					GSM_Serial->write((uint8_t)0x08);
					GSM_Serial->write(0x1A);

					Serial_Buffer _Send = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_WSSEND_, 7};
					delay(_AT_WAIT_DELAY_);
					this->Read_UART_Buffer(&_Send);

				}

				// Close TCP socket regardless of close frame outcome
				return(this->SH(_ConnID));

			}

			// Manual DNS Selection Function
			bool DNS(const uint8_t _ConnID, const char * _Primary, const char * _Secondary) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#DNS="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->print(F(",\""));
				GSM_Serial->print(_Primary);
				GSM_Serial->print(F("\",\""));
				GSM_Serial->print(_Secondary);
				GSM_Serial->print(F("\""));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_DNS_, 20};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// DNS Response Cache Function
			bool CACHEDNS(const bool _Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#CACHEDNS="));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_CACHEDNS_, 20};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Close FTP Connection Function
			bool FTPCLOSE(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPCLOSE"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_FTPCLOSE_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Change FTP Folder Function
			bool FTPCWD(const char * _Folder) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPCWD="));
				GSM_Serial->print(F("\""));
				GSM_Serial->print(_Folder);
				GSM_Serial->print(F("\""));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_FTPCWD_, 30};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Get FTP File Size Function
			bool FTPFSIZE(const char * _FileName, uint32_t & _Length) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPFSIZE="));
				GSM_Serial->print(F("\""));
				GSM_Serial->print(_FileName);
				GSM_Serial->print(F("\""));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_FTPFSIZE_, 30};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n#FTPFSIZE: 174945\r\n\r\nOK\r\n
					// \r\n#FTPFSIZE: 329463\r\n\r\nOK\r\n

					// Parse Length
					if (sscanf(_IO_Buffer, "\r\n#FTPFSIZE: %lu\r\n\r\nOK\r\n", &_Length) != 1) return false;

					// End Function
					return (true);

				}

				// End Function
				return(false);

			}

			// Get FTP File to Buffer function
			bool FTPGETPKT(const char * _FileName, const uint8_t _ViewMode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPGETPKT="));
				GSM_Serial->print(F("\""));
				GSM_Serial->print(_FileName);
				GSM_Serial->print(F("\","));
				GSM_Serial->print(_ViewMode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_FTPGETPKT_, 30};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Open FTP Connection Function
			bool FTPOPEN(const char * _Server, const char * _Username, const char * _Password, const uint8_t _Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPOPEN="));
				GSM_Serial->print(F("\""));
				GSM_Serial->print(_Server);
				GSM_Serial->print(F("\",\""));
				GSM_Serial->print(_Username);
				GSM_Serial->print(F("\",\""));
				GSM_Serial->print(_Password);
				GSM_Serial->print(F("\","));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_FTPOPEN_, 30};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Get FTP File From Buffer Function
			bool FTPRECV(const uint16_t _Size, char * _Data) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPRECV="));
				GSM_Serial->print(_Size);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_FTPRECV_, 1024};

				this->Read_UART_Buffer(&_Buffer);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {
					
					// \r\n#FTPRECV: 200\r\n20202055\r\n:100BA00020202020000D0A002C002C002C00415495\r\n:100BB00023534C3D000D0A004154234532534C52FF\r\n:100BC000493D000D0A00415423534C4544534156BE\r\n:100BD000000D0A00415423534C45443D000D0A00CA\r\n:100BE0004\r\n\r\nOK\r\n

					// Copy received data (bounded by actual bytes read)
					memcpy(_Data, _IO_Buffer, _Buffer.Read_Order);
					_Data[_Buffer.Read_Order] = '\0';

					// End Function
					return (true);

				}

				// End Function
				return (false);

			}

			// Set FTP Time Out Function
			bool FTPTO(const uint16_t _TOut) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPTO="));
				GSM_Serial->print(_TOut);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_FTPTO_, 30};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Set FTP File Transfer Type Function
			bool FTPTYPE(const uint8_t _Type) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPTYPE="));
				GSM_Serial->print(_Type);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_FTPTYPE_, 30};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Soft Reset Function
			bool Z(const uint8_t _Z) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("ATZ"));
				GSM_Serial->print(_Z);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_Z_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Detach From Network and Shut Down Modem Function
			bool SHDN(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SHDN"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {_AT_TIMEOUT_, 0, 0, _TIMEOUT_SHDN_, 7};

				this->Read_UART_Buffer(&_Buffer);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

		// Public Context
		public:

			// Constructor
			explicit LE910C1_EUX(Stream &_Serial) : GSM_Serial(&_Serial) {

				memset(_IO_Buffer, '\0', _IO_Buffer_Size);

			}

	};
