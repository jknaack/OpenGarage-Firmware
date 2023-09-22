Fork of https://github.com/OpenGarage/OpenGarage-Firmware

# Updates/Changes

## New Features
* Support for turning off the blinking light after N blinks (useful if you are in the garage often)
    * Options -> Basic -> Blinks After Start (0 is always blink, default = 0)
    * Useful if you are in the garage often and the light is distracting
* Support for setting DHCP Host Name
    * Options -> Advanced -> Host Name (existing field)
    * The DHCP hostname that is sent to your router now the mDNS hostname, rather than the devicename.  This should mean that your router shows the correct device name instead of OG-ESP123. (Note: you may have to reboot your router to see the new name.)
* Add Start Time and Uptime info to the Log Page
* Improved support for isolated network environments (or no availalbe NTP server)
    * Set Options -> Advanced -> NTP Server to **0.0.0.0** to disable NTP requests
    * NTP Server request failures use a backoff strategy on retries to reduce unnecessary retry attempts
    * Note: Running without an NTP Server may have side effects or cause certain features to stop working
        * The clock will reset to 1970-01-01T00:00:00Z after every reboot.
        * Log time will be incorrect.
        * Door Automation: Closing at a certain hour will not work as the hour of the day will be unknown/wrong
* Converted garage door status images on the home page to SVG and added an SVG favicon on the home page
* Support for alternate AM2320 sensor wiring to allow for wiring the AM2320 and Garage Door Switch Sensors at the same time
    * Options -> T/H Sensor -> AM2320 (I2C, Alternate Wiring) (SDA=0, SCL=5 wires)
    * The regular wiring is still available as AM2320 (I2C) using GPOI Pins 4 (SDA) and 5 (SCL), but if you want to use the Garage Door Sensor we must move off of GPIO 4.  This wiring uses the Button pin, 0 (SDA) and 5 (SCL).  We can't read the temperature when the button is pressed, but that is most likely only used during setup, and the pin is unused otherwise.

## Dev Enhancements
* Python html2raw implementation to support html -> c++ raw string conversion (C++ binary doesn't work on Windows)
    * Includes support for alternate C++ Raw String prefix in cases where the default prefix is in the html file somewhere
* Platform IO Config Improvements
    * Added a debug and release NodeMCUv2 config
        * Added an OpenGarage specific debug definition so it can be turned on without turning on the OpenThings debug info
    * Created a shared environment for common board info
    * Created a default build config

## Issues Fixed
* Issue #71 - webapp car status not correct when door is open 
* Issue #90 - Devices without internet access fail ~50% of API calls (w/ ESP8266/nodeMCU)

## Library and Version Updates
* Bump OpenGarage version to 1.2.2
* Bump espressif8266 to 4.x
* Bump of AM2320 Library (bug fixes)
* Bump of OneWire Library (fixes compiler warning)
* Bump of DHTesp (bug fixes + fixes compiler warning)
* Bump of OpenThingsFramework (fixes debug compiler error on espressif8266 3.0.0+)
* Bump of arduinoWebSockets (fixes compiler warning)

**Note:** Most of these are only updated if building with Platform IO

## Firmware Compilation Instructions:

There are three methods to compile the firmware. You can choose any of them.

### Method 1: Use PlatformIO
Install Visual Studio Code (VS Code) (https://code.visualstudio.com/). Then launch VS Code and install the **platformio** extension. Once the extension is installed, open this `OpenGarage` folder. A `platformio.ini` file is provided in the folder, which defines all libraries and settings required to compile the firmware. Click `PlatformIO:Build` at the bottom of the screen to build the firwmare.

### Method 2: Use makeEspArduino and Makefile

#### Step A: Install ESP8266 core and required libraries
Follow https://github.com/plerup/makeEspArduino to install **ESP8266 core 2.7.4**. Then download or `git clone` the following libraries to a folder such as `$(HOME)/Arduino/libraries` (note the version of each library):

* Blynk library for Arduino (https://github.com/blynkkk/blynk-library) (v1.0.1)
* MQTT PubSUbClient https://github.com/knolleary/pubsubclient (v2.8)
* AM2320 library: https://github.com/hibikiledo/AM2320/releases (v1.1.3)
* OneWire library: https://www.pjrc.com/teensy/td_libs_OneWire.html
* DallasTemperature library: https://github.com/milesburton/Arduino-Temperature-Control-Library/releases (v3.9.1)
* DHTesp library: https://github.com/beegee-tokyo/DHTesp/releases (v1.18)
* OpenThingsFramework library: https://github.com/OpenThingsIO/OpenThings-Framework-Firmware-Library
* WebSocket library: https://github.com/Links2004/arduinoWebSockets/releases (v2.3.5)
* This (OpenGarage) source code (download the repository and extract the `OpenGarage` folder to your Arduino's `libraries` folder.)

#### Step B: Use the Makefile

To compile the firmware code using makeESPArduino, simply run `make` in command line. Before doing so, you probaby need to open `Makefile` and modify some path variables therein to match where you installed the `esp8266` folder.


### Method 3: Use the Arduino IDE

#### Step A: Install Arduino IDE, ESP8266 core, and required libraries

* Install Arduino IDE (https://arduino.cc).
* Launch Arduino IDE and install ESP8266 core 2.7.4 for Arduino (https://github.com/esp8266/Arduino/releases/tag/2.7.4)
* Install the libraries listed above.
* Download this repository and extract the `OpenGarage` folder to your Arduino's `libraries` folder.

#### Step B: Compile
To compile using Arduino: launch Arduino, and select

* File -> Examples -> OpenGarage -> mainArduino.
* Tools -> Board -> Generic ESP8266 Module (if this is not available, check if you've installed the ESP8266 core).
* Tools -> Flash Mode -> DIO.
* Tools -> Flash Size -> **4M (1M SPIFFS)**.

Press Ctrl + R to compile. The compiled firmware (named mainArduino.cpp.bin) is by default copied to a temporary folder.

### Making changes to the firmware
If you want to customize the firmware, feel free to modify any part of the source code. The firmware supports built-in web interface. The HTML data are embedded as program memory strings. Go to the `html` subfolder, which contains the original HTML files, and a `html2raw` tool to convert these HTML files into program memory strings. If you make changes to the HTML files, re-run `html2raw` to re-generate the program memory strings. You can also directly modify the program memory strings, but keeping the original HTML files makes it easy to check and verify your HTML files in a browser.


