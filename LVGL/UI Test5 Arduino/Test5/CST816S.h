#ifndef CST816S_H
#define CST816S_H

#include <Wire.h>
#include <Arduino.h>

class CST816S {
public:
    CST816S(int sda, int scl, int rst_io = -1, int int_io = -1);
    bool begin(void);

    // Touch-Funktion hinzufügen
    bool getTouch(uint16_t *x, uint16_t *y, uint8_t *gesture);
};

#endif // CST816S_H
