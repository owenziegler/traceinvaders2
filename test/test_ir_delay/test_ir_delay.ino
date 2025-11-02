#include <module_ir.hpp>
#include <config.hpp>

IrArray *irArray;
volatile uint8_t irState;

void setup() {
  Serial.begin(115200);
  irArray = new IrArray(); 
  int timeTotal = 0;
  for (int i = 0; i < 100; i++) {
    int time = micros();
    irState = irArray->getLineState();
    time = micros() - time;
    timeTotal += time;
    Serial.println(time);
  }
  float timeAvg = (float)timeTotal / 100;
  Serial.print("Average time: ");
  Serial.println(timeAvg);
}

void loop() {
}