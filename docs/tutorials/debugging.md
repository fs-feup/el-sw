# Debugging and Deployment

Debugging is rephered to the step of indentifying the problems the program contains in runtime. It is an extremely important process for any type of program which most often occurs naturally. For embedded software, this step is most important and it has some differente implications than normal, as the code is running in a microcontroller and depends on external sources which are hard to simulate. This guide goes over the possibilities and dos and don'ts of debugging for embedded software.

## Tools and Teensy

Teensy microcontrollers' hardware does not support Debuggers: a type of software which allows for the code execution to be stopped at an instant in time and the values of variables analyzed. This means we have to resort to simpler methods aka **Serial monitor**. The Serial monitor is a tool that allows the programmer to print variables to a terminal connected to a serial port of the Arduino (or a microcontroller which uses the Arduino framework). 

The Serial Monitor is not available when the Teensy is running in its natural environment. As such, it is necessary to define a special condition for this code to compile, which we'll do using platform.io environments in platformio.ini:

```ini
[platformio]
default_envs = teensy41

[env:teensy41]
platform = teensy
board = teensy41
framework = arduino
check_tool = cppcheck
check_flags = --enable=all

[env:breadboard-test]
platform = teensy
build_type = test
board = teensy40
framework = arduino
check_tool = cppcheck
check_flags = --enable=all

[env:teensy41-debug]
platform = teensy
board = teensy41
build_flags = -D DEBUG
framework = arduino
check_tool = cppcheck
check_flags = --enable=all

[env:breadboard-debug]
platform = teensy
build_type = test
build_flags = -D DEBUG
board = teensy40
framework = arduino
check_tool = cppcheck
check_flags = --enable=all

```

As seen in the example file, for each board there are two environments:
- one for deployment, without build flag
- one for debugging, with the build flag

The build flag can be used then in the code to encapsulate the Serial monitor code inside ifdef directives:

```c++
#ifdef DEBUG
#include <Arduino.h>
#define DEBUG_PRINT(str)              \
  Serial.print(millis());             \
  Serial.print(": ");                 \
  Serial.print(__PRETTY_FUNCTION__);  \
  Serial.print(' ');                  \
  Serial.print(__FILE__);             \
  Serial.print(':');                  \
  Serial.print(__LINE__);             \
  Serial.print(' ');                  \
  Serial.println(str);                \
#else
#define DEBUG_PRINT(str)
#endif
```
We can then easily use defined macros anywhere in the code, as long as we have `#include "debugUtils.hpp"` at the top of the file:
```cpp
inline void ASState::calculateState() {
    DEBUG_PRINT("Calculating State...");
    ...
```
This should print something like that:
```bash
20255: void ASState::calculateState() include/logic/stateLogic.hpp:27 Calculating State...
```

### Prints rules

For better guidance when debugging, use the following rules for new macros you may wish to define:
- **Basic Structure:** `CURRENT_TIME: PRETTY_FUNCTION FILEPATH:LINE [MESSAGE] [#VARIABLE: VARIABLE]...`

