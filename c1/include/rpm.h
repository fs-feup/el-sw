
#include <FlexCAN_T4.h>

extern float rr_rpm;
extern CAN_message_t rr_rpm_msg;
union float2bytes
{
    int input;
    char output[4];
}; 
extern float2bytes data;

void sendRPM()
{
    /*
    1st we multiply rpm by 100 to get a 2 decimal place value.
    The roundf() function rounds rpm to the nearest integer value.
    */
    data.input = roundf(rr_rpm * 100);
    /*
    The order of the bytes in the output array depends on the endianness the system.
    -> little-endian system, the least significant byte will be at output[0],
    and the most significant byte will be at output[3].
    -> big-endian system, it's the other way around.
    */
    rr_rpm_msg.buf[4] = data.output[3];
    rr_rpm_msg.buf[3] = data.output[2];
    rr_rpm_msg.buf[2] = data.output[1];
    rr_rpm_msg.buf[1] = data.output[0];
}