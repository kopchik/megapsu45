#include "mbed.h"
// TODO: accept pin names
void delay(int ms) {
    wait_ms(ms);
}

class MYLCD {
    private:
        DigitalOut wrPin; // write strobe
        DigitalOut rsPin; // command/data
        DigitalOut resetPin;
        DigitalOut csPin;
        BusOut *bus;

    public:
        MYLCD(BusOut* _bus): wrPin(PB_0), rsPin(PA_0), resetPin(PA_1), csPin(PB_0) {
            bus = _bus;
            csPin = 1; // TODO: datasheet says it should be low in 4.1 DBI
            reset();
            init();
//            send_cmd(0x28); //display off
//            wait(2);
//            send_cmd(0x29); // display on
        }
    void reset() {
        resetPin = 0;
        wait_ms(20);
        resetPin = 1;
        wait_ms(200);
        wrPin = 0;
        rsPin = 1;
    }
    void send_cmd(uint8_t cmd) {
        rsPin = 0;
        bus->write(cmd);
        strobe();
        rsPin = 1;
    }
    void strobe() {
        wrPin = 1;
        delay(2);
        wrPin = 0;
        delay(2);
    }
    void LCD_Write_DATA(uint8_t data) {
        rsPin = 1; // TODO: unnecessary
        bus->write(data);
        strobe();
    }
    void setPixel(uint16_t color, int x, int y) {
//        setXY(x,y,x,y);
        LCD_Write_DATA(color);
    }
    void setXY(int x1, int y1, int x2, int y2) {
      send_cmd(0x2a); 
      LCD_Write_DATA(x1>>8);
      LCD_Write_DATA(x1);
      LCD_Write_DATA(x2>>8);
      LCD_Write_DATA(x2);
      send_cmd(0x2b); 
      LCD_Write_DATA(y1>>8);
      LCD_Write_DATA(y1);
      LCD_Write_DATA(y2>>8);
      LCD_Write_DATA(y2);
      send_cmd(0x2c); 
}
    void init(void) {
        send_cmd(0x11);//Sleep Out  
        delay(120);  
        
        //Set EQ
        send_cmd(0xEE);  
        LCD_Write_DATA(0x02);  
        LCD_Write_DATA(0x01);  
        LCD_Write_DATA(0x02);  
        LCD_Write_DATA(0x01);  
        
        // DIR TIM
        send_cmd(0xED);  
        LCD_Write_DATA(0x00);  
        LCD_Write_DATA(0x00);  
        LCD_Write_DATA(0x9A);  
        LCD_Write_DATA(0x9A);  
        LCD_Write_DATA(0x9B);  
        LCD_Write_DATA(0x9B);  
        LCD_Write_DATA(0x00);  
        LCD_Write_DATA(0x00);  
        LCD_Write_DATA(0x00);  
        LCD_Write_DATA(0x00);  
        LCD_Write_DATA(0xAE);  
        LCD_Write_DATA(0xAE);  
        LCD_Write_DATA(0x01);  
        LCD_Write_DATA(0xA2);  
        LCD_Write_DATA(0x00);  
        
        // set RM, DM
        send_cmd(0xB4);  
        LCD_Write_DATA(0x00); 
        
        // set Panel Driving
        send_cmd(0xC0);
        LCD_Write_DATA(0x00); //REV SM GS
        LCD_Write_DATA(0x3B); // NL[5:0]  
        LCD_Write_DATA(0x00); //SCN[6:0]  
        LCD_Write_DATA(0x02); //NDL 0 PTS[2:0]
        LCD_Write_DATA(0x11); //PTG ISC[3:0]  
        
        send_cmd(0xC1);//
        LCD_Write_DATA(0x10);//line inversion
        
        //Set Gamma
        send_cmd(0xC8);  
        LCD_Write_DATA(0x00); //KP1,KP0
        LCD_Write_DATA(0x46); //KP3,KP2
        LCD_Write_DATA(0x12); //KP5,KP4
        LCD_Write_DATA(0x20); //RP1,RP0
          LCD_Write_DATA(0x0c); //VRP0 01
          LCD_Write_DATA(0x00); //VRP1
          LCD_Write_DATA(0x56); //KN1,KN0
          LCD_Write_DATA(0x12); //KN3,KN2
          LCD_Write_DATA(0x67); //KN5,KN4
          LCD_Write_DATA(0x02); //RN1,RN0
          LCD_Write_DATA(0x00); //VRN0
          LCD_Write_DATA(0x0c); //VRN1 01
          
          //Set Power
          send_cmd(0xD0);  
          LCD_Write_DATA(0x44);//DDVDH :5.28
          LCD_Write_DATA(0x42); // BT VGH:15.84 VGL:-7.92
          LCD_Write_DATA(0x06);//VREG1 4.625V
          
          // set VCOM
          send_cmd(0xD1);
          LCD_Write_DATA(0x43); //VCOMH
          LCD_Write_DATA(0x16);
    
          // set power for normal mode      
          send_cmd(0xD2);  
          LCD_Write_DATA(0x04);  
          LCD_Write_DATA(0x22); //12
          
          // set power for partial mode
          send_cmd(0xD3);  
          LCD_Write_DATA(0x04);  
          LCD_Write_DATA(0x12);  
          
          // set power for idle mode
          send_cmd(0xD4);  
          LCD_Write_DATA(0x07);  
          LCD_Write_DATA(0x12);  
          
          //Set Panel
          send_cmd(0xE9); 
          LCD_Write_DATA(0x00);
          
          //Set Frame rate
          send_cmd(0xC5); 
          LCD_Write_DATA(0x08); //61.51Hz
          
          // set address mode
          send_cmd(0X0036);
          LCD_Write_DATA(0X000a); //ÊúÆÁ²ÎÊý
          LCD_Write_DATA(0x003B); //ºáÆÁ²ÎÊý
          
          // pixel format
          send_cmd(0X003A);
          LCD_Write_DATA(0X0055); // 16 bit
          
          // set column address
          send_cmd(0X002A);
          LCD_Write_DATA(0X0000);
          LCD_Write_DATA(0X0000);
          LCD_Write_DATA(0X0001);
          LCD_Write_DATA(0X003F);
          
          // set page address
          send_cmd(0X002B);
          LCD_Write_DATA(0X0000);
          LCD_Write_DATA(0X0000);
          LCD_Write_DATA(0X0001);
          LCD_Write_DATA(0X00E0);
          delay(120);
          
          // set tear on
          send_cmd(0x35);
          LCD_Write_DATA(0x00);//TE ON
          
          // display On
          send_cmd(0x29); 
          delay(5);    
    };
};

int main() {
//    DigitalOut led(PA_5);
//    DigitalOut out1(PC_0);
//    DigitalOut out2(PC_2);
//    DigitalOut out3(PC_4);
//    DigitalOut out4(PC_6);
//    DigitalOut out5(PC_8);
//    DigitalOut out6(PC_10);
//    DigitalOut out7(PC_12);
//    DigitalOut out8(PA_13);
//    DigitalOut pins[] = {out1, out2, out3, out4, out5, out6, out7, out8, led};
//    led = 1;    wait(0.1);

//while(1) {
//    out8 = !out8;
//    wait_ms(10);
//    }
//    while(1) {
//        wait_ms(10);
//        for (int i=0; i<9; i++) {
//            DigitalOut pin = pins[i];
//            pin = !pin;
//        }
//    }
    // PC_14 => PA_13
    BusOut bus(PC_0, PC_1, PC_2, PC_3, PC_4, PC_5, PC_6, PC_7, PC_8, PC_9, PC_10, PC_11, PC_12, PC_13, PA_13, PC_15);
    while(1) {
        bus.write(0xFFFF);
        wait_ms(5);
        bus.write(0);
        wait_ms(5);
    }
//    MYLCD lcd(&bus);
//    for (int x=0; x<300; x++) {
//        lcd.setPixel(0xaaaa, x,x);
//    }
}

