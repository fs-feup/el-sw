#pragma once

#include <Arduino.h>

#include "digitalSettings.hpp"
#include "metro.h"

/**
 * @brief Class responsible for controlling digital outputs in the Master Teensy.
 *
 * The DigitalSender class handles various operations such as controlling LEDs,
 * EBS valves, SDC state, and watchdog signals. It also manages different operational states
 * such as emergency, manual, ready, driving, and finish.
 */
class DigitalSender {
private:
  Metro blink_imer_{LED_BLINK_INTERVAL};  ///< Timer for blinking LED

  /**
   * @brief Turns off both ASSI LEDs (yellow and blue).
   */
  static void turn_off_assi();

public:
  // Array of valid output pins
  static constexpr std::array<int, 9> validOutputPins = {
      ASSI_BLUE_PIN,   ASSI_YELLOW_PIN,    EBS_VALVE_1_PIN,
      EBS_VALVE_2_PIN, MASTER_SDC_OUT_PIN, SDC_LOGIC_CLOSE_SDC_PIN,
      // SDC_LOGIC_WATCHDOG_OUT_PIN
  };

  /**
   * @brief Constructor for the DigitalSender class.
   *
   * Initializes the pins for output as defined in validOutputPins.
   */
  DigitalSender() {
    for (const auto pin : validOutputPins) {
      pinMode(pin, OUTPUT);
    }
  }

  /**
   * @brief Opens the SDC in Master and SDC Logic.
   */
  static void open_sdc();

  /**
   * @brief Closes the SDC in Master and SDC Logic.
   */
  static void close_sdc();

  /**
   * @brief Activates the solenoid EBS valves.
   */
  static void activate_ebs();

  /**
   * @brief Deactivates the solenoid EBS valves.
   */
  static void deactivate_ebs();

  /**
   * @brief ASSI LEDs blue flashing, sdc open and buzzer ringing.
   */
  void enter_emergency_state();

  /**
   * @brief Everything off, sdc closed.
   */
  static void enter_manual_state();

  /**
   * @brief Everything off, sdc open.
   */
  static void enter_off_state();

  /**
   * @brief ASSI yellow LED on, ebs valves activated, sdc closed.
   */
  static void enter_ready_state();

  /**
   * @brief ASSI LEDs yellow flashing, ebs valves deactivated, sdc closed.
   */
  void enter_driving_state();

  /**
   * @brief ASSI blue LED on, ebs valves activated, sdc open.
   */
  static void enter_finish_state();

  /**
   * @brief Blinks the LED at the given pin.
   * @param pin The pin to blink.
   */
  void blink_led(int pin);
};

inline void DigitalSender::open_sdc() {
  digitalWrite(SDC_LOGIC_CLOSE_SDC_PIN, LOW);
  digitalWrite(MASTER_SDC_OUT_PIN, LOW);
}

inline void DigitalSender::close_sdc() {
  digitalWrite(SDC_LOGIC_CLOSE_SDC_PIN, HIGH);
  digitalWrite(MASTER_SDC_OUT_PIN, HIGH);
}

inline void DigitalSender::activate_ebs() {
  digitalWrite(EBS_VALVE_1_PIN, HIGH);
  digitalWrite(EBS_VALVE_2_PIN, HIGH);
}

inline void DigitalSender::deactivate_ebs() {
  digitalWrite(EBS_VALVE_1_PIN, LOW);
  digitalWrite(EBS_VALVE_2_PIN, LOW);
}

inline void DigitalSender::turn_off_assi() {
  analogWrite(ASSI_YELLOW_PIN, LOW);
  analogWrite(ASSI_BLUE_PIN, LOW);
}

inline void DigitalSender::enter_emergency_state() {
  turn_off_assi();
  blink_imer_.reset();
  activate_ebs();
  open_sdc();
}

inline void DigitalSender::enter_manual_state() {
  turn_off_assi();
  deactivate_ebs();
  close_sdc();
}

inline void DigitalSender::enter_off_state() {
  turn_off_assi();
  deactivate_ebs();
  open_sdc();
}

inline void DigitalSender::enter_ready_state() {
  turn_off_assi();
  analogWrite(ASSI_YELLOW_PIN, 1023);
  activate_ebs();
  close_sdc();
}

inline void DigitalSender::enter_driving_state() {
  turn_off_assi();
  blink_imer_.reset();
  deactivate_ebs();
  close_sdc();
}

inline void DigitalSender::enter_finish_state() {
  turn_off_assi();
  analogWrite(ASSI_BLUE_PIN, 1023);
  activate_ebs();
  open_sdc();
}

inline void DigitalSender::blink_led(const int pin) {
  static bool blink_state = false;
  if (blink_imer_.check()) {
    blink_state = !blink_state;
    analogWrite(pin, blink_state * 1023);
  }
}
