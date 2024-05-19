# TEENSY C1

![Distribution of electronics in FSFEUP 01](./assets/c1-c3-overview/car-scheme.png)

## Initial notes:  
TeensyC1 - Caixa 1 - Located inside PCBs box

TeensyC3 - Caixa 3 - Located behind DashBoard

BMS - Battery Management System - Inside TSAC

Bamocar - Inverter/Controller

APPS - Accelerator Pedal Position Sensor - Pedal Box

BL - Brake Light

## TeensyC1 Functionalities:

- BL Logic
- Logging
- CAN BUS Sniffer (Bamocar, BMS and TeensyC3)

## Timers

All timers are made with ElapsedMillis. This library makes this easy by allowing you to create variables (objects) that automatically increase as time elapses. [Documentation](https://github.com/pfeerick/elapsedMillis/wiki)


## BL Logic

Input: Brake sensor (BRAKE_SENSOR_PIN, A5)

Output: Brake Light Command (BRAKE_LIGHT, 2)

![Brake Light Logic Diagram](./assets/c1-c3-overview/diagram-bl.png)

## Logging

This writes the data to a .csv file inside an SD card.  
Teensy 4.1 already has a slot for an SD Card.  
The function write_to_file() is inside logging.cpp, it is not relevant for this part.

![Logging Diagram](./assets/c1-c3-overview/diagram-logging.png)

## CAN BUS Sniffer

This microcontroller is one node of our CAN bus.  
CAN bus is a broadcast type of bus.  
At the beginning of the code we perform the CAN bus setup: canbusSetup()

In this setup, we enable a filter to only deal with important messages for this node. this filter has a FIFO logic (first in first out)  
In this case, we only allow messages with ID 0x111, 0x666 and 0x181, which are APPS values from teensyC3, BMS and Bamocar, respectively.

When the TeensyC1 receives one message with one of the valid IDs, it calls canbusSniffer().  

[CAN_BAMOCAR.pdf](https://drive.google.com/file/d/1UVcGhsBRz_DpuVszRFBb6By628RkIuaZ/view?usp=sharing) <- Here you have all the CAN Documentation of the inverter

Receive address Rx ID = 0x201

Transmit address Tx ID = 0x181

Transmission rate NBT = 4025 (→ 500 kBaud)

BMS message has been configured by the team in the Orion BMS Software.

BMS id = 0x666

Caixa 3 also sends information to Caixa 1, the APPS value to log it.
