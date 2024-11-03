// ReSharper disable CppMemberFunctionMayBeConst
#pragma once

#include <Bounce2.h>
#include <model/digitalData.hpp>
#include <model/structure.hpp>

#include "digitalSettings.hpp"
#include "debugUtils.hpp"

/**
 * @brief Class responsible for the reading of the digital
 * inputs into the Master teensy
 */
class DigitalReceiver
{
public:
    static double _current_left_wheel_rpm;    // Class variable to store the left wheel RPM (non-static)
    static unsigned long last_wheel_pulse_ts; // Timestamp of the last pulse for RPM calculation

    /**
     * @brief read all digital inputs
     */
    void digital_reads();

    /**
     * @brief callback to update rl wheel rpm
     */
    static void updateLeftWheelRpm();

    /**
     * @brief Constructor for the class, sets pintmodes and buttons
     */
    DigitalReceiver(DigitalData *digital_data, Mission *mission)
        : digital_data_(digital_data), mission_(mission)
    {
        pinMode(SDC_STATE_PIN, INPUT);
        pinMode(MISSION_ACCELERATION_PIN, INPUT);
        pinMode(MISSION_AUTOCROSS_PIN, INPUT);
        pinMode(MISSION_EBSTEST_PIN, INPUT);
        pinMode(MISSION_INSPECTION_PIN, INPUT);
        pinMode(MISSION_MANUAL_PIN, INPUT);
        pinMode(MISSION_SKIDPAD_PIN, INPUT);
        pinMode(MISSION_TRACKDRIVE_PIN, INPUT);
        pinMode(ASMS_IN_PIN, INPUT);
        pinMode(SENSOR_PRESSURE_1_PIN, INPUT);
        pinMode(SENSOR_PRESSURE_2_PIN, INPUT);
    }

private:
    DigitalData *digital_data_; ///< Pointer to the digital data storage
    Mission *mission_;         ///< Pointer to the current mission status

    unsigned int asms_change_counter_ = 0;      ///< counter to avoid noise on asms
    unsigned int aats_change_counter_ = 0;      ///< counter to avoid noise on aats
    unsigned int pneumatic_change_counter_ = 0; ///< counter to avoid noise on pneumatic line
    unsigned int mission_change_counter_ = 0;   ///< counter to avoid noise on mission change
    Mission last_tried_mission_ = Mission::MANUAL;      ///< Last attempted mission state

    /**
     * @brief Reads the pneumatic line pressure states and updates the DigitalData object.
     * Debounces input changes to avoid spurious transitions.
     */
    void read_pneumatic_line();

    /**
     * @brief Reads the current mission state based on input pins and updates the mission object.
     * Debounces input changes to avoid spurious transitions.
     */
    void read_mission();

    /**
     * @brief Reads the ASMS switch state and updates the DigitalData object.
     * Debounces input changes to avoid spurious transitions.
     */
    void read_asms_switch();

    /**
     * @brief Reads the AATS state and updates the DigitalData object.
     * Debounces input changes to avoid spurious transitions.
     */
    void read_aats_state();
};

inline void DigitalReceiver::digital_reads()
{
    read_pneumatic_line();
    read_mission();
    read_asms_switch();
    read_aats_state();
}

inline void DigitalReceiver::read_pneumatic_line()
{
    bool pneumatic1 = digitalRead(SENSOR_PRESSURE_1_PIN);
    bool pneumatic2 = digitalRead(SENSOR_PRESSURE_2_PIN); // TODO: maybe poorly read

    digital_data_->pneumatic_line_pressure_1_ = pneumatic1;
    digital_data_->pneumatic_line_pressure_2_ = pneumatic2;
    bool temp_res = pneumatic1 && pneumatic2;

    // Only change the value if it has been different 5 times in a row
    pneumatic_change_counter_ = temp_res == digital_data_->pneumatic_line_pressure_ ? 0 : pneumatic_change_counter_ + 1;
    if (pneumatic_change_counter_ >= DIGITAL_INPUT_COUNTER_LIMIT)
    {
        digital_data_->pneumatic_line_pressure_ = temp_res; // both need to be True
        pneumatic_change_counter_ = 0;
    }
}

inline void DigitalReceiver::read_mission()
{

    Mission temp_res = static_cast<Mission>(
        digitalRead(MISSION_MANUAL_PIN) * to_underlying(Mission::MANUAL) |
        digitalRead(MISSION_ACCELERATION_PIN) * to_underlying(Mission::ACCELERATION) |
        digitalRead(MISSION_SKIDPAD_PIN) * to_underlying(Mission::SKIDPAD) |
        digitalRead(MISSION_AUTOCROSS_PIN) * to_underlying(Mission::AUTOCROSS) |
        digitalRead(MISSION_TRACKDRIVE_PIN) * to_underlying(Mission::TRACKDRIVE) |
        digitalRead(MISSION_EBSTEST_PIN) * to_underlying(Mission::EBS_TEST) |
        digitalRead(MISSION_INSPECTION_PIN) * to_underlying(Mission::INSPECTION));

    mission_change_counter_ = (temp_res == *mission_) &&
                                     (temp_res == last_tried_mission_)
                                 ? 0
                                 : mission_change_counter_ + 1;
    this->last_tried_mission_ = temp_res;
    if (mission_change_counter_ >= DIGITAL_INPUT_COUNTER_LIMIT)
    {
        *mission_ = temp_res;
        mission_change_counter_ = 0;
    }
}

inline void DigitalReceiver::read_asms_switch()
{
    bool temp_res = digitalRead(ASMS_IN_PIN);

    // Only change the value if it has been different 5 times in a row
    asms_change_counter_ = temp_res == digital_data_->asms_on_ ? 0 : asms_change_counter_ + 1;
    if (asms_change_counter_ >= DIGITAL_INPUT_COUNTER_LIMIT)
    {
        digital_data_->asms_on_ = temp_res;
        asms_change_counter_ = 0;
    }
}

inline void DigitalReceiver::read_aats_state()
{
    // AATS is on if SDC is closed (SDC STATE PIN AS HIGH)
    bool is_sdc_closed = !digitalRead(SDC_STATE_PIN);
    // Only change the value if it has been different 5 times in a row
    aats_change_counter_ = is_sdc_closed == digital_data_->sdc_open_ ? 0 : aats_change_counter_ + 1;
    if (aats_change_counter_ >= DIGITAL_INPUT_COUNTER_LIMIT)
    {
        digital_data_->sdc_open_ = is_sdc_closed; // both need to be True
        aats_change_counter_ = 0;
    }
}
