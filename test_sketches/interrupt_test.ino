#include <Arduino.h>
#include "module_driver.hpp"
#include "config.hpp"
#include <Wire.h>
#include <driver/timer.h>
//#include <Adafruit_MotorShield.h>
//#include <Adafruit_SSD1306.h>

// Connect to the two encoder outputs!


// These let us convert ticks-to-RPM
#define GEARING     20
#define ENCODERMULT 10
#define FORWARD 1
#define BACKWARD 2

// Create the motor shield object with the default I2C address
//Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// And connect a DC motor to port M1
//Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
// We'll display the speed/direction on the OLED
//Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

volatile float RPM = 0;
volatile uint32_t lastA = 0;
volatile int pulseCount = 0;
volatile bool interruptTriggered = false;
hw_timer_t *timer = NULL;

// FreeRTOS critical section lock
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR timerInterrupt() {
  //float pulsesInInterval = pulseCount;
  //RPM = (pulsesInInterval * 6000000) / 100000;
  //pulseCount = 0;
  interruptTriggered = true;

}

void IRAM_ATTR pulseInterrupt() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);           // set up Serial library at 9600 bps
  delay(200);
  Serial.println("Serial connected. Starting setup...");
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);
  //driverSetup();
  //Serial.println("Driver setup complete");
  //pinMode(_DRIVER_PIN_ENCODER_B, INPUT_PULLUP);
  //pinMode(_DRIVER_PIN_ENCODER_A, INPUT_PULLUP);
  //Serial.println("Encoder pins configured");
  //attachInterrupt(digitalPinToInterrupt(_DRIVER_PIN_ENCODER_A), pulseInterrupt, RISING);
  //Serial.println("Interrupt attached to encoder pin A");
  /*timer_config_t config;
  config.alarm_en = TIMER_ALARM_EN;
  config.auto_reload = TIMER_AUTORELOAD_EN;
  config.counter_dir = TIMER_COUNT_UP;
  config.divider = 80;
  config.intr_type = TIMER_INTR_LEVEL;
  config.counter_en = TIMER_START;
  Serial.println("Timer configuration created");

  esp_err_t err = timer_init(TIMER_GROUP_0, TIMER_0, &config);
  if(err != ESP_OK) {
    Serial.println("Timer init failed");
    Serial.println(err);
  } else {
    Serial.println("Timer initialized");
  }
  err = timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 100000);
  if(err != ESP_OK) {
    Serial.println("Timer alarm value setting failed");
    Serial.println(err);
  } else {
    Serial.println("Timer alarm value set");
  }
  err = timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  if(err != ESP_OK) {
    Serial.println("Enabling timer interrupt failed");
    Serial.println(err);
  } else {
    Serial.println("Timer interrupt enabled");
  }
  err = timer_isr_register(TIMER_GROUP_0, TIMER_0, timerInterrupt, NULL, ESP_INTR_FLAG_IRAM, NULL);
  if(err != ESP_OK) {
    Serial.println("Invalid arguments to timer_isr_callback_add");
  } else {
    Serial.println("Timer interrupt function registered");
  }
  timer_start(TIMER_GROUP_0, TIMER_0);
  Serial.println("Timer started");
  delay(100);
  */
  timer = timerBegin(80);
  timerAttachInterrupt(timer, timerInterrupt);
  timerAlarm(timer, 1000000, true, 0);
  timerStart(timer);

  //while (!Serial) delay(1);

  Serial.println("MMMMotor party!");
}

void printRPM() {
  float localRPM;
  portENTER_CRITICAL(&mux);  // atomic access in ISR
  localRPM = RPM;
  portEXIT_CRITICAL(&mux);
  Serial.print((int)localRPM); Serial.println(" RPM");
}

int i;
void loop() {
  /*delay(50);
  //myMotor->run(FORWARD);

  for (i=0; i<255; i++) {
    //myMotor->setSpeed(i);
    setMotorA(i);
    setMotorB(i);
    delay(20);
    printRPM();
  }

  for (i=255; i!=0; i--) {
    //myMotor->setSpeed(i);
    setMotorA(i);
    setMotorB(i);
    delay(20);
    printRPM();
  }*/
  if(interruptTriggered) {
    Serial.print("Interrupt triggered!");
    interruptTriggered = false;
  }
  delay(20);
}
