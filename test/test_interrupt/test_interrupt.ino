#include <Arduino.h>
#include <Wire.h>

//#include "../../lib/config/config.hpp"
//#include "../../lib/modules/module_driver.hpp"
#include <config.hpp>
#include <module_driver.hpp>
#include <driver/timer.h>

hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore; //semaphore to signal loop
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; //mux for blocking

Driver *driver;

volatile int isrCounter = 0;
volatile int lastIsrAt = 0;
volatile int pulseCountL = 0;
volatile float lastPulseCountL = 0;
volatile int pulseCountR = 0;
volatile float lastPulseCountR = 0;

#define TIMER_FREQ 1000000 // 1MHz frequency
#define TIMER_AUTORELOAD true //autoreload yes
#define TIMER_RELOAD_COUNT 0 //never stop reloading
#define TIMER_ALARM 50000 //100ms in us

//interrupt service routine
void ARDUINO_ISR_ATTR onTimer() {
// Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter = isrCounter + 1;
  lastPulseCountL = (float)pulseCountL;
  pulseCountL = 0;
  lastPulseCountR = (float)pulseCountR;
  pulseCountR = 0;
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}

void ARDUINO_ISR_ATTR onPulseL() {
  portENTER_CRITICAL_ISR(&timerMux);
  ++pulseCountL;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void ARDUINO_ISR_ATTR onPulseR() {
  portENTER_CRITICAL_ISR(&timerMux);
  ++pulseCountR;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Serial connection established");
  Serial.println("Attempting to set up motor driver...");
  driver = new Driver();
  Serial.println("Motor driver setup complete.");
  //pinMode(_UI_PIN_BUTTON, INPUT_PULLUP);
  Serial.println("Attempting to set encoder pin mode...");
  pinMode(Config::Encoder::Pins::LeftA, INPUT);
  pinMode(Config::Encoder::Pins::RightA, INPUT);
  Serial.println("Encoder pin mode set");
  Serial.println("Attaching interrupt to encoder pin...");
  attachInterrupt(Config::Encoder::Pins::LeftA,  &onPulseL, RISING);
  attachInterrupt(Config::Encoder::Pins::RightA, &onPulseR, RISING);
  Serial.println("Interrupt attached to encoder pin");
  Serial.println("Initializing semaphore");
  timerSemaphore = xSemaphoreCreateBinary();
  Serial.println("Semaphore initialized");
  Serial.println("Initializing timer...");
  timer = timerBegin(TIMER_FREQ);
  Serial.println("Timer initialized.");
  Serial.println("Attaching timer interrupt...");
  timerAttachInterrupt(timer, &onTimer);
  Serial.println("Timer interrupt attached.");
  Serial.println("Setting up timer alarm");
  timerAlarm(timer,TIMER_ALARM, TIMER_AUTORELOAD, TIMER_RELOAD_COUNT);
  Serial.println("Timer alarm set up.");
  Serial.println("Attempting to set both motors at PWM width 255...");
  //setMotorL(255);
  Serial.println("Motor L set to 255");
  //setMotorR(255);
  driver->drive(255,255);
  Serial.println("Motor R set to 255");
  Serial.println("Entering loop...");
}

void loop() {
    // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
    uint32_t isrCount = 0, isrTime = 0, isrLastPulseCountL = 0, isrLastPulseCountR = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    isrLastPulseCountL = lastPulseCountL * Config::Encoder::gearRatio;
    isrLastPulseCountR = lastPulseCountR * Config::Encoder::gearRatio;

    portEXIT_CRITICAL(&timerMux);
    // Print it
    Serial.print(isrLastPulseCountL);
    Serial.print(" left");
    Serial.print(isrLastPulseCountR);
    Serial.print(" right\n");
  }
}