#include <Arduino.h>
#include <Wire.h>
#include <U8x8lib.h>
#include <sstream>
#include <queue>

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);
const int no_of_lines = 6;
std::queue<std::string> dbg_lines = std::queue<std::string>();

std::string to_string(int i) {
    std::ostringstream ss;
    ss << i;
    return ss.str();
}

void init_dbg() {
    u8x8.begin();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    Serial.begin(9600);
    Serial.println(F("Starting"));
}

void dbg_print(std::string msg) {
    Serial.println(msg.c_str());
    while(dbg_lines.size() >= no_of_lines) {
        dbg_lines.pop();
        Serial.println("shrinking to max_size");
    }
    dbg_lines.push(msg);
    u8x8.clear();
    std::queue<std::string> shown = std::queue<std::string>();
    int i = 0;
    while (dbg_lines.size()) {
        auto line = dbg_lines.front();
        dbg_lines.pop();
        shown.push(line);
        u8x8.drawString(0, i++, line.c_str());
    }
    while (shown.size()) {
        dbg_lines.push(shown.front());
        shown.pop();
    }
}

void print_fft(int* bands, int length) {
    std::string buffer = "";
    for(int i=0;i<length;i++){
        buffer += to_string(bands[i]) + " ";
        bands[i] = 0;
    }
    buffer += "\n";
    dbg_print(buffer);
}

void print_fft(uint16_t* bands, int length) {
    std::string buffer = "";
    for(int i=0;i<length;i++){
        buffer += to_string(bands[i]) + " ";
        bands[i] = 0;
    }
    buffer += "\n";
    dbg_print(buffer);
}