// IR Sensor Range Test
// Test file for determining the "cone of vision" of the IR sensors.

#include <Arduino.h>

#define IR_PIN 15

void setup() {
    Serial.begin(115200);
    Serial.println("Setting pin mode");
    pinMode(IR_PIN, INPUT);
    Serial.println("Pin mode set. Entering loop...");
}

void loop() {
    if (digitalRead(IR_PIN)) {
        Serial.println("NO");
    }
    else {
        Serial.println("YES");
    }
    delay(250);
}