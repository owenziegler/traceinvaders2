/*
traceinvaders.ino

Owen Ziegler
2025 09 22

Description: Main module for Trace Invader navigation program.
Contains main state machine logic.
*/

#include <Arduino.h>
#include <cstdint>

#include "utils.hpp"
#include "config.hpp"
#include "module_ir.hpp"
#include "module_lcd.hpp"
#include "module_driver.hpp"
#include "module_timer.hpp"

//states
#define RESET 0
#define COUNTDOWN 1
#define RUNUP 2
#define LAP 3
#define LOST 4
#define FINISH 5

uint8_t lineState;            //
uint8_t currentState = 0;        //
uint8_t nextState = 0;
uint8_t countdownTimer = 5;
bool running = 0;             //
uint32_t stateCounter = 0;         //generic counter var used by states. reset to 0 on state change
uint8_t tickCounter = 0;          //
uint8_t lostCounter;
float lapTime;
Timer timer;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  irSetup();
  lcdSetup();
  driverSetup();
  pinMode(_UI_PIN_BUTTON, INPUT_PULLUP);
  timer = Timer();
}

//FSM state handler functions

void handleReset() {
  displayStrings("Trace Invader", "");
  //if the button is pressed, wait for it to become unpressed
  //when the button is unpressed, set nextState to COUNTDOWN
  if(!digitalRead(_UI_PIN_BUTTON)) { //if the button is pressed
    while (digitalRead(_UI_PIN_BUTTON)) { //wait for the button to become unpressed
      delay(10); //wait 10 ms between checks to reduce CPU usage
    }
    nextState = COUNTDOWN;
  }
}

void handleCountdown() {
  //while countdownTimer > 0:
  //display countdownTimer to screen
  //wait 1000 ms
  //clear screen
  //decrement countdownTimer

  //reset countdownTimer to 5
  //transition to handleRunup
  if(countdownTimer > 0) {
    displayStrings("Get Ready!", String(countdownTimer));
    delay(1000);
    countdownTimer--;
    clearDisplay();
  }
  else {
    countdownTimer = 5;
    nextState = RUNUP;
  }
}

void handleRunup() {
  //same control logic as handleLap, except when a tick is crossed,
  //nextState is set to LAP and the stopwatch is started
  clearDisplay();
  lineState = getLineState();
  displayStrings("Runup",uint8ToBinary(lineState));
  switch(lineState) {
    //center
    case 0b11011:
      setSpeed(1,1);
      break;
    //left 1
    case 0b10011:
      //drive(196,256);
      setSpeed(0.5,1);
      break;
    //left 2
    case 0b10111:
      //drive(64,256);
      setSpeed(0,1);
      break;
    //left 3 & 4
    case 0b00111:
    case 0b01111:
      //drive(32,196);
      setSpeed(0,1);
      break;  
    //right 1
    case 0b11001:
      //drive(256,196);
      setSpeed(1,0.5);
      break;
    //right 2
    case 0b11101:
      //drive(256,64);
      setSpeed(1,0);
      break;
    //right 3 & 4
    case 0b11100:
    case 0b11110:
      //drive(196,32);
      setSpeed(1,0);
      break;
    //any other state
    case 0b10000:
    case 0b00001:
    case 0b00000:
      tickCounter = 0;
      setSpeed(0.5,0.5);
      break;
    case 0b01011:
    case 0b01001:
    case 0b00011:
      setSpeed(0,0);
      delay(100);
      tickCounter++;
      if(tickCounter > 5) {
        timer.timerStart();
        nextState = LAP;
      }
      break;
    default:
      //drive(192,192);
      tickCounter = 0;
      setSpeed(0.5,0.5);
      break;
  }
}

void handleLap() {
  //normal control logic
  //when tick is crossed, next State is set to FINISH and stopwatch is ended
  //when lineState == 0b11111 for more than a certain number of loops, transition to LOST state
  lineState = getLineState();
  switch(lineState) {
    //center
    case 0b11011:
      setMotorA(128);
      setMotorB(128);
      break;
    //left 1
    case 0b10011:
      //drive(196,256);
      setMotorA(100);
      setMotorB(128);
      break;
    //left 2
    case 0b10111:
      //drive(64,256);
      setMotorA(64);
      setMotorB(128);
      break;
    //left 3 & 4
    case 0b00111:
    case 0b01111:
      //drive(32,196);
      setMotorA(32);
      setMotorB(128);
      break;  
    //right 1
    case 0b11001:
      //drive(256,196);
      setMotorA(128);
      setMotorB(100);
      break;
    //right 2
    case 0b11101:
      //drive(256,64);
      setMotorA(128);
      setMotorB(64);
      break;
    //right 3 & 4
    case 0b11100:
    case 0b11110:
      //drive(196,32);
      setMotorA(128);
      setMotorB(32);
      break;
    //any other state
    case 0b10000:
    case 0b00001:
    case 0b00000:
      tickCounter = 0;
      setMotorA(128);
      setMotorB(128);
      break;
    case 0b01011:
    case 0b01001:
    case 0b00011:
      setMotorA(0);
      setMotorB(0);
      delay(100);
      tickCounter++;
      if(tickCounter > 5) {
        lapTime = timer.timerLap();
        nextState = FINISH;
      }
      break;
    case 0b11111:
      lostCounter++;
      if(lostCounter > 5) {
        lostCounter = 0;
        nextState = LOST;
      }
    default:
      //drive(192,192);
      tickCounter = 0;
      setMotorA(128);
      setMotorB(128);
      break;
  }
}

void handleLost() {
  lineState = getLineState();
  if (lineState == 0b11111) {
    setMotorA(-128);
    setMotorB(-128);
  }
  else {
    nextState = LAP;
  }
}

void handleFinish() {
  setMotorA(0);
  setMotorB(0);
  displayStrings("Lap Time (s):", String(lapTime));
  if(!digitalRead(_UI_PIN_BUTTON)) { //if the button is pressed
    while (digitalRead(_UI_PIN_BUTTON)) { //wait for the button to become unpressed
      delay(10); //wait 10 ms between checks to reduce CPU usage
    }
    nextState = RESET;
  }
}

//main loop function
void loop() {
  //state cases
  switch (currentState) {
    case RESET:
      handleReset();
      break;
    case COUNTDOWN:
      handleCountdown();
      break;
    case RUNUP:
      handleRunup();
      break;
    case LAP:
      handleLost();
      break;
    case LOST:
      handleLost();
      break;
    case FINISH:
      handleFinish();
      break;
  }
    //check for reset
  if(!digitalRead(_UI_PIN_BUTTON) && currentState != RESET)
  {
    while(digitalRead(_UI_PIN_BUTTON))
    {
      delay(10);
    }
    nextState = RESET;
  }
  //state switcher
  if(nextState != currentState) {
    stateCounter = 0;
    currentState = nextState;
  }
  //short delay
  delay(5);
}