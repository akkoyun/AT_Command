/**
 * Example 04 — Signal Quality, Network Info & Time Sync
 *
 * Demonstrates reading diagnostic information from the modem:
 *   - Signal quality in dBm and bars (CSQ)
 *   - 4G cell info: MCC, MNC, TAC, CID, RSSI (RFSTS)
 *   - Enable automatic timezone update (CTZU)
 *   - Read current date/time from network (CCLK)
 *   - Ping a remote host and measure round-trip time
 *
 * This example assumes the modem is already connected to the network.
 *
 * Hardware : Telit LE910C1-EUX connected to Serial1
 * Board    : Arduino Mega / any board with a spare HardwareSerial
 */

#include <AT_Command.h>

#define PING_HOST  "8.8.8.8"   // Google DNS — replace if needed

class Modem : public LE910C1_EUX {
public:
    explicit Modem(Stream &s) : LE910C1_EUX(s) {}

    bool begin() {
        if (!AT())       return false;
        if (!ATE(false)) return false;
        return true;
    }

    void printSignal() {
        uint16_t rssi_dbm = 0;
        if (!CSQ(rssi_dbm)) {
            Serial.println(F("CSQ failed"));
            return;
        }

        Serial.print(F("Signal   : -"));
        Serial.print(rssi_dbm);
        Serial.print(F(" dBm  ("));

        // RSSI_to_Signal_Quality is private — reproduce the mapping here
        uint8_t bars;
        int16_t r = -(int16_t)rssi_dbm;
        if      (r >= -65) bars = 5;
        else if (r >= -75) bars = 4;
        else if (r >= -85) bars = 3;
        else if (r >= -95) bars = 2;
        else               bars = 1;

        Serial.print(bars);
        Serial.println(F("/5 bars)"));
    }

    void printCellInfo() {
        uint16_t mcc = 0, mnc = 0, rssi = 0, tac = 0;
        uint8_t  signal_level = 0;
        uint32_t cid = 0;

        // _CONNECTION_4G_ is the only type fully implemented in RFSTS
        if (!RFSTS(_CONNECTION_4G_, mcc, mnc, rssi, signal_level, tac, cid)) {
            Serial.println(F("RFSTS failed (4G not active?)"));
            return;
        }

        Serial.print(F("Operator : "));
        Serial.print(mcc);
        Serial.print('-');
        Serial.println(mnc);

        Serial.print(F("TAC      : "));
        Serial.println(tac, HEX);

        Serial.print(F("Cell ID  : "));
        Serial.println(cid, HEX);
    }

    void syncAndPrintTime() {
        // Tell modem to sync clock from network automatically
        if (!CTZU(true)) {
            Serial.println(F("CTZU failed"));
        }

        uint8_t yy = 0, mo = 0, dd = 0, hh = 0, mm = 0, ss = 0, tz = 0;
        if (!CCLK(yy, mo, dd, hh, mm, ss, tz)) {
            Serial.println(F("CCLK failed"));
            return;
        }

        // Print as ISO 8601: 20YY-MM-DD HH:MM:SS UTC±TZ/4h
        char buf[32];
        snprintf(buf, sizeof(buf), "20%02u-%02u-%02u %02u:%02u:%02u", yy, mo, dd, hh, mm, ss);
        Serial.print(F("Time     : "));
        Serial.print(buf);

        // Timezone is stored as quarter-hours; negative encoded as two's complement uint8
        int8_t tz_signed = (int8_t)tz;
        Serial.print(F("  UTC"));
        if (tz_signed >= 0) Serial.print('+');
        Serial.print((float)tz_signed / 4.0f, 1);
        Serial.println('h');
    }

    void pingHost(const char * host) {
        uint16_t rtt_ms = 0;
        Serial.print(F("Ping "));
        Serial.print(host);
        Serial.print(F(" ... "));

        if (Ping(host, rtt_ms)) {
            Serial.print(rtt_ms);
            Serial.println(F(" ms"));
        } else {
            Serial.println(F("no reply"));
        }
    }
};

Modem modem(Serial1);

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);

    Serial.println(F("--- Signal & Time Example ---"));

    if (!modem.begin()) {
        Serial.println(F("Modem init failed"));
        return;
    }

    modem.printSignal();
    modem.printCellInfo();
    modem.syncAndPrintTime();
    modem.pingHost(PING_HOST);
}

void loop() {
    // Refresh signal every 10 seconds
    modem.printSignal();
    delay(10000);
}
