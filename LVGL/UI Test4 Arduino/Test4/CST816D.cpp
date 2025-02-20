#include "CST816D.h"
#include <Wire.h>

CST816D::CST816D(int8_t sda_pin, int8_t scl_pin, int8_t rst_pin, int8_t int_pin) {
    _sda = sda_pin;
    _scl = scl_pin;
    _rst = rst_pin;
    _int = int_pin;
}

void CST816D::begin(void) {
    if (_sda != -1 && _scl != -1) {
        Wire.begin(_sda, _scl);
    } else {
        Wire.begin();
    }
    if (_rst != -1) {
        pinMode(_rst, OUTPUT);
        digitalWrite(_rst, LOW);
        delay(10);
        digitalWrite(_rst, HIGH);
        delay(300);
    }
    i2c_write(0xFE, 0xFF); // Example to disable low power mode
}

bool CST816D::getTouch(uint16_t *x, uint16_t *y, uint8_t *gesture) {
    bool FingerIndex = false;
    FingerIndex = (bool)i2c_read(0x02);
    *gesture = i2c_read(0x01);

    // Explicitly set recognized gestures
    switch (*gesture) {
        case 0x01: *gesture = SlideDown; break;
        case 0x02: *gesture = SlideUp; break;
        case 0x03: *gesture = SlideLeft; break;
        case 0x04: *gesture = SlideRight; break;
        case 0x05: *gesture = SingleTap; break;
        case 0x0B: *gesture = DoubleTap; break;
        case 0x0C: *gesture = LongPress; break;
        default: *gesture = None; break;
    }

    uint8_t data[4];
    i2c_read_continuous(0x03, data, 4);
    *x = ((data[0] & 0x0F) << 8) | data[1];
    *y = ((data[2] & 0x0F) << 8) | data[3];
    return FingerIndex;
}

// I2C Helper Functions
uint8_t CST816D::i2c_read(uint8_t addr) {
    uint8_t rdData;
    Wire.beginTransmission(I2C_ADDR_CST816D);
    Wire.write(addr);
    Wire.endTransmission(false);
    Wire.requestFrom(I2C_ADDR_CST816D, 1);
    rdData = Wire.read();
    return rdData;
}

uint8_t CST816D::i2c_read_continuous(uint8_t addr, uint8_t *data, uint32_t length) {
    Wire.beginTransmission(I2C_ADDR_CST816D);
    Wire.write(addr);
    Wire.endTransmission(false);
    Wire.requestFrom(I2C_ADDR_CST816D, length);
    for (uint32_t i = 0; i < length; i++) {
        data[i] = Wire.read();
    }
    return 0;
}

void CST816D::i2c_write(uint8_t addr, uint8_t data) {
    Wire.beginTransmission(I2C_ADDR_CST816D);
    Wire.write(addr);
    Wire.write(data);
    Wire.endTransmission();
}
