/**
 * Example 01 — Modem Initialization
 *
 * Demonstrates basic modem startup sequence:
 *   - Echo off, error reporting on
 *   - Read manufacturer, model, firmware version, IMEI
 *   - Check SIM PIN status
 *
 * Hardware : Telit LE910C1-EUX connected to Serial1 (TX1/RX1)
 * Board    : Arduino Mega / any board with a spare HardwareSerial
 */

#include <AT_Command.h>

// Inherit LE910C1_EUX to access the protected AT command functions.
class Modem : public LE910C1_EUX {
public:
    explicit Modem(Stream &s) : LE910C1_EUX(s) {}

    bool begin() {
        // Verify communication
        if (!AT()) return false;

        // Disable echo so responses are clean
        if (!ATE(false)) return false;

        // Enable verbose CME error codes
        if (!CMEE(1)) return false;

        // Set voice/data class to data-only
        if (!FCLASS(0)) return false;

        return true;
    }

    void printInfo() {
        // --- Manufacturer ---
        uint8_t manufacturer = _MODEM_MANUFACTURER_UNKNOWN_;
        if (CGMI(manufacturer)) {
            Serial.print(F("Manufacturer : "));
            Serial.println(manufacturer == _MODEM_MANUFACTURER_TELIT_ ? F("Telit") : F("Unknown"));
        }

        // --- Model ---
        uint8_t model = _MODEM_MODEL_UNKNOWN_;
        if (CGMM(model)) {
            Serial.print(F("Model code   : "));
            Serial.println(model);
        }

        // --- Firmware ---
        char firmware[48];
        memset(firmware, '\0', sizeof(firmware));
        if (SWPKGV(firmware)) {
            Serial.print(F("Firmware     : "));
            Serial.println(firmware);
        }

        // --- IMEI ---
        char imei[16];
        memset(imei, '\0', sizeof(imei));
        if (CGSN(imei)) {
            Serial.print(F("IMEI         : "));
            Serial.println(imei);
        }

        // --- SIM PIN status ---
        uint8_t pin_status = _SIM_UNKNOWN_;
        if (CPIN(pin_status)) {
            Serial.print(F("SIM status   : "));
            switch (pin_status) {
                case _SIM_READY_: Serial.println(F("READY"));   break;
                case _SIM_PIN_:   Serial.println(F("PIN req")); break;
                case _SIM_PUK_:   Serial.println(F("PUK req")); break;
                default:          Serial.println(F("Unknown")); break;
            }
        }
    }
};

Modem modem(Serial1);

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);

    Serial.println(F("--- Modem Init Example ---"));

    if (!modem.begin()) {
        Serial.println(F("Modem init FAILED"));
        return;
    }
    Serial.println(F("Modem OK"));
    modem.printInfo();
}

void loop() {}
