// Sensor bench test: float switches, ultrasonic sensor and Tank class.
// Build/upload with:   pio run -e sensor_test -t upload && pio device monitor -e sensor_test
// (the sensor_test env in platformio.ini only compiles this file + the sensor classes)

#include <Arduino.h>

#include "float_switch.h"
#include "tank.h"
#include "ultrasonic.h"

// Pin configs
#define TANK_FLOAT_PIN   25   // float used by the main-tank
#define MINI_TANK_FLOAT_PIN  26   // second float, used by the mini-tank
#define ULTRA_TRIG_PIN  16   
#define ULTRA_ECHO_PIN  17   

#define FLOAT_DEBOUNCE_MS  1000
#define PRINT_INTERVAL_MS  1000UL

FloatSwitch floatTank(TANK_FLOAT_PIN, FLOAT_DEBOUNCE_MS);
FloatSwitch floatMiniTank(MINI_TANK_FLOAT_PIN, FLOAT_DEBOUNCE_MS);
UltrasonicSensor ultrasonic(ULTRA_ECHO_PIN, ULTRA_TRIG_PIN);
Tank* tank = nullptr;  // created in setup() so pin setup happens after the Arduino core is up

static bool lastTankClosed;
static bool lastMiniTankClosed;
static uint32_t lastPrintMs = 0;

static const char* contactStr(bool closed) { return closed ? "CLOSED (water)" : "OPEN (no water)"; }

void setup() {
  Serial.begin(115200);
  delay(1000);  // give the serial monitor time to connect

  Serial.println();
  Serial.println("===== Sensor test =====");
  Serial.printf("Float Tank  pin %d | Float MINI_TANK pin %d\n", TANK_FLOAT_PIN, MINI_TANK_FLOAT_PIN);
  Serial.printf("Ultrasonic trig %d | echo %d\n", ULTRA_TRIG_PIN, ULTRA_ECHO_PIN);
  Serial.printf("Tank calibration: empty = %.1f cm, full = %.1f cm\n", TANK_EMPTY_DISTANCE_CM, TANK_FULL_DISTANCE_CM);

  // The ultrasonic constructor sets pinMode, but redo it here in case it ran before the core was ready
  pinMode(ULTRA_TRIG_PIN, OUTPUT);
  pinMode(ULTRA_ECHO_PIN, INPUT);

  floatTank.begin();
  floatMiniTank.begin();
  tank = new Tank(floatTank, ultrasonic);  // Tank constructor calls floatTank.begin()

  lastTankClosed = floatTank.isClosed();
  lastMiniTankClosed = floatMiniTank.isClosed();
  Serial.printf("Initial: float TANK %s, float MINI_TANK %s\n", contactStr(lastTankClosed), contactStr(lastMiniTankClosed));
  Serial.println("Move the floats by hand / move an object in front of the ultrasonic sensor.");
  Serial.println("=======================");
}

void loop() {
  // Debounce runs every pass (non-blocking)
  floatTank.update();
  floatMiniTank.update();

  // Report float changes immediately so you can see the debounce working
  if (floatTank.isClosed() != lastTankClosed) {
    lastTankClosed = floatTank.isClosed();
    Serial.printf("[%lu ms] float TANK changed -> %s\n", millis(), contactStr(lastTankClosed));
  }
  if (floatMiniTank.isClosed() != lastMiniTankClosed) {
    lastMiniTankClosed = floatMiniTank.isClosed();
    Serial.printf("[%lu ms] float MINI_TANK changed -> %s\n", millis(), contactStr(lastMiniTankClosed));
  }

  // Periodic full status
  if (millis() - lastPrintMs >= PRINT_INTERVAL_MS) {
    lastPrintMs = millis();

    Serial.println("-----------------------");
    Serial.printf("[%lu ms]\n", lastPrintMs);

    // Raw pin levels (with pull-up: LOW = closed, HIGH = open/disconnected)
    Serial.printf("Float TANK : raw=%s  debounced=%s\n",
                  digitalRead(TANK_FLOAT_PIN) == LOW ? "LOW" : "HIGH", contactStr(floatTank.isClosed()));
    Serial.printf("Float MINI_TANK: raw=%s  debounced=%s\n",
                  digitalRead(MINI_TANK_FLOAT_PIN) == LOW ? "LOW" : "HIGH", contactStr(floatMiniTank.isClosed()));

    // Direct ultrasonic reading (the class also prints "ultra: ..." itself)
    float dist = ultrasonic.readDistanceCm();
    if (dist < 0) {
      Serial.println("Ultrasonic: NO ECHO - check wiring/power/range");
    } else {
      Serial.printf("Ultrasonic: %.1f cm\n", dist);
    }

    delay(60);  // let echoes die out before the Tank class fires the sensor again

    // Tank class
    float level = tank->GetTankLevelPercent();
    if (level < 0) {
      Serial.println("Tank level: UNKNOWN (sensor fault)");
    } else {
      Serial.printf("Tank level: %.1f %%\n", level);
    }
    Serial.printf("Tank empty: %s\n", tank->IsTankEmpty() ? "YES" : "no");
  }
}
