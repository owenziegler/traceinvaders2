#include <Arduino.h>
#include <module_lcd.hpp>
#include <config.hpp>

Lcd *lcd;
unsigned long sum = 0;
unsigned long count = 0;

void setup() {
    Serial.begin(115200);
    lcd = new Lcd(Config::Lcd::Address, "Trace Invader", "Get ready!");
}

void loop() {
    unsigned long time = micros();
    lcd->display("TEST STRING", "TEST STRING");
    sum += micros() - time;
    count++;
    if(count == 100) {
        float avgTime = (float)sum / (float)count;
        lcd->display("", String(avgTime));
        for(;;) {}
    }
}