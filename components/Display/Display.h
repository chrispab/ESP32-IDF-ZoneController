#ifndef Display_h
#define Display_h

#include "Arduino.h"
#include "U8g2lib.h"
#include "u8g2.h"
#include <Wire.h>

#define SYS_FONT u8g2_font_6x12_tf       // 7 px high
#define FONT1 u8g2_font_7x13_t_symbols //cap A - 9px hi,
#define BIG_TEMP_FONT u8g2_font_fub30_tf //30px hieght

#define CHAR_WIDTH 6
#define CHAR_HIEGHT 9
  #define LINE_GAP 3
#define LINE_HIEGHT (CHAR_HIEGHT + LINE_GAP)
#define XPIX 128
#define YPIX 64
#define LINES (YPIX/LINE_HIEGHT)
#define CHARS_PER_LINE ((XPIX/CHAR_WIDTH)+10)



#define OLED_CLOCK_PIN GPIO_NUM_22 //RHS_P_14 SCL
#define OLED_DATA_PIN GPIO_NUM_21  //RHS_P_11 SDA

class Display : public U8G2 {
  public:
    //Display(const u8g2_cb_t *rotation, uint8_t reset = U8X8_PIN_NONE, uint8_t clock = U8X8_PIN_NONE, uint8_t data = U8X8_PIN_NONE);
    //Display(const u8g2_cb_t, uint8_t reset, uint8_t clock, uint8_t data);
    Display(const u8g2_cb_t *rotation, uint8_t reset, uint8_t clock, uint8_t data);

    void writeLine(int lineNumber, const char *lineText);
    void refresh(void);
    void wipe(void);
};

#endif
