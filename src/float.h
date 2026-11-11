#pragma once
#include <Arduino.h>

/*
Internal pull-up is used, so:
  contact CLOSED -> pin reads LOW
  contact OPEN   -> pin reads HIGH (also what a cut/corroded wire looks like)
*/

// Note: pins to use for our 2 floats: 25 and 26 (input with pull-up resistors)

// no water (float open) -> pin reads HIGH
// water present (float closed) -> pin reads LOW

/*
 * Usage:
 *   FloatSwitch tankLow(27);
 *   void setup() { tankLow.begin(); }
 *   void loop()  { tankLow.update(); if (tankLow.isLow()) { ... } }
 *
 * Call update() often (every loop pass). It is non-blocking.
 */

class FloatSwitch {
public:
  FloatSwitch(uint8_t pin, uint16_t debounceMs = 1000);

  // Configure the pin and take an initial reading. Call once in setup().
  void begin();

  // Sample the pin and run the debounce. Call regularly; never blocks.
  void update();

  // Debounced contact state (true = contact closed).
  bool isClosed() const;

private:
  bool readRawClosed() const;

  uint8_t  _pin;
  uint16_t _debounceMs;

  bool     _stableClosed;   // debounced state
  bool     _lastRawClosed;  // last raw sample
  uint32_t _lastEdgeMs;     // when the raw sample last differed from the one before
  uint32_t _lastChangeMs;   // when the debounced state last changed
  bool     _changedFlag;
};