#include <Arduino.h>
#include <Wire.h>
#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

void init_dbg() {
    u8x8.begin();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(0, 1, "LoRaWAN LMiC");
    Serial.begin(115200);
    Serial.println(F("Starting"));
}

void dbg_print(char* msg) {
    Serial.println(msg);
    u8x8.drawString(0, 7, msg);
}