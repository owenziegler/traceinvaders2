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
#include <module_navigation.hpp>

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
float Kp = __PID_KP;
float Ki = __PID_KI;
float Kd = __PID_KD;

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
    //collect sensor data
    irState = irArray->getLineState();
    encoderPulseCountLeft = encoderL->getPulseCount();
    encoderPulseCountRight = encoderR->getPulseCount();
    //act on sensor data
    navigation->navigate();


    pidLeft->Compute();
    pidRight->Compute();

    encoderL->resetPulseCount();
    encoderR->resetPulseCount();

    driver->drive((int)pidOutputLeft, (int)pidOutputRight);
    //lcd->display(String((int)pidOutputLeft), String((int)pidOutputRight));
}


//this is from chatgpt, it's been tested to work.
//every 50ms timer calls onTimer
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
        navigation->nextState = __NAV_STATE::RUNUP;
        portEXIT_CRITICAL_ISR(&mux);
        Serial.println("BUTTON PRESSED");
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
    encoderL = new Encoder(mux, __ENCODER_PIN_L_A);
    Serial.println("Initialized successfully.");

    Serial.print("   Right Encoder......");
    encoderR = new Encoder(mux, __ENCODER_PIN_R_A);
    Serial.println("Initialized successfully.");
    Serial.println("Subsystem modules initialized successfully.");

    Serial.print("Button... ");
    pinMode(__UI_PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(__UI_PIN_BUTTON, onButton, FALLING);
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
    pidLeft->SetSampleTimeUs(__TIMER_PERIOD);
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
    pidRight->SetSampleTimeUs(__TIMER_PERIOD);
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
    timer = timerBegin(__TIMER_HZ);
    //attach onTimer to timer when the alarm triggers
    timerAttachInterrupt(timer, &onTimer);
    //set the timer to go every 50 ms (50000 us). auto-repeat=true, auto-repeat number=0 (infinite)
    timerAlarm(timer, __TIMER_PERIOD, __TIMER_AUTORELOAD, __TIMER_RELOAD_COUNT);
    //set target pulse count
    targetPulseCountLeft = 0;
    targetPulseCountRight = 0;
    baseSpeed = __NAV_BASE_PULSECOUNT_HIGH;
}


//loop has been replaced by controlTask, it never gets called 
void loop() {}
