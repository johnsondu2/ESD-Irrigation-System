#pragma once

class Bed {
 private:
  // sensor reading
  float sensor;

  // valve status
  int valve;  // val 1 or 0 or bool up to u

  // FSM
  // more to do

 public:
  Bed();
  ~Bed();
  // to do

  void update();
};
