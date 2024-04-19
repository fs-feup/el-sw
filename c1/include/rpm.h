#include <math.h>

union float2bytes
{
    int input;
    char output[4];
}; 
float2bytes data;

void rpm_2_byte(float rr_rpm, char * output)
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
    output[0] = data.output[0];
    output[1] = data.output[1];
    output[2] = data.output[2];
    output[3] = data.output[3];

    return;
}