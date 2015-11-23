#include "mbed.h"

class MYLCD {
 private:
  PortOut port;
  DigitalOut wrPin;  // write strobe
  DigitalOut rsPin;  // command/data
  DigitalOut resetPin;
  DigitalOut csPin;

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
  void setPixel(uint16_t color, int x, int y) {
    setXY(x,y,x,y);
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
  void reset_init(void) {
    resetPin = 0;
    wait_ms(120); // just in case
    resetPin = 1;
    wait_ms(100); // >100ms 6.2.13 Exit_sleep_omde (11h) and 5.14.3 Programming sequence

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

#define COLOR 0x0F0F
  MYLCD lcd(PortG, PD_14, PD_10, PD_12, PD_8);
  for (int x=0; x<300; x++) {
    lcd.setPixel(COLOR, x,x);
    lcd.setPixel(COLOR, x+1,x);
    lcd.setPixel(COLOR, x+2,x);
    lcd.setPixel(COLOR, x+3,x);
  }
}
