/**
 * Example 02 — Network Connection
 *
 * Demonstrates connecting the modem to the cellular network:
 *   - Check SIM and read ICCID
 *   - Wait for network registration (CREG)
 *   - Configure APN and activate PDP context (SGACT)
 *   - Print assigned IP address
 *
 * Hardware : Telit LE910C1-EUX connected to Serial1
 * Board    : Arduino Mega / any board with a spare HardwareSerial
 */

#include <AT_Command.h>

#define APN  "internet"   // Replace with your operator's APN

class Modem : public LE910C1_EUX {
public:
    explicit Modem(Stream &s) : LE910C1_EUX(s) {}

    bool begin() {
        if (!AT())       return false;
        if (!ATE(false)) return false;
        if (!CMEE(1))    return false;
        return true;
    }

    bool checkSIM() {
        uint8_t pin_status = _SIM_UNKNOWN_;
        if (!CPIN(pin_status)) return false;
        if (pin_status != _SIM_READY_) {
            Serial.println(F("SIM not ready — check PIN/PUK"));
            return false;
        }

        char iccid[21];
        memset(iccid, '\0', sizeof(iccid));
        if (CCID(iccid)) {
            Serial.print(F("ICCID : "));
            Serial.println(iccid);
        }
        return true;
    }

    // Block until registered, or timeout (ms)
    bool waitRegistered(uint32_t timeout_ms = 60000) {
        uint8_t mode = 0, stat = 0;
        const uint32_t t0 = millis();

        while (millis() - t0 < timeout_ms) {
            if (CREG(_AT_GET_, mode, stat)) {
                // stat 1 = home, 5 = roaming
                if (stat == 1 || stat == 5) {
                    Serial.print(F("Registered (stat="));
                    Serial.print(stat);
                    Serial.println(')');
                    return true;
                }
            }
            delay(2000);
        }
        Serial.println(F("Registration timeout"));
        return false;
    }

    bool activatePDP(const char * apn, uint8_t ip[4]) {
        // Configure PDP context 1
        if (!CGDCONT(1, "IP", apn)) {
            Serial.println(F("CGDCONT failed"));
            return false;
        }

        // Activate and get assigned IP
        if (!SGACT(1, true, ip)) {
            Serial.println(F("SGACT failed"));
            return false;
        }
        return true;
    }
};

Modem modem(Serial1);

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);

    Serial.println(F("--- Network Connect Example ---"));

    if (!modem.begin()) { Serial.println(F("Modem init failed")); return; }

    if (!modem.checkSIM()) return;

    if (!modem.waitRegistered()) return;

    uint8_t ip[4] = {0, 0, 0, 0};
    if (!modem.activatePDP(APN, ip)) return;

    Serial.print(F("IP address : "));
    Serial.print(ip[0]); Serial.print('.');
    Serial.print(ip[1]); Serial.print('.');
    Serial.print(ip[2]); Serial.print('.');
    Serial.println(ip[3]);

    Serial.println(F("Network ready."));
}

void loop() {}
