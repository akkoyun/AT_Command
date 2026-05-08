/* *******************************************************************************
 *  Copyright (C) 2014-2026 Mehmet Gunce Akkoyun Can not be copied and/or
 *	distributed without the express permission of Mehmet Gunce Akkoyun.
 *
 *	Library				: AT_Command
 *	Version				: 02.02.00
 *
 *	Example				: 05_WebSocket
 *	Description			: WebSocket connect/send/receive/ping-pong/close.
 *						  Demonstrates plain ws:// and secure wss:// connections.
 *						  Shows correct server-initiated ping handling (WSPONG).
 *
 *********************************************************************************/

#include <Arduino.h>
#include <AT_Command.h>

// ---- Connection mode ----------------------------------------------------------
// Set WS_USE_SSL to true for wss://, false for ws://
#define WS_USE_SSL  false

// ---- Server configuration (change to your server) ----------------------------
#if WS_USE_SSL
    #define WS_HOST  "echo.websocket.events"   // wss://echo.websocket.events/
    #define WS_PORT  443
#else
    #define WS_HOST  "echo.websocket.events"   // ws://echo.websocket.events/
    #define WS_PORT  80
#endif
#define WS_PATH  "/"
#define APN      "internet"
// -------------------------------------------------------------------------------

class Modem : public LE910C1_EUX {

	public:

		explicit Modem(Stream &s) : LE910C1_EUX(s) {}

		void run() {

			// ----------------------------------------------------------------
			// 1. Modem initialisation
			// ----------------------------------------------------------------
			AT();
			ATE(false);
			CMEE(1);
			FCLASS(0);

			// ----------------------------------------------------------------
			// 2. SIM and network registration
			// ----------------------------------------------------------------
			uint8_t _PIN_State = 0;
			if (!CPIN(_PIN_State) || _PIN_State != _SIM_READY_) {
				Serial.println(F("SIM not ready"));
				return;
			}

			uint8_t _Reg_Mode = 0, _Reg_Stat = 0;
			CREG(_AT_GET_, _Reg_Mode, _Reg_Stat);

			// ----------------------------------------------------------------
			// 3. PDP context and IP activation
			// ----------------------------------------------------------------
			uint8_t _IP[4] = {};
			CGDCONT(1, "IP", APN);
			if (!SGACT(1, true, _IP)) {
				Serial.println(F("SGACT failed"));
				return;
			}

			// ----------------------------------------------------------------
			// 4. Socket configuration
			// ----------------------------------------------------------------
			SCFG(1, 1, 300, 90, 600, 50);
			SCFGEXT(1, 1, false, 0, false, false);

			// ----------------------------------------------------------------
			// 5. Open WebSocket connection (ws:// or wss://)
			// ----------------------------------------------------------------
			Serial.print(WS_USE_SSL ? F("Connecting wss://") : F("Connecting ws://"));
			Serial.print(F(WS_HOST));
			Serial.println(F(WS_PATH));

			if (!WSOPEN(1, WS_HOST, WS_PORT, WS_PATH, WS_USE_SSL)) {
				Serial.println(F("WSOPEN failed"));
				return;
			}
			Serial.println(F("WebSocket connected"));

			// ----------------------------------------------------------------
			// 6. Send a JSON message (payloads up to 65535 bytes supported)
			// ----------------------------------------------------------------
			const char * _Payload = "{\"type\":\"hello\",\"v\":1}";

			if (!WSSEND(1, _Payload, WS_USE_SSL)) {
				Serial.println(F("WSSEND failed"));
				WSCLOSE(1, WS_USE_SSL);
				return;
			}
			Serial.print(F("Sent: "));
			Serial.println(_Payload);

			// ----------------------------------------------------------------
			// 7. Receive server response
			// ----------------------------------------------------------------
			char _Buf[256] = {};
			uint8_t _Opcode = 0;

			if (WSRECV(1, _Buf, sizeof(_Buf), _Opcode, WS_USE_SSL)) {

				if (_Opcode == _WS_OPCODE_PING_) {
					// Server sent keepalive ping — must reply with pong (RFC 6455 §5.5.2)
					WSPONG(1, WS_USE_SSL);
					Serial.println(F("Server ping -> pong sent"));

				} else if (_Opcode == _WS_OPCODE_TEXT_ || _Opcode == _WS_OPCODE_BINARY_) {
					Serial.print(F("Received: "));
					Serial.println(_Buf);
				}

			}

			// ----------------------------------------------------------------
			// 8. Client-initiated ping / pong keepalive
			// ----------------------------------------------------------------
			if (WSPING(1, WS_USE_SSL)) {

				memset(_Buf, '\0', sizeof(_Buf));
				_Opcode = 0;

				if (WSRECV(1, _Buf, sizeof(_Buf), _Opcode, WS_USE_SSL) && _Opcode == _WS_OPCODE_PONG_) {
					Serial.println(F("Pong received"));
				}

			}

			// ----------------------------------------------------------------
			// 9. Close WebSocket connection
			// ----------------------------------------------------------------
			WSCLOSE(1, WS_USE_SSL);
			Serial.println(F("WebSocket closed"));

		}

};

Modem modem(Serial1);

void setup() {
	Serial.begin(115200);
	Serial1.begin(115200);
	modem.run();
}

void loop() {}
