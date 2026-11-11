#include "float.h"

FloatSwitch::FloatSwitch(uint8_t pin, uint16_t debounceMs)
    : _pin(pin),
      _debounceMs(debounceMs),
      _stableClosed(false),
      _lastRawClosed(false),
      _lastEdgeMs(0),
      _lastChangeMs(0),
      _changedFlag(false) {}

void FloatSwitch::begin() {
  pinMode(_pin, INPUT_PULLUP);

  // Initial state is taken as-is, so boot doesn't report a false "change".
  _lastRawClosed = readRawClosed();
  _stableClosed  = _lastRawClosed;
  _lastEdgeMs    = millis();
  _lastChangeMs  = _lastEdgeMs;
  _changedFlag   = false;
}

// read the raw pin state (true = contact closed with pull-up enabled).
bool FloatSwitch::readRawClosed() const {
  return digitalRead(_pin) == LOW;
}

void FloatSwitch::update() {
  const uint32_t now = millis();
  const bool raw = readRawClosed();

  // any change in the raw signal restarts the debounce timer.
  if (raw != _lastRawClosed) {
    _lastRawClosed = raw;
    _lastEdgeMs = now;
  }

  // Accept the new state only once it has been steady for the debounce time.
  if (raw != _stableClosed && (now - _lastEdgeMs) >= _debounceMs) {
    _stableClosed = raw;
    _lastChangeMs = now;
  }
}

bool FloatSwitch::isClosed() const {
  return _stableClosed;
}

/* might be unnecessary?

uint32_t FloatSwitch::msSinceChange() const {
  return millis() - _lastChangeMs;
}

bool FloatSwitch::changed() {
  const bool c = _changedFlag;
  _changedFlag = false;
  return c;
}
  */