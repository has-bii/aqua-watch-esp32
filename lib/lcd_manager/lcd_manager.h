#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

class LCDManager
{
private:
    LiquidCrystal_I2C lcd;

public:
    LCDManager();

    void begin();

    void print(const String &line1, const String &line2, unsigned long duration = 0);
};