#include <Arduino.h>
#include "Display.h"
//#include <U8g2lib.h>
#include "U8g2lib.h"
#include "u8g2.h"

#include <stdlib.h> // for dtostrf(FLOAT,WIDTH,PRECSISION,BUFFER);
#include <Wire.h>
#include <U8g2lib.h>
// OLED display stuff
//this wpuld be the constructor to use if creating object directly without 'display' wrapper
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE,
// /* clock=*/22, /* data=*/21); // ESP32 Thing, HW I2C with pin remapping
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C(rotation, [reset [, clock, data]]) 	full framebuffer, size = 1024 bytes


char displayLine[LINES][CHARS_PER_LINE]; // m lines of n chars +terminator for dispaly store

Display::Display(const u8g2_cb_t *rotation, uint8_t reset, uint8_t clock,
                 uint8_t data)
    : U8G2() {
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &u8g2, rotation, u8x8_byte_arduino_hw_i2c, u8x8_gpio_and_delay_arduino);
    u8x8_SetPin_HW_I2C(getU8x8(), U8X8_PIN_NONE, clock, data);
}

// redraw the display with contents of displayLine array
void Display::refresh(void) {
    clearBuffer();
    for (int i = 0; i < LINES; i++) {
        drawStr(0, ((i + 1) * LINE_HIEGHT), displayLine[i]);
    }
    sendBuffer();
}

void Display::wipe(void) {
    clearBuffer();
    for (int i = 0; i < LINES; i++) {
        strcpy(displayLine[i], " ");
        drawStr(0, ((i + 1) * LINE_HIEGHT), displayLine[i]);
    }
    sendBuffer();
}

// add-update a line of text in the display text buffer
void Display::writeLine(int lineNumber, const char *lineText) {
    // update a line in the diaplay text buffer
    strcpy(displayLine[lineNumber - 1], lineText);
}
