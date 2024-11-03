#pragma once

#include <FlexCAN_T4.h>
#include <Arduino.h>
#include <string>

#include "comm/communicatorSettings.hpp"
#include "model/systemData.hpp"
#include "comm/utils.hpp"
#include "debugUtils.hpp"

/**
 * @brief Array of standard CAN message codes to be used for FIFO filtering
 * Each Code struct contains a key and a corresponding message ID.
 */
inline std::array<Code, 7> fifoCodes = {{{0, C1_ID},
                                         {1, BAMO_RESPONSE_ID},
                                         {2, AS_CU_EMERGENCY_SIGNAL},
                                         {3, MISSION_FINISHED},
                                         {4, AS_CU_ID},
                                         {5, RES_STATE},
                                         {6, RES_READY}}};

/**
 * @brief Array of extended CAN message codes to be used for FIFO filtering
 * Contains the key and corresponding message ID for extended messages.
 */
inline std::array<Code, 1> fifoExtendedCodes = {{
    {7, STEERING_ID},
}};

/**
 * @brief Class that contains definitions of typical messages to send via CAN
 * It serves only as an example of the usage of the strategy pattern,
 * where the communicator is the strategy
 */
class Communicator
{
private:
    // Static FlexCAN_T4 object for CAN2 interface with RX and TX buffer sizes specified
    inline static FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;

public:
    // Pointer to SystemData instance for storing system-related data
    inline static SystemData *_systemData = nullptr;

    /**
     * @brief Constructor for the Communicator class
     * Initializes the Communicator with the given system data instance.
     * @param system_data Pointer to the SystemData instance.
     */
    explicit Communicator(SystemData *systemdata);

    /**
     * @brief Initializes the CAN bus
     */
    void init();

    /**
     * @brief Parses the message received from the CAN bus
     */
    static void parse_message(const CAN_message_t &msg);

    /**
     * @brief Sends a message to the CAN bus
     * @param len Length of the message
     * @param buffer Buffer containing the message
     * @param id ID of the message
     * @return 0 if successful
     */
    template <std::size_t N>
    static int send_message(unsigned len, const std::array<uint8_t, N> &buffer, unsigned id);

    /**
     * @brief Callback for message from AS CU
     */
    static void pcCallback(const uint8_t *buf);

    /**
     * @brief Callback for data from C1 Teensy
     */
    static void c1Callback(const uint8_t *buf);

    /**
     * @brief Callback RES default callback
     */
    static void resStateCallback(const uint8_t *buf);

    /**
     * @brief Callback for RES activation
     */
    static void resReadyCallback();

    /**
     * @brief Callback from inversor, for alive signal and data
     */
    static void bamocarCallback(const uint8_t *buf);

    /**
     * @brief Callback for steering actuator information
     */
    static void steeringCallback();

    /**
     * @brief Publish AS State to CAN
     */
    static int publish_state(int state_id);

    /**
     * @brief Publish AS Mission to CAN
     */
    static int publish_mission(int mission_id);

    /**
     * @brief Publish AS Mission to CAN
     */
    static int publish_debug_log(SystemData system_data, uint8_t sate, uint8_t state_checkup);

    /**
     * @brief Publish rl wheel rpm to CAN
     */
    static int publish_left_wheel_rpm(double value);
};

inline Communicator::Communicator(SystemData *system_data)
{
    _systemData = system_data;
}

void Communicator::init()
{

    // Library initialization
    can2.begin();
    can2.setBaudRate(500000);

    // Enable FIFO
    can2.enableFIFO();
    can2.enableFIFOInterrupt();

    // Set filters
    can2.setFIFOFilter(REJECT_ALL);
    for (auto &fifoCode : fifoCodes)
        can2.setFIFOFilter(fifoCode.key, fifoCode.code, STD);

    // Set filters for extended
    for (auto &fifoExtendedCode : fifoExtendedCodes)
        can2.setFIFOFilter(fifoExtendedCode.key, fifoExtendedCode.code, EXT);

    // Set callback
    can2.onReceive(FIFO, parse_message);

    DEBUG_PRINT("CAN2 started");
    can2.mailboxStatus(); // Prints CAN mailbox info
}

inline void Communicator::c1Callback(const uint8_t *buf)
{
    if (buf[0] == HYDRAULIC_LINE)
    {
        _systemData->sensors._hydraulic_line_pressure = (buf[2] << 8) | buf[1];
    }
    else if (buf[0] == RIGHT_WHEEL_CODE)
    {
        double right_wheel_rpm = (buf[4] << 24) | (buf[3] << 16) | (buf[2] << 8) | buf[1];
        right_wheel_rpm *= WHEEL_PRECISION; // convert back adding decimal part
        _systemData->sensors._right_wheel_rpm = right_wheel_rpm;
    }
    else if (buf[0] == LEFT_WHEEL_CODE)
    {
        double left_wheel_rpm = (buf[4] << 24) | (buf[3] << 16) | (buf[2] << 8) | buf[1];
        left_wheel_rpm *= WHEEL_PRECISION; // convert back adding decimal part
        _systemData->sensors._left_wheel_rpm = left_wheel_rpm;
    }
}

inline void Communicator::resStateCallback(const uint8_t *buf)
{
    bool emg_stop1 = buf[0] & 0x01;
    bool emg_stop2 = buf[3] >> 7 & 0x01;
    bool go_switch = (buf[0] >> 1) & 0x01;
    bool go_button = (buf[0] >> 2) & 0x01;

    // DEBUG_PRINT("Received message from RES");

    if (go_button || go_switch)
        _systemData->r2dLogics.processGoSignal();
    else if (!emg_stop1 && !emg_stop2)
    {
        DEBUG_PRINT("RES Emergency Signal");
        _systemData->failureDetection.emergencySignal = true;
    }

    _systemData->failureDetection.radio_quality = buf[6];
    bool signal_loss = (buf[7] >> 6) & 0x01;
    if (!signal_loss)
    {
        _systemData->failureDetection.resSignalLossTimestamp.reset(); // making ure we dont receive only ignal lo for the defined time interval
        // DEBUG_PRINT("SIGNAL OKAY");
    }
    else
    {
        // Too many will violate the disconnection time limit
        // DEBUG_PRINT("SIGNAL LOSS");
    }
}

inline void Communicator::resReadyCallback()
{
    // If res sends boot message, activate it
    DEBUG_PRINT("Received RES Ready");
    unsigned id = RES_ACTIVATE;
    std::array<uint8_t, 2> msg = {0x01, NODE_ID}; // 0x00 in byte 2 for all nodes

    send_message(2, msg, id);
}

inline void Communicator::bamocarCallback(const uint8_t *buf)
{
    _systemData->failureDetection.inversorAliveTimestamp.reset();

    if (buf[0] == BTB_READY)
    {
        if (buf[1] == false)
        {
            DEBUG_PRINT("BTB not ready");
            _systemData->failureDetection.ts_on = false;
            DEBUG_PRINT_VAR(_systemData->failureDetection.ts_on);
        }
    }
    else if (buf[0] == VDC_BUS)
    {
        unsigned dc_voltage = (buf[2] << 8) | buf[1];
        _systemData->failureDetection.dc_voltage = dc_voltage;

        if (dc_voltage < DC_THRESHOLD)
        {
            _systemData->failureDetection.dcVoltageHoldTimestamp.reset();
            if (_systemData->failureDetection.dcVoltageDropTimestamp.checkWithoutReset())
            {
                DEBUG_PRINT("DC Voltage Drop under defined value for more than 150ms");

                _systemData->failureDetection.ts_on = false;
            }
        }
        else
        {
            _systemData->failureDetection.dcVoltageDropTimestamp.reset();
            if (_systemData->failureDetection.dcVoltageHoldTimestamp.checkWithoutReset())
            {

                _systemData->failureDetection.ts_on = true;
            }
        }
    }
}

inline void Communicator::pcCallback(const uint8_t *buf)
{
    DEBUG_PRINT("PC callback");
    if (buf[0] == PC_ALIVE)
    {
        _systemData->failureDetection.pcAliveTimestamp.reset();
        // DEBUG_PRINT("Received AS CU Alive");
    }
    else if (buf[0] == MISSION_FINISHED)
    {
        _systemData->missionFinished = true;
    }
    else if (buf[0] == AS_CU_EMERGENCY_SIGNAL)
    {
        _systemData->failureDetection.emergencySignal = true;
        DEBUG_PRINT_VAR(_systemData->failureDetection.emergencySignal);
    }
}

inline void Communicator::steeringCallback()
{
    _systemData->failureDetection.steerAliveTimestamp.reset();
}

inline void Communicator::parse_message(const CAN_message_t &msg)
{
    switch (msg.id)
    {
    case AS_CU_ID:
        pcCallback(msg.buf);
    case RES_STATE:
        resStateCallback(msg.buf);
        break;
    case RES_READY:
        resReadyCallback();
        break;
    case C1_ID:
        c1Callback(msg.buf); // rwheel, lwheel and hydraulic line
        break;
    case BAMO_RESPONSE_ID:
        bamocarCallback(msg.buf);
        break;
    case STEERING_ID:
        steeringCallback();
        break;
    default:
        break;
    }
}

inline int Communicator::publish_state(const int state_id)
{
    const std::array<uint8_t, 2> msg = {STATE_MSG, static_cast<uint8_t>(state_id)};
    send_message(2, msg, MASTER_ID);
    return 0;
}

inline int Communicator::publish_mission(int mission_id)
{
    const std::array<uint8_t, 2> msg = {MISSION_MSG, static_cast<uint8_t>(mission_id)};

    send_message(2, msg, MASTER_ID);
    return 0;
}

inline int Communicator::publish_debug_log(SystemData system_data, uint8_t state, uint8_t state_checkup)
{

    // 8 bytes for the CAN message
    uint32_t hydraulic_pressure = system_data.sensors._hydraulic_line_pressure; // 32-bit value
    // TBD, consider extracting to a function in utils.hpp
    uint8_t emergency_signal_bit = system_data.failureDetection.emergencySignal;
    uint8_t pneumatic_line_pressure_bit = system_data.digitalData.pneumatic_line_pressure;
    uint8_t engage_ebs_check_bit = system_data.r2dLogics.engageEbsTimestamp.checkWithoutReset();
    uint8_t release_ebs_check_bit = system_data.r2dLogics.releaseEbsTimestamp.checkWithoutReset();
    uint8_t steer_dead_bit = system_data.failureDetection.steer_dead_;
    uint8_t pc_dead_bit = system_data.failureDetection.pc_dead_;
    uint8_t inversor_dead_bit = system_data.failureDetection.inversor_dead_;
    uint8_t res_dead_bit = system_data.failureDetection.res_dead_;
    uint8_t asms_on_bit = system_data.digitalData.asms_on;
    uint8_t ts_on_bit = system_data.failureDetection.ts_on;
    uint8_t sdc_state_open_bit = system_data.digitalData.sdcState_OPEN;
    uint8_t mission = system_data.mission;
    const std::array<uint8_t, 8> msg = {
        DBG_LOG_MSG,
        (hydraulic_pressure >> 24) & 0xFF,
        (hydraulic_pressure >> 16) & 0xFF,
        (hydraulic_pressure >> 8) & 0xFF,
        hydraulic_pressure & 0xFF,
        (emergency_signal_bit & 0x01) << 7 |
            (pneumatic_line_pressure_bit & 0x01) << 6 |
            (engage_ebs_check_bit & 0x01) << 5 |
            (release_ebs_check_bit & 0x01) << 4 |
            (steer_dead_bit & 0x01) << 3 |
            (pc_dead_bit & 0x01) << 2 |
            (inversor_dead_bit & 0x01) << 1 |
            (res_dead_bit & 0x01),
        (asms_on_bit & 0x01) << 7 |
            (ts_on_bit & 0x01) << 6 |
            (sdc_state_open_bit & 0x01) << 5 |
            (state_checkup & 0x0F),
        (mission & 0x0F) | ((state & 0x0F) << 4)};

    send_message(8, msg, MASTER_ID);

    uint32_t dc_voltage = system_data.failureDetection.dc_voltage;
    uint8_t pneumatic_line_pressure_bit_1 = system_data.digitalData.pneumatic_line_pressure_1;
    uint8_t pneumatic_line_pressure_bit_2 = system_data.digitalData.pneumatic_line_pressure_2;

    const std::array<uint8_t, 7> msg2 = {
        DBG_LOG_MSG_2,
        (dc_voltage >> 24) & 0xFF,
        (dc_voltage >> 16) & 0xFF,
        (dc_voltage >> 8) & 0xFF,
        dc_voltage & 0xFF,
        pneumatic_line_pressure_bit_1 & 0x01,
        pneumatic_line_pressure_bit_2 & 0x01};
    send_message(7, msg2, MASTER_ID);
    return 0;
}

inline int Communicator::publish_left_wheel_rpm(double value)
{
    std::array<uint8_t, 5> msg;
    create_left_wheel_msg(msg, value);

    send_message(5, msg, MASTER_ID);
    return 0;
}

template <std::size_t N>
inline int Communicator::send_message(const unsigned len, const std::array<uint8_t, N> &buffer, const unsigned id)
{
    CAN_message_t can_message;
    can_message.id = id;
    can_message.len = len;
    for (unsigned i = 0; i < len; i++)
    {
        can_message.buf[i] = buffer[i];
    }
    can2.write(can_message);

    return 0;
}
