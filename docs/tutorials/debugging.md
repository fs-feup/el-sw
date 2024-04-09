# Debugging and Deployment

Debugging is rephered to the step of indentifying the problems the program contains in runtime. It is an extremely important process for any type of program which most often occurs naturally. For embedded software, this step is most important and it has some differente implications than normal, as the code is running in a microcontroller and depends on external sources which are hard to simulate. More than this, this process is often linked with the deployment of the code in its destination environment, adding to the complexity of doing this process correctly. This guide goes over the possibilities and dos and don'ts of debugging for embedded software.

## Tools and Teensy

Teensy microcontrollers' hardware does not support Debuggers: a type of software which allows for the code execution to be stopped at an instant in time and the values of variables analyzed. This means we have to resort to simpler methods aka **Serial monitor**. The Serial monitor is a tool that allows the programmer to print variables to a terminal connected to a serial port of the Arduino (or a microcontroller which uses the Arduino framework). 

The Serial Monitor is not available when the Teensy is running in its natural environment. As such, it is necessary to define a special condition for this code to compile, which we'll do using platform.io environments in platformio.ini:

```
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

[env:teensy41]
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
float var;

void setup() {
    #ifdef DEBUG
    Serial.begin(9600);
    #endif
    var = 2;
}

void loop() {
    var++;
    #ifdef DEBUG
    Serial.print("Var:")
    Serial.println(var)
    #endif
}
```

### Prints rules

For better guidance when debugging, use the following rules for prints:
- **Basic Structure:** "[function_name]: Message", with the functional call arguments being optional
- **State Structure:** "[function_name](arg1, arg2...)(local var1, local var2...)(global var1, global var2...)(class var1, class var2...) - line" 

