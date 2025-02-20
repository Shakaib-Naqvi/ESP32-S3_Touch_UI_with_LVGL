#include "CST816S.h"

// Konstruktor
CST816S::CST816S(int sda, int scl, int rst_io, int int_io) {
    Wire.begin(sda, scl);
    if (rst_io >= 0) {
        pinMode(rst_io, OUTPUT);
        digitalWrite(rst_io, HIGH);
    }
}

// Touchscreen-Initialisierung
bool CST816S::begin(void) {
    Serial.println("CST816S: Initialisierung gestartet...");
    
    // Beispiel für eine I2C-Kommunikation zur Überprüfung
    Wire.beginTransmission(0x15);  // Beispiel-Adresse des Touch-Controllers
    if (Wire.endTransmission() != 0) {
        Serial.println("CST816S: I2C-Gerät nicht gefunden!");
        return false;
    }

    Serial.println("CST816S: Erfolgreich initialisiert!");
    return true;
}

// Touchscreen-Abfrage
bool CST816S::getTouch(uint16_t *x, uint16_t *y, uint8_t *gesture) {
    Wire.beginTransmission(0x15);  // Adresse des Touch-Controllers
    Wire.write(0x00);  // Befehl zur Abfrage des Touch-Status
    Wire.endTransmission();
    
    Wire.requestFrom(0x15, 5);  // 5 Bytes anfordern
    if (Wire.available() < 5) {
        return false;
    }

    uint8_t status = Wire.read();  // Status-Byte
    *x = Wire.read() << 8 | Wire.read();
    *y = Wire.read() << 8 | Wire.read();
    *gesture = status & 0x0F;

    return (status & 0x80) ? true : false;  // Bit 7 zeigt Touch-Status
}
