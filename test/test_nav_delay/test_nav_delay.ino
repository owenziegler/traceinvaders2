#include <Arduino.h>
#include <cstdint>

#include <QuickPID.h>

#include <utils.hpp>
#include <config.hpp>
#include <module_ir.hpp>
#include <module_lcd.hpp>
#include <module_driver.hpp>
#include <module_timer.hpp>
#include <module_encoder.hpp>
//#include <module_navigation.hpp>
#include "module_navigation_test.hpp"

hw_timer_t* timer = NULL;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
SemaphoreHandle_t timerSemaphore;
TaskHandle_t controlTaskHandle = NULL;

Driver *driver;
IrArray *irArray;
Encoder *encoderL;
Encoder *encoderR;
Navigation *navigation;

//QuickPID parameters
float targetPulseCountLeft;
float encoderPulseCountLeft;
float pidOutputLeft;

float targetPulseCountRight;
float encoderPulseCountRight;
float pidOutputRight;

//we need to tune these
float Kp = Config::Pid::Kp;
float Ki = Config::Pid::Ki;
float Kd = Config::Pid::Kd;

//initialize using "new" in setup()
QuickPID *pidLeft;
QuickPID *pidRight;

volatile int i = 0;
volatile int phase = 0;
volatile float baseSpeed = 0;
volatile bool phaseChanged = false;

//sensor data
volatile uint8_t irState;

void ARDUINO_ISR_ATTR run() {
    unsigned long time = micros();
    navigation->count++;
    //collect sensor data
    irState = irArray->getLineState();
    encoderPulseCountLeft = encoderL->getPulseCount();
    encoderPulseCountRight = encoderR->getPulseCount();
    //act on sensor data
    navigation->navigate();

    if(targetPulseCountLeft >= 0 || targetPulseCountRight >= 0) {
        driver->setForward();
    } else {
        driver->setBackward();
        targetPulseCountLeft = -targetPulseCountLeft;
        targetPulseCountRight = -targetPulseCountRight;
    }

    pidLeft->Compute();
    pidRight->Compute();

    encoderL->resetPulseCount();
    encoderR->resetPulseCount();
    if(navigation->stop) {
        driver->drive(0,0);
    } else {
        driver->drive((uint8_t)pidOutputLeft, (uint8_t)pidOutputRight);
    }
    navigation->timesum += (micros() - time);  
}


//this is from chatgpt, it's been tested to work.
//every 5ms timer calls onTimer
void ARDUINO_ISR_ATTR onTimer() {
  // Signal the task to run
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(timerSemaphore, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();  // Request context switch if needed
  }
}

volatile uint32_t lastButtonTime = 0;

void IRAM_ATTR onButton() {
    uint32_t now = millis();
    if (now - lastButtonTime > 500) {  // basic debounce
        lastButtonTime = now;
        portENTER_CRITICAL_ISR(&mux);
        navigation->nextState = Config::Navigation::States::Countdown; 
        portEXIT_CRITICAL_ISR(&mux);
    }
}

void controlTask(void * parameter) {
    for (;;) {
        if(xSemaphoreTake(timerSemaphore, portMAX_DELAY)) {
            run();
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("Serial connection established.");
    
    Serial.print("Initializing semaphore... ");
    timerSemaphore = xSemaphoreCreateBinary();
    Serial.println("Initialized successfully.");

    Serial.print("Creating RTOS task... ");
    xTaskCreatePinnedToCore(
        controlTask,
        "controlTask",
        4096,
        NULL,
        1,
        &controlTaskHandle,
        1
    );
    Serial.println("Created successfully.");
    
    Serial.println("Initializing subsystem modules...");
    Serial.print("   Driver.............");
    driver = new Driver();
    //driver->drive(255,255);
    Serial.println("Initialized successfully.");

    Serial.print("   IR Array...........");
    irArray = new IrArray();
    Serial.println("Initialized successfully.");

    
    Serial.print("   Left Encoder.......");
    encoderL = new Encoder(mux, Config::Encoder::Pins::LeftA);
    Serial.println("Initialized successfully.");

    Serial.print("   Right Encoder......");
    encoderR = new Encoder(mux, Config::Encoder::Pins::RightA);
    Serial.println("Initialized successfully.");
    Serial.println("Subsystem modules initialized successfully.");

    Serial.print("Button... ");
    pinMode(Config::Ui::Pins::PinButton, INPUT_PULLUP);
    attachInterrupt(Config::Ui::Pins::PinButton, onButton, FALLING);
    Serial.println("Initialized successfully.");

    Serial.println("Setting up PID...");
    Serial.print("left pid... ");
    pidLeft = new QuickPID(
        &encoderPulseCountLeft,
        &pidOutputLeft,
        &targetPulseCountLeft,
        Kp, Ki, Kd,
        QuickPID::Action::direct
    );
    pidLeft->SetMode(QuickPID::Control::automatic);
    pidLeft->SetOutputLimits(0,255);
    pidLeft->SetAntiWindupMode(QuickPID::iAwMode::iAwCondition);
    pidLeft->SetSampleTimeUs(Config::Timer::Period);
    Serial.println("done");

    Serial.print("right pid... ");
    pidRight = new QuickPID(
        &encoderPulseCountRight,
        &pidOutputRight,
        &targetPulseCountRight,
        Kp, Ki, Kd,
        QuickPID::Action::direct
    );
    pidRight->SetMode(QuickPID::Control::automatic);
    pidRight->SetOutputLimits(0,255);
    pidRight->SetAntiWindupMode(QuickPID::iAwMode::iAwCondition);
    pidRight->SetSampleTimeUs(Config::Timer::Period);
    Serial.println("done");

    Serial.print("Inititalizing navigation...");
    navigation = new Navigation(
        &irState,
        &targetPulseCountLeft,
        &targetPulseCountRight,
        &baseSpeed
    );
    Serial.println("done");

    Serial.println("Initializing timer...");
    //most of these params should be moved to config.hpp
    //start timer at 1 MHz sample rate
    timer = timerBegin(Config::Timer::Frequency);
    //attach onTimer to timer when the alarm triggers
    timerAttachInterrupt(timer, &onTimer);
    //set the timer to go every 50 ms (50000 us). auto-repeat=true, auto-repeat number=0 (infinite)
    timerAlarm(timer, Config::Timer::Period, Config::Timer::AutoReload, Config::Timer::ReloadCount);
    //set target pulse count
    targetPulseCountLeft = 0;
    targetPulseCountRight = 0;
    baseSpeed = Config::Navigation::BasePulseCount;

}


//loop has been replaced by controlTask, it never gets called 
void loop() {}
