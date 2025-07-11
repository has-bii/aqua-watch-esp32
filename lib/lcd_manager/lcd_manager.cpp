#include <lcd_manager.h>

LCDManager::LCDManager() : lcd(LCD_ADDR, LCD_COLS, LCD_ROWS)
{
}

void LCDManager::print(const String &line1, const String &line2, unsigned long duration)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    if (!line2.isEmpty())
    {
        lcd.setCursor(0, 1);
        lcd.print(line2);
    }
    if (duration > 0)
    {
        delay(duration); // Wait for the specified duration before clearing the display
    }
}

void LCDManager::begin()
{
    lcd.init();
    lcd.backlight();
}