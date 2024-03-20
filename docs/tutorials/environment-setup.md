# New Project Setup

## Prerequisites

- Git set up with GitHub and ssh
- VSCode

You can find tutorials for this [here](https://github.com/fs-feup/autonomous-systems/blob/main/docs/tutorials/environment_setup/coding_environment.md) (ignore parts rephering to ROS).

## Links
- [Platformio documentation](https://docs.platformio.org/en/latest/)

## Set Up

### Platformio Manual Installation
1. Install vscode
3. Make sure you have python3 installed
4. Install python3-venv: ```sudo apt install python3-venv```
5. Install gcc
	```sh
	sudo apt install gcc
	sudo apt install g++
	```
2. Install [platformio extension](https://platformio.org/install/ide?install=vscode)
6. Install platformio on pc (optional)
	```sh
	sudo apt install python3-pip
	pip install -U platformio
	```
7. Open platformio extension in vscode

### Configuring system for uploads

Lastly, you need to update/create a file in a certain location to allow uploads to the teensy via USB port. The file is the following:

```sh
# UDEV Rules for Teensy boards, http://www.pjrc.com/teensy/
#
# The latest version of this file may be found at:
#   http://www.pjrc.com/teensy/00-teensy.rules
#
# This file must be placed at:
#
# /etc/udev/rules.d/00-teensy.rules    (preferred location)
#   or
# /lib/udev/rules.d/00-teensy.rules    (req'd on some broken systems)
#
# To install, type this command in a terminal:
#   sudo cp 00-teensy.rules /etc/udev/rules.d/00-teensy.rules
#
# After this file is installed, physically unplug and reconnect Teensy.
#
ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04*", ENV{ID_MM_DEVICE_IGNORE}="1", ENV{ID_MM_PORT_IGNORE}="1"
ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04[789a]*", ENV{MTP_NO_PROBE}="1"
KERNEL=="ttyACM*", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04*", MODE:="0666", RUN:="/bin/stty -F /dev/%k raw -echo"
KERNEL=="hidraw*", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04*", MODE:="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="04*", MODE:="0666"
KERNEL=="hidraw*", ATTRS{idVendor}=="1fc9", ATTRS{idProduct}=="013*", MODE:="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="1fc9", ATTRS{idProduct}=="013*", MODE:="0666"

#
# If you share your linux system with other users, or just don't like the
# idea of write permission for everybody, you can replace MODE:="0666" with
# OWNER:="yourusername" to create the device owned by you, or with
# GROUP:="somegroupname" and mange access using standard unix groups.
#
# ModemManager tends to interfere with USB Serial devices like Teensy.
# Problems manifest as the Arduino Serial Monitor missing some incoming
# data, and "Unable to open /dev/ttyACM0 for reboot request" when
# uploading.  If you experience these problems, disable or remove
# ModemManager from your system.  If you must use a modem, perhaps
# try disabling the "MM_FILTER_RULE_TTY_ACM_INTERFACE" ModemManager
# rule.  Changing ModemManager's filter policy from "strict" to "default"
# may also help.  But if you don't use a modem, completely removing
# the troublesome ModemManager is the most effective solution.
```

The instructions for the installation of the file are in the file's comments.

### Unit Testing
Some examples test files were developed in the project to demonstrate how they should be written:
1. Folder separation
2. Unit testing - each test tests only one function/method and one scenario of that function
3. Each test file should correspond to one implementation file and be name 'test_<file_name>', to improve traceability
The test framework used is **unity**. It was chosen due to having support with platformio out of the box and being able to run tests both in native environment and embedded. To get more information on the framework, visit [this website](https://docs.platformio.org/en/latest/advanced/unit-testing/frameworks/unity.html#unit-testing-frameworks-unity). More information in pio testing can be found [here](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html).
### Testing Environments
Some tests can be run on the computer, as they only depend on logic or ever present libraries. However, many of the tests depend in some way of functionalities inherent to arduino. As such, most testing will be performed using a breadboard for testing with a Teensy.

```

## Links
- [Platformio ini file](https://docs.platformio.org/en/latest/projectconf/index.html)
