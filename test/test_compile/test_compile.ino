#include <Arduino.h>
#include <module_driver.hpp>
#include <module_ir.hpp>
#include <module_lcd.hpp>
#include <module_timer.hpp>
#include <module_encoder.hpp>
#include <config.hpp>

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

Driver *driver;
IrArray *irArray;
Lcd *lcd;
Timer *timer;
Encoder *encoderL;
Encoder *encoderR;

void setup() {
    Serial.begin(115200);
    driver = new Driver();
    irArray = new IrArray();
    lcd = new Lcd(Config::Ui::Lcd::Address, "Trace Invader", "Get ready!");
    encoderL = new Encoder(mux, Config::Encoder::Pins::LeftA);
    encoderR = new Encoder(mux, Config::Encoder::Pins::RightA);
}

void loop() {
    driver->drive(255,255);
    for(int i = 0; i < 10; i++) {
        lcd->display(String(encoderL->getPulseCount()),
                    String(encoderR->getPulseCount()));
        delay(100);
    }

    driver->drive(255,128);
    for(int i = 0; i < 10; i++) {
        lcd->display(String(encoderL->getPulseCount()),
                    String(encoderR->getPulseCount()));
        delay(100);
    }

    driver->drive(128,255);
    for(int i = 0; i < 10; i++) {
        lcd->display(String(encoderL->getPulseCount()),
                    String(encoderR->getPulseCount()));
        delay(100);
    }
    
    driver->drive(255,0);
    for(int i = 0; i < 10; i++) {
        lcd->display(String(encoderL->getPulseCount()),
                    String(encoderR->getPulseCount()));
        delay(100);
    }

    driver->drive(0,255);
    for(int i = 0; i < 10; i++) {
        lcd->display(String(encoderL->getPulseCount()),
                    String(encoderR->getPulseCount()));
        delay(100);
    }
}