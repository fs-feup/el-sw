| Publisher | Id | length | buf[0]| buf[1]| buf[2]| buf[3] | buf[4] | buf[5] | buf[6] | buf[7] | Comments |
|:-------------|:------|---------:|:------------|:------------------|:------------------|:-----------|---------:|---------:|---------:|---------:|:---------------------------------------------------------------------------------------------------|
| Teensy C3 | 0x111 | 4 | APPS1 LSB | APPS1 MSB | APPS2 LSB | APPS 2 MSB | | | | |More information [here](https://docs.google.com/document/d/1OHhF_4qy_adlV3IM-yzU2rS6dTElfm43VSPI2wBDXpY/edit?usp=sharing) | 
| Teensy C3 / AS CU | 0x201 | 3 | 0x90 | Torque LSB | Torque MSB | | | | | | Torque Request to Bamocar |
| Bamocar | 0x181 | 4 | 0xE8 | 0x01 | 0x00 | 0x00 | | | | | receive enable Response form Bamocar |
| Teensy C3 | 0x201 | 3 | 0x8E | 0x44 | 0x4D | | | | | | clear Bamocar errors (this is not a bypass of the errors if the error persist it won't be cleared) |
| Teensy C3 | 0x201 | 3 | 0x51 | 0x00 | 0x00 | | | | | | This is part of the setup sequence of the bamocar ([see Ndrive manual](https://drive.google.com/drive/folders/1bdFkmG5v9gZULkJfAdY3rqtZdVJwwXQX))|
| Teensy C3 | 0x201 | 3 | 0x3D | 0xE2 | 0x00 | | | | | | Transmission request REGID 0xE2, 0x00 -> only one transmission, [all REGID Information](https://drive.google.com/file/d/1UVcGhsBRz_DpuVszRFBb6By628RkIuaZ/view) |
| Bamocar| 0x181 | 4 | 0xE2 | 0x01 | 0x00 | 0x00 | | | | | BTB Response, if we receive this message means that bamocar is ready for operation |
| Teensy C3 | 0x201 | 3 | 0x3D | 0xE8 | 0x00 | | | | | | Transmission Request REGID 0xE8, "Enable", this is also part of the setup sequence of the bamocar |
| Teensy C3 | 0x201 | 3 | 0x3D | 0x40 | 0x00 | | | | | | Transmission Request REGID 0x40, "Status |
| Teensy C3 | 0x201 | 3 | 0x51 | 0x04 | 0x00 | | | | | | This is part of the Shutdown sequence of the bamocar ([see Ndrive manual](https://drive.google.com/drive/folders/1bdFkmG5v9gZULkJfAdY3rqtZdVJwwXQX)) |
| Teensy C3 | 0x201 | 3 | 0x3D | 0xEB | 0x64 | | | | | | Transmission Request DC Voltage, 100ms cycle |
| Teensy C3 | 0x201 | 3 | 0x3D | 0xCE | 0x64 | | | | | | Transmission Request RPM, 10ms cycle |
| Teensy C3 | 0x201 | 3 | 0x3D | 0x30 | 0x64 | | | | | | Transmission Request Speed , 10ms cycle|
| Teensy C3 | 0x201 | 3 | 0x3D | 0x5F | 0x64 | | | | | | Transmission Request Current , 10ms cycle |
| Teensy C3 | 0x201 | 3 | 0x3D | 0x49 | 0x64 | | | | | | Transmission Request Motor Temperature, 10ms cycle|
| Teensy C3 | 0x201 | 3 | 0x3D | 0xA0 | 0x64 | | | | | | Transmission Request Torque Motor , 10ms cycle |
| Teensy C3 | 0x201 | 3 | 0x3D | 0X8A | 0x64 | | | | | | Transmission Request Voltage Motor, 10ms cycle |
| Teensy C3 | 0x201 | 3 | 0x3D | 0xEB | 0x64 | | | | | | Transmission Request DC Voltage , 10ms cycle |
| Bamocar| 0x181 | 4 | 0x30 | Speed LSB | Speed MSB | --- | | | | | Receive current Speed from bamocar, "speed = (msg.buf[2] << 8) BITWISE_OR msg.buf[1];"; 32767 MAX corresponds to max RPM (configurable) |
| Bamocar| 0x181 | 4 | 0xEB | DCVotlage LSB | DCVoltage MSB | --- | | | | | Receive current DC Voltage; 32767 MAX corresponds to max Voltage (configurable) |
| Bamocar| 0x181 | 4 | 0xCE | RPM LSB | RPM MSB | --- | | | | | Receive motor RPM, MAX RPM |
| Bamocar| 0x181 | 4 | 0x5F | Current LSB | Current MSB | --- | | | | | Receive Current, MAX CURRENT |
| Bamocar| 0x181 | 4 | 0x49 | Motor Temp LSB | Motor Temp MSB | --- | | | | | Receive Motor temperature |
| Bamocar| 0x181 | 4 | 0x4A | Inv Temp LSB| Inv Temp MSB | --- | | | | | Receive Inversor temperature (I assume that "powerStage" is the inverter temperature) |
| Bamocar| 0x181 | 4 | 0x8A | Motor Voltage LSB | Motor Voltage MSB | --- | | | | | Receive Motor Voltage |
| Bamocar| 0x181 | 4 | 0xA0 | Torque LSB | Torque MSB | --- | | | | | Receive Torque |
| BMS | 0x666 | 3 | Current LSB | Current MSB | | | | | | | Receive Current from BMS |
| Teensy C1 | 0x123 | 3 | 0x90 | Brake Value LSB | Brake Value MSB | | | | | | Hydraulic Line Brake Pressure, scaled by 1e1 in an integer value |
| AS CU | 0x400 | 1 | 0x41 | | | | | | | | PC Alive Signal |
| AS CU | 0x400 | 1 | 0x42 | | | | | | | | Mission Finished Signal |
| AS CU | 0x400 | 1 | 0x43 | | | | | | | | Autonomous System / Computational Unit Emergency Detection Signal |
| Teensy C1 | 0x123 | 5 | 0x11 | Right Wheel RPM LSB | Right Wheel RPM | Right Wheel RPM | Right Wheel RPM MSB | | | | Right Wheel Current RPM Value, scaled by 1e2 in an integer value |
| Teensy C1 | 0x123 | 5 | 0x12 | Left Wheel RPM LSB | Right Wheel RPM | Right Wheel RPM | Right Wheel RPM MSB | | | | Left Wheel Current RPM Value, scaled by 1e2 in an integer value |
| Steering Actuator | 0x295D | 8 | Steering Angle LSB | Steering Angle MSB | | | | | | | Steering Angle Value | Steering angle variation from Cubemars (message format is dubious, check datasheet)
| AS CU | 0x45D | 4 | Steering Angle LSB | Steering Angle LSB2 | Steering Angle MSB2 | Steering Angle MSB | | | | | Steering angle command for Cubemars - Weird formula, requires multiplication by 10000, check datasheet or ROS-CAN code
| RES | 0x180 + 0x11 | 8 | PMO2000 | PMO2001 | PMO2002 | PMO2003 | PMO2004 | PMO2005 | PMO2006 | PMO2007 | Res State Message. 0x11 in ID refers to the 0x11 Node ID defined by FSG. PMO2000: emergency - bit 0, go signals - bit 1 & 2. PMO2003: emergency - bit 7. PMO2006: Radio Quality (0-100%). PMO2007: signal loss - bit 6 |
| RES | 0x700 + 0x11 | 1 | 0x00 | | | | | | | | RES Ready Message Informs that the device is initialized. 0x11 in ID refers to the 0x11 Node ID defined by FSG |
| Master | 0x000 | 2 | 0x01 | 0x11 |  | | | | | | Set RES to operational mode. First byte sets the state. Second refers to the 0x11 Node ID defined by FSG |
| Master | 0x300 | 2 | 0x31 | Current AS State | | | | | | | Current ASState represented by the defined ENUM value {0 - AS_Manual, 1 - AS_Off, 2 - AS_Ready, 3 - AS_Driving, 4 - AS_Finished, 5 - AS_Emergency} |
| Master | 0x300 | 2 | 0x32 | Current Mission |  | | | | | | Current Mission represented by the defined ENUM value {0 - Manual, 1 - Acceleration, 2 - Skidpad, 3 - Autocross, 4 - Trackdrive, 5 - EBS_Test, 6 - Inspection} |
| Master | 0x500 | 8 | Speed Actual | Speed Target | Steering Angle Actual | Steering Angle Target | Hydraulic Brake Actual | Hydraulic Brake Target | Motor Moment Actual | Motor Moment Target | Logging Message with Vehicle's Current Driving Mechanics States as defined by competition in ([FSG Handbook 2023](https://www.formulastudent.de/fileadmin/user_upload/all/2023/important_docs/FSG23_Competition_Handbook_v1.0.pdf)) |
| Master | 0x501 | 6 | Acceleration Longitudinal LSB | Acceleration Longitudinal MSB | Acceleration Lateral LSB | Acceleration Lateral MSB | Yaw Rate LSB | Yaw Rate MSB | | | Logging Message with Vehicle's Current Vehicle Driving Mechanics Controls as defined by competition in ([FSG Handbook 2023](https://www.formulastudent.de/fileadmin/user_upload/all/2023/important_docs/FSG23_Competition_Handbook_v1.0.pdf)) |
| Master | 0x502 | 5 | AS + EBS + AMI States | Steering + Service Brake States + Lap Counter + Cones Count Actual | Cones Count Actual + Cones Count All | Cones Count All | Cones Count All | | | |  Logging Message with Vehicle's Current System Status as defined by competition in ([FSG Handbook 2023](https://www.formulastudent.de/fileadmin/user_upload/all/2023/important_docs/FSG23_Competition_Handbook_v1.0.pdf)) |
| IMU | 0x175 | 7 | Yaw rate LSB | Yaw rate MSB | Reserved | Reserved | Acc Y-axis LSB | Acc Y-axis MSB | Reserved | |Quantization Yaw Rate 0.005 [°/s/digit] and Quantization Acc Y-axis 0.0001274 [g/digit] |
| IMU | 0x179 | 7 | Roll rate LSB | Roll rate MSB | Reserved | Reserved | Acc X-axis LSB | Acc X-axis MSB | Reserved | |Quantization Roll Rate 0.005 [°/s/digit] and Quantization Acc X-axis 0.0001274 [g/digit] | 
| IMU | 0x17C | 7 | Pitch rate LSB | Pitch rate MSB | Reserved | Reserved | Acc Z-axis LSB | Acc Z-axis MSB | Reserved | |Quantization Pitch Rate 0.005 [°/s/digit] and Quantization Acc Z-axis 0.0001274 [g/digit] |       
| Bosch Steering Wheel Angle Sensor | 0x0a1 | 4 | Angle LSB | Angle MSB | Speed | 0b-X-X-X-X-X-Trim-Cal-OK | Reserved | |
| External Device | 0x7C0 | 2 | 0b-X-X-X-X-X-CCW | Reserved | | | | | | | A zero adjustment is needed before using the sensor for the first time. To do so, reset the calibration with CCW = 0x5. After reseting the calibration, a new calibration needs to be started with CCW = 0x3 |
| External Device | 0x665 | | | | | | | | | | This ID is used to up the car in R2D state, without the need to turn on the TS. We need to be careful to not use this ID anywhere else|

[Bamocar Manual](https://drive.google.com/file/d/1AB3R3GgfrNnoZevtf9uZ19a8wwtScbqD/view?usp=sharing)
