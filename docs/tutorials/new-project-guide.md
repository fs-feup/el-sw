# Creating project
In case you need to create a new project for a new microcontroller.
## Creating new project
1. Select PIO Home - Home
2. New Project
3. Select the hardware
4. You are done
## Configuring project
How to configure a project for **EL&SW** boards at FS FEUP:
1. Notes on the base structure of the project:
	1. **include** folder contains header files (hpp) - almost all programming goes here, all testable code goes here
	2. **src** folder contains executable files
	3. **lib** folder contains external libraries
	4. **test** folder contains unit tests
2. create 'comm', 'embedded' and 'logic' sub-folders inside *include* (purely organizational as of now)
	1. **comm** folders contain the code related to communication (CAN). No files external to this folder should include information on ports, communication protocols or include CAN libraries. Files from this folder should not contain any other logic other than pure translation from CAN to useful information.
	2. **embedded** folder contains the code related to IO operations with the board, which require inclusion of Arduino.h library or any other resource only available when connected to a board. Files from this folder should not contain any other logic other the direct calling functions and classes to the board's functionalities.
	3. **logic** folder contains the code regarding all the logic of the actual programs running in the boards. This folder should not contain calls to either communication or embedded libraries. This folder can and should be further subdivided if it contains many files. 
3. create 'test_comm', 'test_embedded', 'test_logic', 'test_logic_native' (logic that can be tested on pc) and 'test_integration' (integration tests) sub-bfolders inside *test*.
4. In platformio.ini
	1. ![platformio.ini file](../assets/new-project-setup/inifile.png)
	2. under [platformio] general rules are defined 
		1. **default_envs** defines the default environments to be compiled (exclude native so that it is not built to the board, it will fail)
	3. under [env:name] we define a development environment with the name "name"
		1. **platform** defines the environment it should work in
		2. **build_flags** is used to define flags, which in this case is used for the compiler to know which files to compile, either PC's SOCKET CAN communications of Teenzy's FLEX CAN communications code
		3. **board** defines the model of the board
		4. **build_type** can configure some definitions. The debug type enables the code to be executed step by step (in debug mode) in the board, with a tool that provides an inside view into the state of the variables and program. More information [here](https://docs.platformio.org/en/latest/projectconf/build_configurations.html).
		5. **framework** defines the framework of the board
		6. **check_tool** defines the static analysis tool to be used. To run static analysis, run ```pio check```
		7. **test_ignore** sets the test folders to ignore in the given environment
		8. More information on the platformio.ini can be found [here](https://docs.platformio.org/en/latest/projectconf/index.html)
The way the platformio.ini file is defined, the native environment is only made to be used for running tests and is not compiled by default.
