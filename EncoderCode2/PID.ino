/*
  ESP32-S3 + TB6612FNG + Encoders + QuickPID (simple, forward-only)
  - Uses your modules: Driver, Encoder, Lcd, Timer (Timer unused, kept for consistency), IrArray (unused here)
  - PID controls motor speed by matching "encoder ticks per sample"
  - Sample period = 20 ms (50 Hz)
  - Serial @ 115200:
      spd <L> <R>       e.g.  spd 6 6     (targets ~6 ticks per 20 ms on each wheel)
      pidk <Kp> <Ki> <Kd> e.g. pidk 12 80 0

  Notes:
  - Keep wheels off the ground for first tests.
  - If encoders count backward or stay at 0, check wiring/pins in config.hpp.
  - LCD warning about AVR libs is OK; your Lcd wrapper may still work on ESP32.
*/

#include <Arduino.h>
#include <QuickPID.h>            // Installed in your libraries folder
#include "module_driver.hpp"     // Your TB6612 wrapper: driver->drive(pwmL, pwmR)
#include "module_ir.hpp"         // Not used here, included for completeness
#include "module_lcd.hpp"        // Your LCD wrapper
#include "module_timer.hpp"      // Not used here
#include "module_encoder.hpp"    // Your encoder wrapper: encoder->getPulseCount()
#include "config.hpp"            // Must define __I2C_ADDR_LCD, __ENCODER_PIN_L_A, __ENCODER_PIN_R_A

// --------- ESP32 spinlock used by your Encoder class ----------
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// --------- Your module objects ----------
Driver  *driver;
IrArray *irArray;
Lcd     *lcd;
Timer   *timer;
Encoder *encoderL;
Encoder *encoderR;

// ================== PID CONFIG (keep it simple) ==================
// Control loop period: 20 ms (50 Hz)
static const uint16_t CTRL_MS = 20;

// Setpoints: desired encoder ticks per 20 ms (small integers are good starters)
static float spL = 5.0f;   // left wheel target (ticks/sample)
static float spR = 5.0f;   // right wheel target (ticks/sample)

// Inputs: measured ticks per sample (computed every 20 ms)
static float inL = 0.0f;
static float inR = 0.0f;

// Outputs: PWM commands to motors (0..255), forward-only for simplicity
static float outL = 0.0f;
static float outR = 0.0f;

// Starter gains (tweak with "pidk" if needed)
// Kp pushes harder on error; Ki removes steady error; Kd left at 0 to keep it calm
static float Kp = 12.0f, Ki = 80.0f, Kd = 0.0f;

// QuickPID objects (your version wants float* and supports a 7-arg ctor)
QuickPID pidL(&inL, &outL, &spL, Kp, Ki, Kd, QuickPID::Action::direct);
QuickPID pidR(&inR, &outR, &spR, Kp, Ki, Kd, QuickPID::Action::direct);

// Encoder count bookkeeping to compute "ticks per sample"
static int32_t lastCntL = 0;
static int32_t lastCntR = 0;

// ================== tiny serial command parser ==================
static void handleSerial() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  line.trim();

  // spd <L> <R>    -> set both wheel targets (ticks per sample)
  if (line.startsWith("spd ") || line.startsWith("SPD ")) {
    line.remove(0, 4);
    int sp = line.indexOf(' ');
    if (sp > 0) {
      float l = line.substring(0, sp).toFloat();
      float r = line.substring(sp + 1).toFloat();
      if (l < 0) l = 0;  // keep forward-only
      if (r < 0) r = 0;
      spL = l; spR = r;
      Serial.printf("[ok] spd L=%.2f  R=%.2f  (ticks/%dms)\n", spL, spR, CTRL_MS);
    } else {
      Serial.printf("[err] usage: spd <L> <R>  (ticks per %dms)\n", CTRL_MS);
    }
    return;
  }

  // pidk <Kp> <Ki> <Kd>   -> retune gains on the fly
  if (line.startsWith("pidk ") || line.startsWith("PIDK ")) {
    line.remove(0, 5);
    float p, i, d;
    int n = sscanf(line.c_str(), "%f %f %f", &p, &i, &d);
    if (n == 3) {
      Kp = p; Ki = i; Kd = d;
      pidL.SetTunings(Kp, Ki, Kd);
      pidR.SetTunings(Kp, Ki, Kd);
      Serial.printf("[ok] pidk Kp=%.3f Ki=%.3f Kd=%.3f\n", Kp, Ki, Kd);
    } else {
      Serial.println("[err] usage: pidk <Kp> <Ki> <Kd>");
    }
    return;
  }

  if (line.equalsIgnoreCase("help")) {
    Serial.printf("cmds:\n  spd <L> <R>   (ticks per %dms)\n  pidk <Kp> <Ki> <Kd>\n", CTRL_MS);
  }
}

// ================== Arduino SETUP ==================
void setup() {
  Serial.begin(115200);
  delay(200);

  // Instantiate your modules (as in your snippet)
  driver   = new Driver();
  irArray  = new IrArray();
  lcd      = new Lcd(__I2C_ADDR_LCD, "Trace Invader", "PID Ready");
  timer    = new Timer();
  encoderL = new Encoder(mux, __ENCODER_PIN_L_A);
  encoderR = new Encoder(mux, __ENCODER_PIN_R_A);

  // Configure QuickPID
  pidL.SetOutputLimits(0, 255);                 // motor PWM range
  pidR.SetOutputLimits(0, 255);
  pidL.SetSampleTimeUs(CTRL_MS * 1000);         // 20 ms in microseconds
  pidR.SetSampleTimeUs(CTRL_MS * 1000);
  pidL.SetMode(QuickPID::Control::automatic);   // start controlling
  pidR.SetMode(QuickPID::Control::automatic);

  Serial.printf("QuickPID ready. CTRL=%d ms  start sp: L=%.1f R=%.1f (ticks/sample)\n", CTRL_MS, spL, spR);
  Serial.println("Type:  spd <L> <R>    e.g.  spd 6 6");
  Serial.println("       pidk <Kp> <Ki> <Kd>    e.g.  pidk 12 80 0");
}

// ================== Arduino LOOP ==================
void loop() {
  handleSerial();  // let you change setpoints/gains while running

  static uint32_t t0 = millis();
  uint32_t now = millis();

  // Run control logic every CTRL_MS (20 ms)
  if (now - t0 >= CTRL_MS) {
    t0 += CTRL_MS;

    // 1) Read cumulative encoder counts from your Encoder objects
    int32_t cntL = encoderL->getPulseCount();
    int32_t cntR = encoderR->getPulseCount();

    // 2) Convert to "ticks per sample" by differencing
    int32_t dL = cntL - lastCntL;  lastCntL = cntL;
    int32_t dR = cntR - lastCntR;  lastCntR = cntR;

    // 3) Feed PID inputs (forward-only: clamp negatives to 0)
    inL = (dL > 0) ? (float)dL : 0.0f;
    inR = (dR > 0) ? (float)dR : 0.0f;

    // 4) Compute new outputs (0..255), one PID per wheel
    pidL.Compute();
    pidR.Compute();

    // 5) Drive motors with your Driver (PWM left/right)
    //    If one side hits max (255) and can't reach setpoint, reduce spd or tune Kp/Ki.
    driver->drive((int)outL, (int)outR);

    // 6) Show raw counts on LCD (optional but handy)
    lcd->display(String(cntL), String(cntR));

    // 7) Optional: uncomment for serial telemetry
    // Serial.printf("L: sp=%.1f in=%.1f out=%d | R: sp=%.1f in=%.1f out=%d\n",
    //               spL, inL, (int)outL,   spR, inR, (int)outR);
  }

  // (Any other non-blocking work can run here)
}
