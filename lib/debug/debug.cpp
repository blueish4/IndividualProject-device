#include <Arduino.h>
#include <Wire.h>
#include <U8x8lib.h>
#include <string>

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
const int no_of_lines = 2;
std::string dbg_lines[no_of_lines] = {"", ""};

void init_dbg() {
    u8x8.begin();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    Serial.begin(9600);
    Serial.println(F("Starting"));
}

void dbg_print(std::string msg) {
    Serial.println(msg.c_str());
    if (dbg_lines[no_of_lines-1] == "") {
        for (int i=no_of_lines-1;i>0;i--) {
            dbg_lines[i] = dbg_lines[i-1];
        }
    }
    dbg_lines[0] = msg;
    u8x8.clear();
    for(int i=0;i<no_of_lines;i++){
        u8x8.drawString(0, i, dbg_lines[i].c_str());
    }
}