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

hw_timer_t* timer = NULL;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
SemaphoreHandle_t timerSemaphore;
TaskHandle_t controlTaskHandle = NULL;

Driver *driver;
IrArray *irArray;
Lcd *lcd;
Encoder *encoderL;
Encoder *encoderR;

//QuickPID parameters
float targetPulseCountLeft;
float encoderPulseCountLeft;
float pidOutputLeft;

float targetPulseCountRight;
float encoderPulseCountRight;
float pidOutputRight;

//we need to tune these
float Kp = 6;
float Ki = 10;
float Kd = 2;

//initialize using "new" in setup()
QuickPID *pidLeft;
QuickPID *pidRight;

volatile int i = 0;
volatile bool phase = 0;

//sensor data
volatile uint8_t irState;

void ARDUINO_ISR_ATTR navigate() {
    switch(irState) {
        case 0b01111:
            targetPulseCountLeft = 0;
            targetPulseCountRight = 5;
            break;
        case 0b00111:
            targetPulseCountLeft = 2;
            targetPulseCountRight = 5;
            break;
        case 0b10111:
            targetPulseCountLeft = 8;
            targetPulseCountRight = 14;
            break;
        case 0b10011:
            targetPulseCountLeft = 12;
            targetPulseCountRight = 16;
            break;
        case 0b11011:
            targetPulseCountLeft = 20;
            targetPulseCountRight = 20;
            break;
        case 0b11001:
            targetPulseCountLeft = 16;
            targetPulseCountRight = 12;
            break;
        case 0b11101:
            targetPulseCountLeft = 7;
            targetPulseCountRight = 4;
            break;
        case 0b11100:
            targetPulseCountLeft = 5;
            targetPulseCountRight = 2;
            break;
        case 0b11110:
            targetPulseCountLeft = 5;
            targetPulseCountRight = 0;
            break;
        default:
            targetPulseCountLeft = 10;
            targetPulseCountRight = 10;
    }
}

void ARDUINO_ISR_ATTR run() {
    uint32_t time = micros();
    //collect sensor data
    irState = irArray->getLineState();
    encoderPulseCountLeft = encoderL->getPulseCount();
    encoderPulseCountRight = encoderR->getPulseCount();

    //act on sensor data
    if (phase) {
        navigate();
    }

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
    if (now - lastButtonTime > 200) {  // basic debounce
        lastButtonTime = now;
        portENTER_CRITICAL_ISR(&mux);
        phase = !phase;
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

    Serial.print("   LCD................");
    lcd = new Lcd(__I2C_ADDR_LCD, "Trace Invader", "Get ready!");
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
    pidRight->SetSampleTimeUs(__TIMER_PERIOD);
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
    targetPulseCountLeft = 5;
    targetPulseCountRight = 5;
}


//loop has been replaced by controlTask, it never gets called 
void loop() {}
