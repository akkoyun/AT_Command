/* *******************************************************************************
 *  Copyright (C) 2014-2022 Mehmet Gunce Akkoyun Can not be copied and/or
 *	distributed without the express permission of Mehmet Gunce Akkoyun.
 *
 *	Library				: AT_Command
 *	Code Developer		: Mehmet Gunce Akkoyun (akkoyun@me.com)
 *
 *********************************************************************************/

#ifndef __AT_Command__
#define __AT_Command__

	// Include Arduino Library
	#ifndef Arduino_h
		#include <Arduino.h>
	#endif

	// Include Definations
	#include "Definations.h"
	#include "Config.h"

	// Telit LE910C1_EUX Class
	class LE910C1_EUX {

		// Define Modem Name
		#ifndef _LE910C1_EUX_
			#define _LE910C1_EUX_
		#endif

		// Private Variables
		private:

			// GSM Serial Stream Definition
			Stream * GSM_Serial;

			// Serial Buffer Variable Structure Definition
			struct Serial_Buffer {
				uint8_t 			Response;
				uint16_t 			Read_Order;
				uint16_t 			Data_Order;
				const uint16_t 		Time_Out;
				uint16_t 			Size;
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
			bool Read_UART_Buffer(Serial_Buffer * _Buffer, char * _Buffer_Variable) {

				// Response Wait Delay
				delay(5);

				// Set Read Order
				_Buffer->Read_Order = 0;
				_Buffer->Response = _AT_TIMEOUT_;

				// Read Current Time
				const uint32_t _Current_Time = millis();

				// Read UART Response
				while (GSM_Serial->available() > 0) {

					// Read Serial Char
					_Buffer_Variable[_Buffer->Read_Order] = GSM_Serial->read();

					// Control for Response
					_Buffer->Response = this->Find(_Buffer_Variable, _Buffer->Read_Order);

					// Increase Read Order
					if (isAscii(_Buffer_Variable[_Buffer->Read_Order])) _Buffer->Read_Order++;

					// Handle for timeout
					if (millis() - _Current_Time >= _Buffer->Time_Out) break;

				}

				// Control for Response
				if (_Buffer->Response != _AT_TIMEOUT_) return(true);

				// End Function
				return(false);

			}

			// OK Find Function
			uint8_t Find(char * _Buffer, const uint16_t _Size) {

				// Declare Response Variable
				uint8_t _Response = _AT_TIMEOUT_;

				// Control for "\r\n> "
				if (_Size > 3) {

					// \r\n> 
					// Control for <\r\n> > Response
					if (
						(_Buffer[_Size - 3] == 13) && 	// \r
						(_Buffer[_Size - 2] == 10) && 	// \n
						(_Buffer[_Size - 1] == 62) && 	// >
						(_Buffer[_Size - 0] == 32)		// " "
					) _Response = _AT_SD_PROMPT_;

				} 

				// Control "\r\nOK\r\n"
				if (_Size > 5) {

					// \r\nOK\r\n
					// Control for <\r\nOK\r\n> Response
					if (
						(_Buffer[_Size - 5] == 13) && 	// \r
						(_Buffer[_Size - 4] == 10) && 	// \n
						(_Buffer[_Size - 3] == 79) && 	// O
						(_Buffer[_Size - 2] == 75) && 	// K
						(_Buffer[_Size - 1] == 13) && 	// \r
						(_Buffer[_Size - 0] == 10)  	// \n
					) _Response = _AT_OK_;

				}

				// Control for "\r\nERROR\r\n"
				if (_Size > 8) {

					// \r\nERROR\r\n
					// Control for <\r\nERROR\r\n> Response
					if (
						(_Buffer[_Size - 8] == 13) && 	// \r
						(_Buffer[_Size - 7] == 10) && 	// \n
						(_Buffer[_Size - 6] == 69) && 	// E
						(_Buffer[_Size - 5] == 82) && 	// R
						(_Buffer[_Size - 4] == 82) && 	// R
						(_Buffer[_Size - 3] == 79) && 	// O
						(_Buffer[_Size - 2] == 82) && 	// R
						(_Buffer[_Size - 1] == 13) && 	// \r
						(_Buffer[_Size - 0] == 10)		// \n
					) _Response = _AT_ERROR_;

				}

				// Control for "\r\nSRING: 3\r\n"
				if (_Size > 11) {

					// \r\nSRING: 3\r\n
					if (
						(_Buffer[_Size - 11] == 13) && 	// \r
						(_Buffer[_Size - 10] == 10) && 	// \n
						(_Buffer[_Size - 9] == 83) && 	// S
						(_Buffer[_Size - 8] == 82) && 	// R
						(_Buffer[_Size - 7] == 73) && 	// I
						(_Buffer[_Size - 6] == 78) && 	// N
						(_Buffer[_Size - 5] == 71) && 	// G
						(_Buffer[_Size - 4] == 58) && 	// :
						(_Buffer[_Size - 3] == 32) && 	// " "
						(_Buffer[_Size - 1] == 13) && 	// \r
						(_Buffer[_Size - 0] == 10)		// \n
					) _Response = _AT_SRING_;

				}

				// Control for "\r\nSRING: 3,6\r\n"
				if (_Size > 13) {

					// \r\nSRING: 3,6\r\n
					if (
						(_Buffer[_Size - 13] == 13) && 	// \r
						(_Buffer[_Size - 12] == 10) && 	// \n
						(_Buffer[_Size - 11] == 83) && 	// S
						(_Buffer[_Size - 10] == 82) && 	// R
						(_Buffer[_Size - 9] == 73) && 	// I
						(_Buffer[_Size - 8] == 78) && 	// N
						(_Buffer[_Size - 7] == 71) && 	// G
						(_Buffer[_Size - 6] == 58) && 	// :
						(_Buffer[_Size - 5] == 32) && 	// " "
						(_Buffer[_Size - 1] == 13) && 	// \r
						(_Buffer[_Size - 0] == 10)		// \n
					) _Response = _AT_SRING_;

				}

				// Control for "\r\nSRING: 3,16\r\n"
				if (_Size > 14) {

					// \r\nSRING: 3,16\r\n
					if (
						(_Buffer[_Size - 14] == 13) && 	// \r
						(_Buffer[_Size - 13] == 10) && 	// \n
						(_Buffer[_Size - 12] == 83) && 	// S
						(_Buffer[_Size - 11] == 82) && 	// R
						(_Buffer[_Size - 10] == 73) && 	// I
						(_Buffer[_Size - 9] == 78) && 	// N
						(_Buffer[_Size - 8] == 71) && 	// G
						(_Buffer[_Size - 7] == 58) && 	// :
						(_Buffer[_Size - 6] == 32) && 	// " "
						(_Buffer[_Size - 1] == 13) && 	// \r
						(_Buffer[_Size - 0] == 10)		// \n
					) _Response = _AT_SRING_;

				}

				// Control for "\r\nSRING: 3,166\r\n"
				if (_Size > 15) {

					// \r\nSRING: 3,166\r\n
					if (
						(_Buffer[_Size - 15] == 13) && 	// \r
						(_Buffer[_Size - 14] == 10) && 	// \n
						(_Buffer[_Size - 13] == 83) && 	// S
						(_Buffer[_Size - 12] == 82) && 	// R
						(_Buffer[_Size - 11] == 73) && 	// I
						(_Buffer[_Size - 10] == 78) && 	// N
						(_Buffer[_Size - 9] == 71) && 	// G
						(_Buffer[_Size - 8] == 58) && 	// :
						(_Buffer[_Size - 7] == 32) && 	// " "
						(_Buffer[_Size - 1] == 13) && 	// \r
						(_Buffer[_Size - 0] == 10)		// \n
					) _Response = _AT_SRING_;

				}

				// Control for "\r\n+CME ERROR: 6\r\n"
				if (_Size > 16) {

					// \r\n+CME ERROR: 6\r\n
					// Control for <\r\n+CME> Response
					if (
						(_Buffer[_Size - 16] == 13) && 	// \r
						(_Buffer[_Size - 15] == 10) && 	// \r
						(_Buffer[_Size - 14] == 43) && 	// +
						(_Buffer[_Size - 13] == 67) &&	// C 
						(_Buffer[_Size - 12] == 77) && 	// M
						(_Buffer[_Size - 11] == 69) && 	// E
						(_Buffer[_Size - 10] == 32) && 	// " "
						(_Buffer[_Size - 9] == 69) && 	// E
						(_Buffer[_Size - 8] == 82) && 	// R
						(_Buffer[_Size - 7] == 82) && 	// R
						(_Buffer[_Size - 6] == 79) && 	// O
						(_Buffer[_Size - 5] == 82) && 	// R
						(_Buffer[_Size - 4] == 58) && 	// :
						(_Buffer[_Size - 3] == 32) && 	// " "
						(_Buffer[_Size - 1] == 13) && 	// \r
						(_Buffer[_Size - 0] == 10)		// \r
					) _Response = _AT_CME_;

				} 

				// Control for "\r\n+CME ERROR: 14\r\n"
				if (_Size > 17) {

					// \r\n+CME ERROR: 14\r\n
					// Control for <\r\n+CME> Response
					if (
						(_Buffer[_Size - 17] == 13) && 	// \r
						(_Buffer[_Size - 16] == 10) && 	// \r
						(_Buffer[_Size - 15] == 43) && 	// +
						(_Buffer[_Size - 14] == 67) &&	// C 
						(_Buffer[_Size - 13] == 77) && 	// M
						(_Buffer[_Size - 12] == 69) && 	// E
						(_Buffer[_Size - 11] == 32) && 	// " "
						(_Buffer[_Size - 10] == 69) && 	// E
						(_Buffer[_Size - 9] == 82) && 	// R
						(_Buffer[_Size - 8] == 82) && 	// R
						(_Buffer[_Size - 7] == 79) && 	// O
						(_Buffer[_Size - 6] == 82) && 	// R
						(_Buffer[_Size - 5] == 58) && 	// :
						(_Buffer[_Size - 4] == 32) && 	// " "
						(_Buffer[_Size - 1] == 13) && 	// \r
						(_Buffer[_Size - 0] == 10)		// \r
					) _Response = _AT_CME_;

				} 

				// Control for "\r\n+CME ERROR: 614\r\n"
				if (_Size > 18) {

					// \r\n+CME ERROR: 614\r\n
					// Control for <\r\n+CME> Response
					if (
						(_Buffer[_Size - 18] == 13) && 	// \r
						(_Buffer[_Size - 17] == 10) && 	// \r
						(_Buffer[_Size - 16] == 43) && 	// +
						(_Buffer[_Size - 15] == 67) &&	// C 
						(_Buffer[_Size - 14] == 77) && 	// M
						(_Buffer[_Size - 13] == 69) && 	// E
						(_Buffer[_Size - 12] == 32) && 	// " "
						(_Buffer[_Size - 11] == 69) && 	// E
						(_Buffer[_Size - 10] == 82) && 	// R
						(_Buffer[_Size - 9] == 82) && 	// R
						(_Buffer[_Size - 8] == 79) && 	// O
						(_Buffer[_Size - 7] == 82) && 	// R
						(_Buffer[_Size - 6] == 58) && 	// :
						(_Buffer[_Size - 5] == 32) && 	// " "
						(_Buffer[_Size - 1] == 13) && 	// \r
						(_Buffer[_Size - 0] == 10)		// \r
					) _Response = _AT_CME_;

				} 				

				// End Function
				return(_Response);

			}

			// RSSI to Signal Quality Function
			uint8_t RSSI_to_Signal_Quality(const uint8_t _RSSI) {

				// Handle for RSSI
				if (_RSSI >= -65) {

					// Return Signal Quality
					return 5;

				} else if (_RSSI >= -75) {

					// Return Signal Quality
					return 4;

				} else if (_RSSI >= -85) {

					// Return Signal Quality
					return 3;

				} else if (_RSSI >= -95) {

					// Return Signal Quality
					return 2;

				} else {

					// Return Signal Quality
					return 1;

				}

			}

		// Public Variables
		public:

			// Constructor
			LE910C1_EUX(Stream &_Serial) {

				// Set Serial Port
				GSM_Serial = & _Serial;

			}

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_AT_, 11};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Set Connection Mode Function
			bool FCLASS(const uint8_t _FCLASS = 0) {

				// Control for Parameter
				if (_FCLASS != 0 and _FCLASS != 8) return(false);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_FCLASS_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CGMI_, 35};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
					sscanf(_Buffer_Variable, "\r\n%9s\r\n\r\nOK\r\n", _Manufacturer_Name);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CGMM_, 35};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Get Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Set No Model
				_Model = _MODEM_MODEL_UNKNOWN_;

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// Declare Variable
					char _Model_Name[15];

					// Clear Variable
					memset(_Model_Name, '\0', 15);

					// Get Model Name				
					sscanf(_Buffer_Variable, "\r\n%14s\r\n\r\nOK\r\n", _Model_Name);

					// Control for Model Name
					if (strstr(_Model_Name, "GE910-QUAD") != NULL) {

						// Set Model
						_Model = _MODEM_MODEL_GE910_QUAD_;

					} else if (strstr(_Model_Name, "GE910-QUAD-V3") != NULL) {

						// Set Model
						_Model = _MODEM_MODEL_GE910_QUAD_V3_;

					} else if (strstr(_Model_Name, "LE910S1-EA") != NULL) {

						// Set Model
						_Model = _MODEM_MODEL_LE910S1_EA_;

					} else if (strstr(_Model_Name, "LE910R1-EU") != NULL) {

						// Set Model
						_Model = _MODEM_MODEL_LE910R1_EU_;

					} else if (strstr(_Model_Name, "LE910C1-EUX") != NULL) {

						// Set Model
						_Model = _MODEM_MODEL_LE910C1_EUX_;

					} else {

						// Set Model
						_Model = _MODEM_MODEL_UNKNOWN_;

					}

					// Return Response
					if (_Model != _MODEM_MODEL_UNKNOWN_) return(true);

				}

				// End Function
				return(false);

			}

			// Get Firmware Function
			bool SWPKGV(char * _Firmware) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SWPKGV"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SWPKGV_, 70};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// AT#SWPKGV\r\n
					// \r\n25.30.226-P0F.225200\r\nM0F.223006\r\nP0F.225200\r\nA0F.223006\r\n\r\nOK\r\n

					// Clear Variable
					memset(_Firmware, '\0', 15);

					// Handle Firmware
					strncpy(_Firmware, &_Buffer_Variable[2], 9);

					// End Function
					return true;

				}

				// End Function
				return(false);

			}

			// Get IMEI Function
			bool CGSN(char * _IMEI) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT+CGSN=1"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CGSN_, 35};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// AT+CGSN=1\r\n
					// \r\n+CGSN: 354485417617003\r\n\r\nOK\r\n

					// Clear Buffer Variable
					memset(_IMEI, '\0', 17);

					// Control for Numeric
					for (uint8_t i = 0; i < _Buffer.Size; i++) {

						// Control for Numeric
						if (isdigit(_Buffer_Variable[i])) {

							// Set IMEI
							_IMEI[_Buffer.Data_Order++] = _Buffer_Variable[i];

						}

					}

					// End Function
					return true;

				}

				// End Function
				return(false);

			}

			// Set Power Mode Function
			bool CFUN(const uint8_t _Fun = 1) {

				// Control for Parameter
				if (_Fun != 0 and _Fun != 1 and _Fun != 4 and _Fun != 5 and _Fun != 12) return(false);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CFUN_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// Set CMEE Function
			bool CMEE(const uint8_t _CMEE = 1) {

				// Control for Parameter
				if (_CMEE != 0 and _CMEE != 1 and _CMEE != 2) return(false);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CMEE_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CEER_, 21};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// Handle Variables
					if (sscanf(_Buffer_Variable, "\r\n#CEER: %03d\r\n\r\nOK\r\n", &_Code) == 1) return (true);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_ATE_, 13};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// Flow Control Function
			bool K(const uint8_t _K = 0) {

				// Control for Parameter
				if (_K != 0 and _K != 3) return(false);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_K_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CPIN_, 30};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
					sscanf(_Buffer_Variable, "\r\n+CPIN: %09s\r\n\r\nOK\r\n", _PIN_Response);

					// Control for SIM State
					if (strstr(_PIN_Response, "READY")) {
						_Code = _SIM_READY_;
					} else if (strstr(_PIN_Response, "SIM PIN")) {
						_Code = _SIM_PIN_;
					} else if (strstr(_PIN_Response, "SIM PUK")) {
						_Code = _SIM_PUK_;
					} else {
						_Code = _SIM_UNKNOWN_;
					}

					// Handle Status
					return (_Code != _SIM_UNKNOWN_);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CCID_, 37};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// Clear ICCID Variable
					memset(_ICCID, '\0', 21);

					// Handle for Response
					for (uint8_t i = 0; i < _Buffer.Size; i++) {

						// Handle ICCID
						if (isdigit(_Buffer_Variable[i])) {

							// Set ICCID Variable
							_ICCID[_Buffer.Data_Order++] = _Buffer_Variable[i];

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
				if (_Function_Type == SET) {

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
					Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SIMDET_, 23};

					// Declare Buffer Variable
					char _Buffer_Variable[_Buffer.Size];

					// Clear Buffer Variable
					memset(_Buffer_Variable, '\0', _Buffer.Size);

					// Declare Response
					this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

					// Handle for Response
					return (_Buffer.Response == _AT_OK_);

				}

				// GET Function
				if (_Function_Type == GET) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT#SIMDET?"));
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SIMDET_, 23};

					// Declare Buffer Variable
					char _Buffer_Variable[_Buffer.Size];

					// Clear Buffer Variable
					memset(_Buffer_Variable, '\0', _Buffer.Size);

					// Declare Response
					this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

					// Clear Variables
					_Mode = 0;
					_SIM_in_Pin_State = false;

					// Handle for Response
					if (_Buffer.Response == _AT_OK_) {

						// \r\n#SIMDET: 2,1\r\n\r\nOK\r\n

						// Read SIMDET Mode
						_Mode = _Buffer_Variable[11];

						// Read Pin State
						if (_Buffer_Variable[13] == 1) {
							
							// Set Variable
							_SIM_in_Pin_State = true;

						} else {

							// Set Variable
							_SIM_in_Pin_State = false;

						}

						// End Function
						return (true);

					}

				}

				// End Function
				return (false);

			}

			// Set GPIO Function
			bool GPIO(const bool _Function_Type = SET, const uint8_t _Pin = 1, const uint8_t _Mode = 0, const uint8_t _Direction = 2) {

				// SET Function
				if (_Function_Type == SET) {

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
					Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_GPIO_, 7};

					// Declare Buffer Variable
					char _Buffer_Variable[_Buffer.Size];

					// Clear Buffer Variable
					memset(_Buffer_Variable, '\0', _Buffer.Size);

					// Get Response
					this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

					// Handle for Response
					return (_Buffer.Response == _AT_OK_);

				}

				return(false);

			}

			// Set SLED Function
			bool SLED(const uint8_t _SLED = 2) {

				// Control for Parameter
				if (_SLED != 0 and _SLED != 1 and _SLED != 2 and _SLED != 3 and _SLED != 4 and _SLED != 5) return(false);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SLED_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Get Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SLEDSAV_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_E2SLRI_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Get Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				return (_Buffer.Response == _AT_OK_);

			}

			// CREG Function
			bool CREG(const bool _Function_Type, uint8_t & _Mode, uint8_t & _Stat) {

				// SET Function
				if (_Function_Type == SET) {

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
					Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CREG_, 25};

					// Declare Buffer Variable
					char _Buffer_Variable[_Buffer.Size];

					// Clear Buffer Variable
					memset(_Buffer_Variable, '\0', _Buffer.Size);

					// Get Response
					this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

					// Handle for Response
					return (_Buffer.Response == _AT_OK_);

				}

				// GET Function
				if (_Function_Type == GET) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT+CREG?"));
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CREG_, 25};

					// Declare Buffer Variable
					char _Buffer_Variable[_Buffer.Size];

					// Clear Buffer Variable
					memset(_Buffer_Variable, '\0', _Buffer.Size);

					// Get Response
					this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

					// Handle for Response
					if (_Buffer.Response == _AT_OK_) {

						// Handle Variables
						sscanf(_Buffer_Variable, "\r\n+CREG: %hhu,%hhu\r\n\r\nOK\r\n", &_Mode, &_Stat);

						// Handle Response
						if (_Stat >= 0 && _Stat <= 11) return(true);

					}

				}

				// End Function
				return(false);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CGDCONT_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SGACT_, 35};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n#SGACT: 000.000.000.000\r\n\r\nOK\r\n

					// Handle IP
					uint8_t _Variable_Count = sscanf(_Buffer_Variable, "\r\n#SGACT: %03hhu.%03hhu.%03hhu.%03hhu\r\n\r\nOK\r\n", &_IP_Segment[0], &_IP_Segment[1], &_IP_Segment[2], &_IP_Segment[3]);

					// Control for IP
					return (_Variable_Count == 4);

				}

				// End Function
				return(false);

			}

			// WS46 Function
			bool WS46(const bool _Function_Type, uint8_t & _Mode) {

				// GET Function
				if (_Function_Type == GET) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT+WS46?"));
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_WS46_, 20};

					// Declare Buffer Variable
					char _Buffer_Variable[_Buffer.Size];

					// Clear Buffer Variable
					memset(_Buffer_Variable, '\0', _Buffer.Size);

					// Declare Response
					this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

					// Handle for Response
					if (_Buffer.Response == _AT_OK_) {

						// Handle Variables
						uint8_t _Variable_Count = sscanf(_Buffer_Variable, "\r\n+WS46: %02hhu\r\n\r\nOK\r\n", &_Mode);

						// Handle Response
						return (_Variable_Count == 1);

					}

				}

				// SET Function
				if (_Function_Type == SET) {

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
					Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_WS46_, 20};

					// Declare Buffer Variable
					char _Buffer_Variable[_Buffer.Size];

					// Clear Buffer Variable
					memset(_Buffer_Variable, '\0', _Buffer.Size);

					// Declare Response
					this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

					// Handle for Response
					return (_Buffer.Response == _AT_OK_);

				}

				// End Function
				return(false);

			}

			// RFSTS Function
			bool RFSTS(uint16_t & _MCC, uint16_t & _MNC, uint16_t & _RSSI, uint8_t & _Signal_Level, uint32_t & _Cell_ID, uint16_t & _TAC) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);
				
				// Send UART Command
				GSM_Serial->print(F("AT#RFSTS"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_RFSTS_, 120};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// AT#RFSTS\r\n
					// \r\n#RFSTS: "286 01",1795,-101,-67,-15,2242,,128,3,1,0B5D121,"286016339612498","Turkcell",3,3,108\r\n\r\nOK\r\n
					// \r\n#RFSTS: "286 01",1651,-101,-66,-15,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,121\r\n\r\nOK\r\n

					// #RFSTS:<PLMN>,<EARFCN>,<RSRP>,<RSSI>,<RSRQ>,<TAC>,<RAC>,[<TXPWR>],<DR X>, <MM>,<RRC>,<CID>,<IMSI>,[<NetNameAsc>],<SD>,<ABND>,<T3402>,<T3412>,<SI NR>
					//
					// <PLMN> 		- Country code and operator code(MCC, MNC)						+ "286 01"
					// <EARFCN> 	- E-UTRA Assigned Radio Channel									- 6400	
					// <RSRP> 		- Reference Signal Received Power								- -100
					// <RSSI> 		- Received Signal Strength Indication							+ -66
					// <RSRQ> 		- Reference Signal Received Quality								- -18
					// <TAC> 		- Tracking Area Code											+ 2242
					// <RAC> 		- Routing Area Code												- FF
					// <TXPWR> 		- Tx Power (In traffic only)									- 0
					// <DR X> 		- Discontinuous reception cycle Length (cycle length in ms)		- 128
					// <MM> 		- Mobility Management state (for debug purpose only)			- 10
					// <RRC> 		- Radio Resource state (for debug purpose only)					- 1
					// <CID> 		- Cell ID														+ 859315
					// <IMSI> 		- International Mobile Station ID								- "286016339811626"
					// <NetNameAsc> - Operator name													- "TR TURKCELL"
					// <SD> 		- Service Domain												- 3
					// <ABND> 		- Active Band													- 20
					// <T3402> 		- Timer T3402 in seconds										- 720
					// <T3412> 		- Timer T3412 in seconds										- 3240
					// <SI NR> 		- Signal-to-Interface plus Noise Ratio							- -5

					// Example Response
					// \r\n#RFSTS: "286 01",1651,-101,-66,-15,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,121\r\n\r\nOK\r\n
					// \r\n#RFSTS: "286 01",1651,-100,-66,-14,2242,,128,3,0,0B5D120,"286016339612498","Turkcell",3,3,126\r\n\r\nOK\r\n
					// \r\n#RFSTS: "286 01",1795,-101,-73,-10,2242,,128,3,0,0B5D121,"286016339612498","Turkcell",3,3,138\r\n\r\nOK\r\n
					// \r\n#RFSTS: "286 01",100,-98,-68,-10,2242,227,128,3,1,0B5D125,"286016339612498","Turkcell",3,1,120\r\n\r\nOK\r\n
					// \r\n#RFSTS: "286 01",1651,-99,-63,-14,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,101\r\n\r\nOK\r\n
					// \r\n#RFSTS: "286 01",1651,-99,-68,-12,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,128\r\n\r\nOK\r\n
					// \r\n#RFSTS: "286 01",1795,-102,-69,-15,2242,,128,3,1,0B5D121,"286016339612498","Turkcell",3,3,102\r\n\r\nOK\r\n
					// \r\n#RFSTS: "286 01",1651,-99,-67,-12,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,126\r\n\r\nOK\r\n
					// \r\n#RFSTS: "286 01",1651,-99,-64,-14,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,107\r\n\r\nOK\r\n



					// Handle Variables
					char * _Segment_1 = strtok(_Buffer_Variable, ",");	// \r\n#RFSTS: "286 01"
					strtok(NULL, ",");									// 1651
					strtok(NULL, ",");									// -101
					char * _Segment_4 = strtok(NULL, ",");				// -66
					strtok(NULL, ",");									// -15
					char * _Segment_6 = strtok(NULL, ",");				// 2242
					strtok(NULL, ",");									//	
					strtok(NULL, ",");									// 128
					strtok(NULL, ",");									// 3
					strtok(NULL, ",");									// 1
					char * _Segment_11 = strtok(NULL, ",");				// 0B5D120

					// Handle MCC, MNC
					_MCC = 0; _MNC = 0;
					sscanf(_Segment_1, "\r\n#RFSTS: \"%03u %02u\"", &_MCC, &_MNC);

					// Handle RSSI
					_RSSI = 0;
					sscanf(_Segment_4, "-%03u", &_RSSI);

					// Calculate Signal Level
					_Signal_Level = 0;
					_Signal_Level = this->RSSI_to_Signal_Quality(_RSSI * -1);

					// Handle TAC
					_TAC = 0;
					sscanf(_Segment_6, "%05u", &_TAC);

					// Handle Cell ID
					_Cell_ID = 0;
					sscanf(_Segment_11, "%lX", &_Cell_ID);

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// MONI Function
			bool MONI(uint16_t & _TAC, uint32_t & _Cell_ID, uint16_t & _RSSI, uint8_t & _Signal_Level, uint32_t & _PCell_ID) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Command Chain Delay (Advice by Telit)
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#MONI"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_MONI_, 130};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// Control for GSM or LTE
					if (strstr(_Buffer_Variable, "bsic") != NULL) {

						// GSM network
						// #MONI: <netname> BSIC:<bsic> RxQual:<qual> LAC:<lac> Id:<id> ARFCN:<arfcn> PWR:<dBm> dBm TA: <timadv>

					} else {

						// LTE network
						// #MONI: <netmame> RSRP:<rsrp> RSRQ:<rsrq> TAC:<tac> Id:<id> EARFCN:<earfcn> PWR:<dBm> DRX:<drx> pci:<physicalCellId> QRxLevMin:<QRxLevMin>

						// <netname>			- network name
						// <id>					- cell identifier
						// <dBm>				- received signal strength in dBm
						// <drx>				- Discontinuous reception cycle length
						// <physicalCellId>		- physical cell identifier
						// <rsrp>				- Reference Signal Received Power
						// <rsrq>				- Reference Signal Received Quality
						// <tac>				- Tracking Area Code
						// <earfcn>				- E-UTRA Absolute Radio Frequency Channel Number
						// <QRxLevMin>			- Minimum required received signal level

						// AT#MONI\r\n
						// \r\n#MONI: Turkcell RSRP:-97 RSRQ:-11 TAC:2242 Id:0B5D125 EARFCN:100 PWR:-66dbm DRX:128 pci:335 QRxLevMin:0\r\n\r\nOK\r\n

						// Handle Variables
						strtok(_Buffer_Variable, " ");				// \r\n#MONI:
						strtok(NULL, " ");							// Turkcell
						strtok(NULL, " ");							// RSRP:-97
						strtok(NULL, " ");							// RSRQ:-11
						char * _Segment_5 = strtok(NULL, " ");		// TAC:2242
						char * _Segment_6 = strtok(NULL, " ");		// Id:0B5D125
						strtok(NULL, " ");							// EARFCN:100
						char * _Segment_8 = strtok(NULL, " ");		// PWR:-66dbm
						strtok(NULL, " ");							// DRX:128
						char * _Segment_10 = strtok(NULL, " ");		// pci:335
						strtok(NULL, " ");							// QRxLevMin:0\r\n\r\nOK\r\n

						// Handle TAC
						_TAC = 0;
						sscanf(_Segment_5, "TAC:%05u", &_TAC);

						// Handle Cell ID
						_Cell_ID = 0;
						sscanf(_Segment_6, "Id:%lx", &_Cell_ID);

						// Handle RSSI
						_RSSI = 0;
						sscanf(_Segment_8, "PWR:-%03udbm", &_RSSI);

						// Handle PCell ID
						_PCell_ID = 0;
						sscanf(_Segment_10, "pci:%lx", &_PCell_ID);

						// Calculate Signal Level
						_Signal_Level = 0;
						_Signal_Level = this->RSSI_to_Signal_Quality(_RSSI * -1);

						// End Function
						return(true);

					}

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SCFG_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SCFGEXT_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SCFGEXT2_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				GSM_Serial->print(F("AT#SCFGEXT2="));
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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SCFGEXT3_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Firewall Add/Remove Function
			bool FRWL(const bool _Function_Type, const uint8_t _Action, const char *_IP_Addr) {

				// SET Function
				if (_Function_Type == SET) {

					// Clear UART Buffer
					this->Clear_UART_Buffer();

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Send UART Command
					GSM_Serial->print(F("AT#FRWL="));
					GSM_Serial->print(_Action);
					if (_Action != 2) GSM_Serial->print(F(",\""));
					if (_Action != 2) GSM_Serial->print(_IP_Addr);
					if (_Action != 2) GSM_Serial->print(F("\",\"255.255.255.255\""));
					GSM_Serial->write(0x0D);
					GSM_Serial->write(0x0A);

					// Declare Buffer Object
					Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_FRWL_, 15};

					// Declare Buffer Variable
					char _Buffer_Variable[_Buffer.Size];

					// Clear Buffer Variable
					memset(_Buffer_Variable, '\0', _Buffer.Size);

					// Declare Response
					this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_ICMP_, 15};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Ping Function
			bool Ping(const char * _IP) {

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_Ping_, 150};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// \r\n#PING: 01,"159.89.111.150",0,51\r\n
				// \r\nOK\r\n

				// End Function
				return(_Buffer.Response == _AT_OK_);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CCLK_, 40};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					/*
					--> AT+CCLK?\r\n
					<-- \r\n+CCLK: "23/08/02,11:18:16+12"\r\n\r\nOK\r\n
					*/

					// Handle Variables
					uint8_t _Variable_Count = sscanf(_Buffer_Variable, "\r\n+CCLK: \"%02hhu/%02hhu/%02hhu,%02hhu:%02hhu:%02hhu+%02hhu\"\r\n\r\nOK\r\n", &_Year, &_Month, &_Day, &_Hour, &_Minute, &_Second, &_Time_Zone);

					// Control for Variables
					if (_Year > 99 || _Year < 22 || _Month > 12 || _Day > 31 || _Hour > 24 || _Minute > 59 || _Second > 59) return false;

					// Control for Variable
					if (_Variable_Count == 7) return true;

				}

				// End Function
				return(false);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CTZU_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_NITZ_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_CCLKMODE_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SD_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SH_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SL_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Socket Answer Function
			// TODO: Düzenlenecek
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

				// Declare Buffer Object
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SA_, 30};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Control for Response
				if (_Buffer.Response == _AT_SRING_) {

					// Declare Buffer Variable
					uint16_t _Buffer;

					// Handle Variables
					if (sscanf(_Buffer_Variable, "\r\nOK\r\n\r\nSRING: %01u,%03u\r\n", &_Buffer, &_Length) == 2) return(true);

				}

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SS_, 60};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n#SS: 2,4,5.26.173.230,80\r\n\r\nOK\r\n
					// \r\n#SS: 2,2,5.26.173.230,80,213.14.250.214,54883\r\n\r\nOK\r\n
					// \r\n#SS: 2,2,000.000.000.000,80,000.000.000.000,54883\r\n\r\nOK\r\n

					// Handle State
					_State = (_Buffer_Variable[9] - 48);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SO_, 20};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SI_, 50};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n#SI: 2,51,0,13900,0\r\n\r\nOK\r\n

					// Handle Variables
					strtok(_Buffer_Variable, ",");				// \r\n#SI: 2
					strtok(NULL, ",");							// 51
					strtok(NULL, ",");							// 0
					char * _Buffer_Size = strtok(NULL, ",");	// 13900

					// Handle Buffer Size
					_Data_Buffer = 0;
					sscanf(_Buffer_Size, "%05u", &_Data_Buffer);

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// Socket Pack Send Function
			bool SSEND(const uint8_t _ConnID, const char * _Data_Pack) {

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SSEND_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_SD_PROMPT_) {

					// Send Delay
					delay(10);

					// Send Data Pack
					GSM_Serial->print(_Data_Pack);

					// Print End Char
					GSM_Serial->print((char)26);

					// Declare Buffer Object
					Serial_Buffer _Buffer_Send = {0, 0, 0, _TIMEOUT_SSEND_, 7};

					// Command Chain Delay (Advice by Telit)
					delay(_AT_WAIT_DELAY_);

					// Declare Buffer Variable
					char _Buffer_Send_Variable[_Buffer_Send.Size];

					// Clear Buffer Variable
					memset(_Buffer_Send_Variable, '\0', _Buffer_Send.Size);

					// Declare Response
					this->Read_UART_Buffer(&_Buffer_Send, _Buffer_Send_Variable);

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
				delay(_AT_WAIT_DELAY_);

				// Send UART Command
				GSM_Serial->print(F("AT#SRECV="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_MaxByte);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Buffer Object
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SRECV_, 1023};

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Data);

				// Handle for Response
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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_FTPCLOSE_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_FTPCWD_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Get FTP File Size Function
			bool FTPFSIZE(const uint16_t _FileName, uint32_t & _Length) {

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_FTPFSIZE_, 30};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {

					// \r\n#FTPFSIZE: 174945\r\n\r\nOK\r\n

					// Handle Variables
					sscanf(_Buffer_Variable, "\r\n#FTPFSIZE:%lu\r\n\r\nOK\r\n", &_Length);

					// End Function
					return (true);

				}

				// End Function
				return(false);

			}

			// Get FTP File to Buffer function
			bool FTPGETPKT(const uint16_t _FileName, const uint8_t _ViewMode) {

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_FTPGETPKT_, 20};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_FTPOPEN_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Get FTP File From Buffer Function
			bool FTPRECV(const uint16_t _Size, uint16_t & _ReadSize, uint8_t & _State, char * _Data) {

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_FTPRECV_, 255};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Handle for Response
				if (_Buffer.Response == _AT_OK_) {
					
					// \r\n#FTPRECV: 200\r\n20202055\r\n:100BA00020202020000D0A002C002C002C00415495\r\n:100BB00023534C3D000D0A004154234532534C52FF\r\n:100BC000493D000D0A00415423534C4544534156BE\r\n:100BD000000D0A00415423534C45443D000D0A00CA\r\n:100BE0004\r\n\r\nOK\r\n
					// \r\n+CME ERROR: 614\r\n	

					// Clear Size Variable
					_ReadSize = 0;

					// Control for Response
					if (_Buffer.Response) {

						// Parse Size
						sscanf(_Buffer_Variable, "\r\n#FTPRECV: %03u\r\n", &_ReadSize);

						// Calculate Header Length
						const int _Start = 14 + (int)log10(_ReadSize) + 1;

						// Parse Data
						for (uint16_t i = _Start; i < (_Start + _ReadSize); i++) {

							// Get Data
							_Data[_Buffer.Data_Order] = _Buffer_Variable[i];

							// Increase Data Order
							_Buffer.Data_Order += 1;

						}

					} else {

						// \r\n+CME ERROR: 614\r\n

						// Declare CMEE Variable
						int _CME = 0;

						// Control for CME Error
						sscanf(_Buffer_Variable, "\r\n+CME ERROR: %03d\r\n", &_CME);

						// Handle CME Error
						if (_CME == 606) _State = 1;
						if (_CME == 614) _State = 2;

					}

					// Control for Timeout
					if (!_Buffer.Response) return(false);

					// End Function
					if (_ReadSize == _Buffer.Data_Order and _ReadSize != 0) {

						// Return Function
						return (true);

					} else {

						// Wait Delay
						delay(1000);

						// Return Function
						return (false);

					}

					// End Function
					return(false);

				}

				// End Function
				return(false);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_FTPTO_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_FTPTYPE_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_Z_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Detech From Network and Shut Down Modem Function
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
				Serial_Buffer _Buffer = {0, 0, 0, _TIMEOUT_SHDN_, 7};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// End Function
				return(_Buffer.Response == _AT_OK_);

			}

			// Find SRING
			bool SRING(const uint16_t _TimeOut) {

				// Declare Buffer Object
				Serial_Buffer _Buffer = {0, 0, 0, _TimeOut, 20};

				// Declare Buffer Variable
				char _Buffer_Variable[_Buffer.Size];

				// Clear Buffer Variable
				memset(_Buffer_Variable, '\0', _Buffer.Size);

				// Declare Response
				this->Read_UART_Buffer(&_Buffer, _Buffer_Variable);

				// Control for SRING
				if (_Buffer.Response == _AT_SRING_) return(true);

				// End Function
				return(false);

			}

	};

#endif /* defined(AT_Command) */
