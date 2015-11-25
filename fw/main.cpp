#include "mbed.h"
#include "simple_font.h"
#include <cstdarg>

#define WHITE 0xFFFF
#define BLACK 0x0000

typedef uint16_t u16;
typedef uint8_t u8;

class MYLCD {
 private:
  PortOut port;
  DigitalOut wrPin;  // write strobe
  DigitalOut rsPin;  // command/data
  DigitalOut resetPin;
  DigitalOut csPin;
  uint16_t size_x = 480;
  uint16_t size_y = 320;

 public:
  MYLCD(PortName _port, PinName wr, PinName rs, PinName reset, PinName cs)
      : port(_port), wrPin(wr), rsPin(rs), resetPin(reset), csPin(cs) {
    csPin = 0;  // must be low according to 4.1 DBI in datasheet
    reset_init();
  }
  void send_cmd(uint8_t cmd) {
    rsPin = 0;
    wrPin = 0;
    port.write(cmd);
    wrPin = 1;
  }
  void send_data(uint16_t data) {
    rsPin = 1;
    wrPin = 0;
    port.write(data);
    wrPin = 1;
  }
  void setPixelXY(uint16_t color, int x, int y) {
    setXY(y, x, y, x);
    send_data(color);
  }
  void setXY(int x1, int y1, int x2, int y2) {
    send_cmd(0x2a);
    send_data(x1 >> 8);
    send_data(x1);
    send_data(x2 >> 8);
    send_data(x2);
    send_cmd(0x2b);
    send_data(y1 >> 8);
    send_data(y1);
    send_data(y2 >> 8);
    send_data(y2);
    send_cmd(0x2c);
  }
  void fill_screen(uint16_t color = 0xFFFF) {
    for (int y = 0; y < size_y; y++) {
      for (int x = 0; x < size_x; x++) {
        setPixelXY(color, x, y);
      }
    }
  }
  void draw_rect(u16 x, u16 y, u16 w, u16 h, u16 color = WHITE) {
    uint16_t _width = size_x;
    uint16_t _height = size_y;
    // rudimentary clipping (drawChar w/big text requires this)
    if ((x >= _width) || (y >= _height))
      return;
    if ((x + w - 1) >= _width)
      w = _width - x;
    if ((y + h - 1) >= _height)
      h = _height - y;

    SetAddrWindow(x, y, x + w - 1, y + h - 1);

    rsPin = 1;
    for (y = h; y > 0; y--) {
      for (x = w; x > 0; x--) {
        port.write(color);
        wrPin = 0;
        wrPin = 1;
      }
    }
  }

  void SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    send_cmd(0x2A);  // Column addr set
    send_data(x0 >> 8);
    send_data(x0 & 0xFF);  // XSTART
    send_data(x1 >> 8);
    send_data(x1 & 0xFF);  // XEND

    send_cmd(0x2B);  // Row addr set
    send_data(y0 >> 8);
    send_data(y0);  // YSTART
    send_data(y1 >> 8);
    send_data(y1);  // YEND

    send_cmd(0x2C);  // write to RAM
  }
  void printf(u16 x, u16 y, u16 size, u16 color, const char* fmt, ...) {
    char buf[100];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    print(buf, x, y, size, color);
    va_end(args);
  }
  // from ILI9340_Display driver that origins from many sources like Adafruit
  // library
  void print(char* str, u16 x, u16 y, u16 size = 4, u16 color = WHITE) {
    int len = strlen(str);
    for (int pos = 0; pos < len; pos++) {
      draw_char(str[pos], x + 6 * pos * size, y, size, color);
    }
  }
  void draw_char(unsigned char ascii,
                 u16 x,
                 u16 y,
                 u16 size = 4,
                 u16 color = WHITE) {
    char orientation = '3';
    SetAddrWindow(x, y, x + size, y + size);

    if ((ascii < 0x20) || (ascii > 0x7e))  // check for valid ASCII char
    {
      ascii = '?';  // char not supported
    }
    for (unsigned char i = 0; i < 8; i++) {
      unsigned char temp = simpleFont[ascii - 0x20][i];
      for (unsigned char f = 0; f < 8; f++) {
        if ((temp >> f) & 0x01) {
          switch (orientation) {
            case '0':
              draw_rect(x + f * size, y - i * size, size, size, color);
              break;
            case '1':
              draw_rect(x - i * size, x - f * size, size, size, color);
              break;
            case '2':
              draw_rect(x - f * size, y + i * size, size, size, color);
              break;
            case '3':
            default:
              draw_rect(x + i * size, y + f * size, size, size, color);
          }
        }
      }
    }
  }
  void reset_init(void) {
    resetPin = 0;
    wait_ms(5);  // just in case
    resetPin = 1;
    wait_ms(100);  // >100ms 6.2.13 Exit_sleep_omde (11h) and 5.14.3 Programming
                   // sequence

    // set default pin values (just in case)
    wrPin = 0;
    rsPin = 1;

    // Sleep Out
    send_cmd(0x11);
    wait_ms(120);

    // Set EQ
    send_cmd(0xEE);
    send_data(0x02);
    send_data(0x01);
    send_data(0x02);
    send_data(0x01);

    // DIR TIM
    send_cmd(0xED);
    send_data(0x00);
    send_data(0x00);
    send_data(0x9A);
    send_data(0x9A);
    send_data(0x9B);
    send_data(0x9B);
    send_data(0x00);
    send_data(0x00);
    send_data(0x00);
    send_data(0x00);
    send_data(0xAE);
    send_data(0xAE);
    send_data(0x01);
    send_data(0xA2);
    send_data(0x00);

    // set RM, DM
    send_cmd(0xB4);
    send_data(0x00);

    // set Panel Driving
    send_cmd(0xC0);
    send_data(0x00);  // REV SM GS
    send_data(0x3B);  // NL[5:0]
    send_data(0x00);  // SCN[6:0]
    send_data(0x02);  // NDL 0 PTS[2:0]
    send_data(0x11);  // PTG ISC[3:0]

    send_cmd(0xC1);   //
    send_data(0x10);  // line inversion

    // Set Gamma
    send_cmd(0xC8);
    send_data(0x00);  // KP1,KP0
    send_data(0x46);  // KP3,KP2
    send_data(0x12);  // KP5,KP4
    send_data(0x20);  // RP1,RP0
    send_data(0x0c);  // VRP0 01
    send_data(0x00);  // VRP1
    send_data(0x56);  // KN1,KN0
    send_data(0x12);  // KN3,KN2
    send_data(0x67);  // KN5,KN4
    send_data(0x02);  // RN1,RN0
    send_data(0x00);  // VRN0
    send_data(0x0c);  // VRN1 01

    // Set Power
    send_cmd(0xD0);
    send_data(0x44);  // DDVDH :5.28
    send_data(0x42);  // BT VGH:15.84 VGL:-7.92
    send_data(0x06);  // VREG1 4.625V

    // set VCOM
    send_cmd(0xD1);
    send_data(0x43);  // VCOMH
    send_data(0x16);

    // set power for normal mode
    send_cmd(0xD2);
    send_data(0x04);
    send_data(0x22);  // 12

    // set power for partial mode
    send_cmd(0xD3);
    send_data(0x04);
    send_data(0x12);

    // set power for idle mode
    send_cmd(0xD4);
    send_data(0x07);
    send_data(0x12);

    // Set Panel
    send_cmd(0xE9);
    send_data(0x00);

    // Set Frame rate
    send_cmd(0xC5);
    send_data(0x08);  // 61.51Hz

    // set address mode
    send_cmd(0X0036);
    send_data(0X000a);  //ÊúÆÁ²ÎÊý
    send_data(0x003B);  //ºáÆÁ²ÎÊý

    // pixel format
    send_cmd(0X003A);
    send_data(0X0055);  // 16 bit

    // set column address
    send_cmd(0X002A);
    send_data(0X0000);
    send_data(0X0000);
    send_data(0X0001);
    send_data(0X003F);

    // set page address
    send_cmd(0X002B);
    send_data(0X0000);
    send_data(0X0000);
    send_data(0X0001);
    send_data(0X00E0);
    wait_ms(120);

    // set tear on
    send_cmd(0x35);
    send_data(0x00);  // TE ON

    // display On
    send_cmd(0x29);
    wait_ms(5);
  };
};

int main() {
  AnalogIn in(PF_4);
/*
  PortOut port(PortG);
  //BusOut port(PD_8, PD_10, PD_12, PD_14);
  //BusOut port(PD_12);

  while (1) {
    port.write(0xAAAA);
    wait_ms(5);
    port.write(0);
    wait_ms(5);
  }
*/

//calc 2.93 32740
//calc 29300*cur/32740
#define COLOR 0xF0F0
  MYLCD lcd(PortG, PD_14, PD_10, PD_12, PD_8);
  lcd.fill_screen(BLACK);
  // lcd.draw_rect(0xFF00, 100,100, 200,200);
  // lcd.draw_char('A', 100,100, 6);
  // lcd.draw_char('B', 130,100, 6);
  // lcd.printf(0,0,4,WHITE, "%d Hz", SystemCoreClock);
  u8 adcidx = 0;
  u16 adcbuf[100];
  uint32_t adcval = 0;
  while (1) {
    char buf[100];
    adcbuf[(adcidx++) %100] = in.read_u16();
    adcval = 0;
    for(int idx=0; idx<100; idx++) {
      adcval += adcbuf[idx];
    }
    adcval /= sizeof(adcbuf);

    if(adcidx%30) {
      continue;
    }
    uint32_t micro = 29300 * adcval / 32740;
    snprintf(buf, sizeof(buf), "%d.%d", micro/1000, micro%1000);
    lcd.printf(0, 0, 8, 0xFF00, "%s", buf);
    wait_ms(200);
    lcd.printf(0, 0, 8, BLACK, "%s", buf);
  }
}
