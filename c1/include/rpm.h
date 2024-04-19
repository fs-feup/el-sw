#include <math.h>
#include <algorithm>
#include <iterator>

union float2bytes
{
    int input;
    char output[4];
};
float2bytes data;

void rpm_2_byte(float rr_rpm, char *rr_rpm_byte)
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
    std::copy(std::begin(data.output), std::end(data.output), rr_rpm_byte);

    return;
}