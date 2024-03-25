#pragma once

#include <Bounce2.h>
#include <embedded/digitalData.hpp>
#include <logic/structure.hpp>

#define DEBOUNCE_INTERVAL 5
#define PRESSED_STATE LOW

class DigitalReceiver {
public:
  void digitalReads();

  DigitalReceiver(DigitalData *digitalData, Mission *mission)
      : digitalData(digitalData), mission(mission) {
    pinMode(LWSS_PIN, INPUT);
    pinMode(WD_IN, INPUT);
    pinMode(WD_OUT, OUTPUT);

    asms_switch = newButton(ASMS_SWITCH_PIN);
    aats_switch = newButton(AATS_SWITCH_PIN);
  }

private:
  DigitalData *digitalData;
  Mission *mission;

  Button asms_switch, aats_switch;

  Button newButton(uint8_t pin);

  void readLwss();
  void readPneumaticLine();
  void readMission();
  void readAsmsSwitch();
  void readAatsSwitch();
  void askReadWatchdog();

  void updateLeftWheelRpm();
};

void DigitalReceiver::updateLeftWheelRpm() {
  digitalData->_left_wheel_rpm =
      digitalData->pulse_count /
      (WHEEL_MEASUREMENT_INTERVAL_MIN * PULSES_PER_ROTATION);

  digitalData->pulse_count = 0;
  digitalData->left_wheel_update_ts.update();
}

Button DigitalReceiver::newButton(uint8_t pin) {
  Button button = Button();
  button.attach(pin, INPUT_PULLUP);
  button.interval(DEBOUNCE_INTERVAL);
  button.setPressedState(PRESSED_STATE);

  return button;
}

void DigitalReceiver::digitalReads() {
  readLwss();
  readPneumaticLine();
  readMission();
  readAsmsSwitch();
  readAatsSwitch();
  askReadWatchdog();
}

void DigitalReceiver::readLwss() {
  bool const current_lwss_state = digitalRead(LWSS_PIN);

  if (current_lwss_state == HIGH && digitalData->last_lwss_state == LOW)
    digitalData->pulse_count++;

  digitalData->last_lwss_state = current_lwss_state;

    if (digitalData->left_wheel_update_ts.hasTimedOut(
        WHEEL_MEASUREMENT_INTERVAL_MIN))  // TODO André: I believe this won't work because it's in minutes, not ms
        updateLeftWheelRpm();
}

void DigitalReceiver::readPneumaticLine() {
  // TODO: wait for eletro indications
}

void DigitalReceiver::readMission() {
  // Enum value attributed considering the True Boolean Value
  *mission = static_cast<Mission>(
      digitalRead(MISSION_MANUAL_PIN) * MANUAL |
      digitalRead(MISSION_ACCELERATION_PIN) * ACCELERATION |
      digitalRead(MISSION_SKIDPAD_PIN) * SKIDPAD |
      digitalRead(MISSION_AUTOCROSS_PIN) * AUTOCROSS |
      digitalRead(MISSION_TRACKDRIVE_PIN) * TRACKDRIVE |
      digitalRead(MISSION_EBSTEST_PIN) * EBS_TEST |
      digitalRead(MISSION_INSPECTION_PIN) * INSPECTION);
}

void DigitalReceiver::readAsmsSwitch() {
  asms_switch.update();
  if (asms_switch.pressed())
    digitalData->asms_on = true;
  else
    digitalData->asms_on = false;
}

void DigitalReceiver::readAatsSwitch() {
  aats_switch.update();
  if (aats_switch.pressed())
    digitalData->aats_on = true;
  else
    digitalData->aats_on = false;
}

void DigitalReceiver::askReadWatchdog() {
  if (digitalData->wd_pulse_ts.hasTimedOut(
          WD_WAIT_INTERVAL_MS)) { // After timeout send pulse
    digitalData->wd_pulse_ts.update();
    digitalWrite(WD_OUT, HIGH);
    digitalData->watchdog_comm_state = true;
  } else if (digitalData->wd_pulse_ts.hasTimedOut(WD_PULSE_INTERVAL_MS) &&
             digitalData->watchdog_comm_state) {
    // after pulse put pin in low again
    digitalWrite(WD_OUT, LOW);
    digitalData->watchdog_comm_state = false;
  }

  if (digitalRead(WD_IN) == LOW) // if low, failure checks will open sdc
    digitalData->watchdog_state = false;
}