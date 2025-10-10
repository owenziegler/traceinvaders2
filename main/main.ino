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
float targetRpmLeft;
float encoderRpmLeft;
float pidOutputLeft;

float targetRpmRight;
float encoderRpmRight;
float pidOutputRight;

//we need to tune these
float Kp = 0;
float Ki = 0;
float Kd = 0;

//initialize using "new" in setup()
QuickPID *pidLeft;
QuickPID *pidRight;

volatile float rpmLeft;
volatile float rpmRight;

volatile int i = 0;
volatile bool phase = 0;

void ARDUINO_ISR_ATTR run() {
    //collect data
    portENTER_CRITICAL(&mux);
    //right now rpm only shows as 214 or 0, no in between. don't know if it's a math thing or a sensor thing.
    //maybe check vs pulse counts?
    rpmLeft = (encoderL->getPulseCount() / 50) * (__ENCODER_RPM_NUMERATOR / __ENCODER_RPM_DENOMINATOR);
    rpmRight = (encoderR->getPulseCount() / 50) * (__ENCODER_RPM_NUMERATOR / __ENCODER_RPM_DENOMINATOR);
    portEXIT_CRITICAL(&mux);

    //displays weird on lcd, in the thousands. On serial it has the behavior described above
    //lcd->display(String(rpmLeft),String(rpmRight));
    Serial.print(String(rpmLeft));
    Serial.print(" ");
    Serial.print(String(rpmRight));
    Serial.println("");

    //ramp up and down each motor for testing
    if(phase == 0) {
        driver->drive(i, 0);
        i++;
        if(i == 255) {
            phase = 1;
        }
    }
    else if (phase == 1) {
        driver->drive(i, 0);
        i--;
        if(i == 0) {
            phase = 2;
        }
    }
    else if (phase == 2) {
        driver->drive(0, i);
        i++;
        if(i == 255) {
            phase = 3;
        }
    }
    else if (phase == 3) {
        driver->drive(0, i);
        i--;
        if(i == 0) {
            phase = 0;
        }
    }
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
    driver->drive(255,255);
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

    Serial.println("Initializing timer...");
    //most of these params should be moved to config.hpp
    //start timer at 1 MHz sample rate
    timer = timerBegin(1000000);
    //attach onTimer to timer when the alarm triggers
    timerAttachInterrupt(timer, &onTimer);
    //set the timer to go every 50 ms (50000 us). auto-repeat=true, auto-repeat number=0 (infinite)
    timerAlarm(timer, 50000, true, 0);
}

//loop has been replaced by controlTask, it never gets called 
void loop() {}
