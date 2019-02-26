/*

 */

#ifndef Display_h
#define Display_h

#include "Arduino.h"
#include "U8g2lib.h"
#include "u8g2.h"
#include <Wire.h>


#define SYS_FONT u8g2_font_6x12_tf       // 7 px high
#define BIG_TEMP_FONT u8g2_font_fub30_tf //30px hieght

#define OLED_CLOCK_PIN GPIO_NUM_22 //RHS_P_14 SCL
#define OLED_DATA_PIN GPIO_NUM_21  //RHS_P_11 SDA

/**
 */
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
