/*
  ESP32 (S3) + TB6612FNG dual-motor driver
  Uses pins from pins.h (provided by you).
  LEDC PWM @ 20 kHz, 12-bit. Simple serial command interface.

  Commands (serial, 115200):
    set <pct>          e.g. "set 75"
    dir L FWD|REV      set left direction
    dir R FWD|REV      set right direction
    go                 run both motors
    brake              hard brake (IN1=IN2=HIGH)
    coast              freewheel (IN1=IN2=LOW)
    stby 0|1           standby low/high
*/

#if !defined(ARDUINO_ARCH_ESP32)
#error "This sketch uses ESP32 LEDC (ledcSetup/ledcAttachPin). Select an ESP32/ESP32-S3 board."
#endif

#include <Arduino.h>
#include <Wire.h>
#include "pins.h"

// ---------------- PWM CONFIG ----------------
static const int PWM_FREQ_HZ  = 20000;   // 20 kHz (inaudible)
static const int PWM_RES_BITS = 12;      // 0..4095
static const int PWM_MAX      = (1 << PWM_RES_BITS) - 1;

// LEDC channels (0..15). One per motor PWM pin.
static const int CH_PWM_LEFT  = 0;
static const int CH_PWM_RIGHT = 1;

// ---------------- RUNTIME STATE ----------------
enum Dir : int8_t { FWD = +1, REV = -1 };

static Dir leftDir  = FWD;
static Dir rightDir = FWD;
static int dutyPercent = 50;                   // start at 50%

static inline int dutyRaw() {
  return (PWM_MAX * dutyPercent) / 100;
}

// Invert defines if your physical wiring is reversed
//#define INVERT_LEFT
//#define INVERT_RIGHT

// ---------------- MOTOR HELPERS ----------------
static void motorRunOne(uint8_t in1, uint8_t in2, int pwmCh, Dir dir, int duty_raw) {
  if (dir == FWD) {                 // IN1=HIGH, IN2=LOW
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {                          // IN1=LOW,  IN2=HIGH
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  ledcWrite(pwmCh, constrain(duty_raw, 0, PWM_MAX));
}

static void motorBrakeOne(uint8_t in1, uint8_t in2, int pwmCh) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, HIGH);
  ledcWrite(pwmCh, 0);
}

static void motorCoastOne(uint8_t in1, uint8_t in2, int pwmCh) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  ledcWrite(pwmCh, 0);
}

static void motorRunLeft() {
#ifdef INVERT_LEFT
  motorRunOne(_DRIVER_PIN_AIN1, _DRIVER_PIN_AIN2, CH_PWM_LEFT,  (leftDir==FWD?REV:FWD), dutyRaw());
#else
  motorRunOne(_DRIVER_PIN_AIN1, _DRIVER_PIN_AIN2, CH_PWM_LEFT,  leftDir,  dutyRaw());
#endif
}

static void motorRunRight() {
#ifdef INVERT_RIGHT
  motorRunOne(_DRIVER_PIN_BIN1, _DRIVER_PIN_BIN2, CH_PWM_RIGHT, (rightDir==FWD?REV:FWD), dutyRaw());
#else
  motorRunOne(_DRIVER_PIN_BIN1, _DRIVER_PIN_BIN2, CH_PWM_RIGHT, rightDir, dutyRaw());
#endif
}

static void motorBrakeBoth() {
  motorBrakeOne(_DRIVER_PIN_AIN1, _DRIVER_PIN_AIN2, CH_PWM_LEFT);
  motorBrakeOne(_DRIVER_PIN_BIN1, _DRIVER_PIN_BIN2, CH_PWM_RIGHT);
}

static void motorCoastBoth() {
  motorCoastOne(_DRIVER_PIN_AIN1, _DRIVER_PIN_AIN2, CH_PWM_LEFT);
  motorCoastOne(_DRIVER_PIN_BIN1, _DRIVER_PIN_BIN2, CH_PWM_RIGHT);
}

// ---------------- COMMAND PARSER ----------------
static void handleCommand(const String& lineIn) {
  String s = lineIn;
  s.trim();
  if (!s.length()) return;

  int sp1 = s.indexOf(' ');
  String cmd = (sp1 == -1) ? s : s.substring(0, sp1);
  String args = (sp1 == -1) ? "" : s.substring(sp1 + 1);
  cmd.toLowerCase();

  if (cmd == "set") {
    int pct = args.toInt();
    dutyPercent = constrain(pct, 0, 100);
    motorRunLeft();
    motorRunRight();
    Serial.printf("[ok] duty=%d%%\n", dutyPercent);
    return;
  }

  if (cmd == "dir") {
    args.trim();
    int sp = args.indexOf(' ');
    if (sp < 0) { Serial.println("[err] usage: dir L|R FWD|REV"); return; }
    String side = args.substring(0, sp); side.trim();
    String d    = args.substring(sp + 1); d.trim(); d.toUpperCase();

    Dir newDir = (d == "REV") ? REV : FWD;
    if (side == "L" || side == "l") {
      leftDir = newDir;  motorRunLeft();
      Serial.printf("[ok] LEFT dir=%s\n", (leftDir==FWD?"FWD":"REV"));
    } else if (side == "R" || side == "r") {
      rightDir = newDir; motorRunRight();
      Serial.printf("[ok] RIGHT dir=%s\n", (rightDir==FWD?"FWD":"REV"));
    } else {
      Serial.println("[err] usage: dir L|R FWD|REV");
    }
    return;
  }

  if (cmd == "brake") { motorBrakeBoth(); Serial.println("[ok] brake"); return; }
  if (cmd == "coast") { motorCoastBoth(); Serial.println("[ok] coast"); return; }

  if (cmd == "stby") {
    int v = args.toInt();
    digitalWrite(_DRIVER_PIN_STBY, v ? HIGH : LOW);
    Serial.printf("[ok] STBY=%d (%s)\n", v ? 1 : 0, v ? "enabled" : "disabled");
    return;
  }

  if (cmd == "go") {
    digitalWrite(_DRIVER_PIN_STBY, HIGH);
    motorRunLeft();
    motorRunRight();
    Serial.println("[ok] motors running");
    return;
  }

  Serial.println("[err] cmds: set <pct> | dir L|R FWD|REV | brake | coast | stby 0|1 | go");
}

// ---------------- SETUP / LOOP ----------------
void setup() {
  Serial.begin(115200);
  delay(100);

  // I2C on your pins (safe even if no device attached)
  Wire.begin(_I2C_PIN_SDA, _I2C_PIN_SCL);

  // Optional: button and IR sensors
  pinMode(_UI_BUTTON_PIN, INPUT_PULLUP);
  pinMode(_IR_PIN_0, INPUT);
  pinMode(_IR_PIN_1, INPUT);
  pinMode(_IR_PIN_2, INPUT);
  pinMode(_IR_PIN_3, INPUT);
  pinMode(_IR_PIN_4, INPUT);

  // Motor driver pins
  pinMode(_DRIVER_PIN_STBY, OUTPUT);
  pinMode(_DRIVER_PIN_AIN1, OUTPUT);
  pinMode(_DRIVER_PIN_AIN2, OUTPUT);
  pinMode(_DRIVER_PIN_BIN1, OUTPUT);
  pinMode(_DRIVER_PIN_BIN2, OUTPUT);

  // Setup LEDC PWM channels and attach to PWMA/PWMB
  ledcSetup(CH_PWM_LEFT,  PWM_FREQ_HZ, PWM_RES_BITS);
  ledcAttachPin(_DRIVER_PIN_PWMA, CH_PWM_LEFT);

  ledcSetup(CH_PWM_RIGHT, PWM_FREQ_HZ, PWM_RES_BITS);
  ledcAttachPin(_DRIVER_PIN_PWMB, CH_PWM_RIGHT);

  // Enable driver & start motors at the initial duty/direction
  digitalWrite(_DRIVER_PIN_STBY, HIGH);
  motorRunLeft();
  motorRunRight();

  Serial.println("\n=== TB6612FNG dual-motor console ===");
  Serial.printf("PWM: %d Hz, %d-bit, duty=%d%%\n", PWM_FREQ_HZ, PWM_RES_BITS, dutyPercent);
  Serial.println("cmds: set <pct> | dir L|R FWD|REV | brake | coast | stby 0|1 | go");
}

void loop() {
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    handleCommand(line);
  }

  // Example: simple heartbeat every 2s
  static uint32_t t0 = millis();
  if (millis() - t0 > 2000) {
    t0 += 2000;
    Serial.printf("status: duty=%d%%  L=%s  R=%s  STBY=%d  BTN=%d\n",
      dutyPercent,
      (leftDir==FWD?"FWD":"REV"),
      (rightDir==FWD?"FWD":"REV"),
      digitalRead(_DRIVER_PIN_STBY),
      !digitalRead(_UI_BUTTON_PIN)); // pressed if wired to GND
  }
}
