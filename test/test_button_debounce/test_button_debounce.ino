#include <Arduino.h>

volatile uint32_t lastButtonTime = 0;

void IRAM_ATTR onButton() {
    uint32_t now = millis();
    if (now - lastButtonTime > 500) {  // basic debounce
        lastButtonTime = now;
        Serial.println("BUTTONPRESSED");
    }
}

void setup() {
  Serial.begin(115200);

}

void loop() {
  Serial.println("TESTING BUTTON:");
  onButton();
  delay(100);
  onButton();
  delay(500);
}