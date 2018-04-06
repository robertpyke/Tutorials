Intro
============

The following lab assumes you have the Arduino IDE installed, and can install the necessary libraries and board.
Tested with an MKR-1000.

Template
================

Let's start with the [base_wifi.ino](https://github.com/robertpyke/Tutorials/blob/master/aws/iot/base_wifi.ino) and
[arduino_secrets.h](https://github.com/robertpyke/Tutorials/blob/master/aws/iot/arduino_secrets.h) for our initial sketch.

The following is a brief explanation of each part of the initial base_wifi.ino:

File Overview
------------------

The following are the 3 main sections of the file:

```c
#include ...

setup() {
  ...
}

loop() {
  ...
}
```

The first section contains the #include statements. These are your imports (the libraries you're using).

The second section is the setup function. This runs once on start-up, and can be used to configure your board, clients, etc.

The third section is the main loop. This function will be called after setup, repeatedly. It's where you'll implement your main device business logic.

Libraries/constants
-----------------------

```c
#include "arduino_secrets.h"
#include <SPI.h>
#include <WiFi101.h>
```

The arduino_secrets.h file contains our constants.

For now, you just need to set the WIFI SSID and PASS (if your wifi is password protected).

```c
#define SECRET_SSID "Guest"
#define SECRET_PASS ""
```

Wifi101 is the Wifi library that supports our device's wifi chipset. [Reference/Docs](https://www.arduino.cc/en/Reference/WiFi101)

Setup
-------------------------------

This is the setup function:

```c
void setup() {
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");

  // Allow the hardware to sort itself out
  delay(1500);

  printWiFiData();
}
```

Let's go into each section, and briefly discuss what's happening.

```c
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
```

In this section, we're opening the serial port. The serial port allows for us to communicate to/from the board. The arduino IDE has a serial monitor, that will let us see the serial output from the board. For this project, we'll use the serial port as a logger, letting us watch what's happening on the board. 9600 is the baud rate (9600 bits/second).

```c
 // check for the presence of the shield:
 if (WiFi.status() == WL_NO_SHIELD) {
   Serial.println("WiFi shield not present");
   // don't continue:
   while (true);
 }
```

Here, we're checking for the existence of a WiFi shield/module. In arduino speak, the shield is just a physical module. This is just confirming the device is fitted with Wifi support.

```c
  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
```

Here we see a loop. While the device is not connected to wifi, it loops, attempting to connect. It gives the board 10 seconds to connect, before giving up, and looping again.

This is the first time you're seeing Serial.print (and Serial.println). Serial.print(ln) is printing to the Serial port (as we discussed before). When you run this code, you'll be able to see these "print statements" in the serial monitor.

If you're attempting to connect to a network without password protection, remove the pass argument.
