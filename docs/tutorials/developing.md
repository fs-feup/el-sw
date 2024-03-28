# Development Guide

## Good Practices

Check the rules on programming in [Notion](https://www.notion.so/Coding-Guidelines-2a14cfa826a846f1845f89e5cc54b0ec?pvs=4).

## Executable Files (.cpp in src)
By default, the tests defined with unity only fetch files from the include folder (.hpp) files. There is a flag that can allow them to also fetch files from the src folder but this action will lead the tests to include the main file, which includes the Arduino library, which is not present in the desktop and thus will remove the tests' ability to be run in desktop. What this means is a change in paradigm of development: all code that should be tested should be entirely defined in the include folder, in .hpp files. This means the only executable files (.cpp) in the project are ones that contain calls to these header files' components, such as the main files and few other helper programs. In conclusion -> code in .hpp files in the include directory.

## Compiling and Running
You can use the platformio functions both through the VSCode gui or the terminal. I prefer the terminal as it is easier to set some options. To upload the project into the board, simply run:
```sh
pio run
```
To select a certain environment:
```sh
pio run --environment teensy41
```
The native environment is not selected by default.
You can also start a debug session using:
```sh
pio debug
```
For more information on debugging, visit [this](https://docs.platformio.org/en/latest/plus/debugging.html). For more tools, run:
```sh
pio -h
```
## Testing

To run the tests in the desktop:
```sh
pio test --environment native
```
This will not run the tests in the *test_embedded* folder, as the environment is set to ignore them.
To run the tests when using a teensy:
```sh
pio test
```
More information on commands can be found [here](https://docs.platformio.org/en/latest/core/userguide/cmd_test.html).