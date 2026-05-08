/**
 * Example 03 — HTTP POST via TCP Socket
 *
 * Demonstrates sending a JSON payload to a server and reading the response:
 *   - Configure socket (SCFG, SCFGEXT)
 *   - Open TCP connection (ATSD)
 *   - Send HTTP POST with JSON body (SSEND)
 *   - Wait for SRING notification then read response (SRECV)
 *   - Close socket (SH)
 *
 * This example assumes the modem is already connected to the network.
 * Run example 02 first or integrate the network bring-up steps.
 *
 * Hardware : Telit LE910C1-EUX connected to Serial1
 * Board    : Arduino Mega / any board with a spare HardwareSerial
 */

#include <AT_Command.h>

// Server settings — override here or via Config.h before the #include above
#ifndef _PostMan_Server_
    #define _PostMan_Server_   "165.227.154.147"
#endif
#ifndef _PostMan_EndPoint_
    #define _PostMan_EndPoint_ "/"
#endif

#define CONN_ID  1   // Socket connection ID (1–6)

class Modem : public LE910C1_EUX {
public:
    explicit Modem(Stream &s) : LE910C1_EUX(s) {}

    bool configureSocket() {
        // SCFG: connID, cid, pktSz, maxTo, connTo, txTo
        if (!SCFG(CONN_ID, 1, 300, 90, 600, 50)) return false;

        // SCFGEXT: connID, srMode, recvDataMode, keepAlive, listenAutoRsp, sendDataMode
        if (!SCFGEXT(CONN_ID, 1, false, 0, false, false)) return false;

        return true;
    }

    bool connectTCP(const char * server, uint8_t port) {
        // AT#SD: cid, protocol(TCP=0), port, ip, closureType, localPort, connMode
        if (!ATSD(CONN_ID, _AT_TCP_, server, port, _CONNECTION_AUTO_CLOSE_, 0, _CONNECTION_COMMAND_)) {
            Serial.println(F("TCP connect failed"));
            return false;
        }
        Serial.println(F("TCP connected"));
        return true;
    }

    bool sendPost(const char * payload) {
        // SSEND with _HTTP_POST_ builds the full HTTP/1.1 header automatically
        if (!SSEND(CONN_ID, _HTTP_POST_, payload)) {
            Serial.println(F("SSEND failed"));
            return false;
        }
        return true;
    }

    bool readResponse(char * out, uint16_t out_size) {
        // Wait for SRING notification (modem signals data is ready)
        uint16_t data_len = 0;
        if (!SRING(data_len)) {
            Serial.println(F("No SRING received"));
            return false;
        }
        Serial.print(F("Incoming bytes: "));
        Serial.println(data_len);

        // Read at most out_size - 1 bytes so there's room for the null terminator
        uint16_t to_read = (data_len < out_size - 1) ? data_len : (out_size - 1);
        if (!SRECV(CONN_ID, to_read, out)) {
            Serial.println(F("SRECV failed"));
            return false;
        }
        return true;
    }

    void closeSocket() {
        SH(CONN_ID);
    }
};

Modem modem(Serial1);

// Response buffer — 512 bytes is enough for most HTTP responses
char response[512];

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);

    Serial.println(F("--- HTTP POST Example ---"));

    if (!modem.configureSocket()) {
        Serial.println(F("Socket config failed"));
        return;
    }

    if (!modem.connectTCP(_PostMan_Server_, 80)) return;

    const char * payload = "{\"device\":\"arduino\",\"value\":42}";
    Serial.print(F("Sending: "));
    Serial.println(payload);

    if (!modem.sendPost(payload)) {
        modem.closeSocket();
        return;
    }

    memset(response, '\0', sizeof(response));
    if (modem.readResponse(response, sizeof(response))) {
        Serial.println(F("--- Server Response ---"));
        Serial.println(response);
    }

    modem.closeSocket();
    Serial.println(F("Done."));
}

void loop() {}
