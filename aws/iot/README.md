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

In this section, we're opening the serial port. The serial port allows for us to communicate to/from the board. The arduino IDE has a serial monitor, that will let us see the serial output from the board. For this project, we'll use the serial port as a logger, letting us watch what's happening on the board. 9600 is the baud rate (9600 bits/second). Baud rate should not be confused with bored rate, which is how quickly you get bored of these labs, or board rate, which is the rate at which ardunio boards are released.

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

```c
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");

  // Allow the hardware to sort itself out
  delay(1500);

  printWiFiData();
```

Once the wifi connect loop finishes, the device is connected. We print that we're connected, then sleep for 1500 ms (1.5 seconds), before printing the wifi information. Once we print the wifi data, the setup function is complete. The device will then start looping on the <code>loop()</code> function.

Before we move to the <code>loop</code> function, let's look at the <code>printWifiData</code> function:

```c

void printWiFiData() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
```

This sample code is provided by the Wifi101 module. This function prints general information about the wifi connection. The main thing I want to draw your attention to is the *rssi*. We'll use this later in the lab. The *rssi* is the "received signal strength indicator" of the Wifi connection. This number will be negative, and the closer the value is to 0, the stronger the wifi connection.

Main Loop
----------

This is the main loop:

```c
void loop() {
  delay(500);
  Serial.println("looping..");
}
```

As you can see, it's pretty bare-bones. We loop every 500ms, and print looping..

Task 1
-----------

Okay, you've had a breif explanation of the main parts of the file. You should have a sketch with one main .ino file, and an arduino_secrets.h file. Your first task, update the .h file, and the .ino file, with the correct Wifi SSID/PASS. If the wifi doesn't have a password, don't forget to remove the pass paramater where indicated above. Once you've made those few changes, run the code, and make sure you're seeing the device connect to wifi, and then loop on the main loop.

![task1-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task1-complete.png "Task 1 Complete")


Task 2
-------------

Update the main loop to print the rssi. Play around with the board, and try to partially block the wifi connection. You should see the rssi value change. Feel free to tweak the <code>delay()</code> value, to print the rssi more frequently.

![task2-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task2-complete.png "Task 2 Complete")

Reading Pins
============

Reading the Built-in LED state
----------------------------------

In our previous section, we got our device connected to wifi, and printed our connection strength in the main loop. Let's start reading device pins. This is probably going to be super complic...

```c
  long ledValue = digitalRead(LED_BUILTIN);
  Serial.println("LED:");
  Serial.println(ledValue);
```

.. *never mind*.. Turns out, reading pins is pretty easy. Digital read will read the value of a pin as a 0 (low) or 1 (high).
In this example, we're reading the value of the built-in LED.

Task 3
------

Read the value of the LED pin in the main loop. Don't cheat... actually do it. I'll know if you didn't.

Notice anything, is your bored rate increasing (*get it, it's punny*)? The LED value isn't changing.

![task3-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task3-complete.png "Task 3 Complete")


Reading Digital Sensor Pins
-----------------------

In the previous section, we learnt how to read a pin, but it's always the same value. Let's introduce a sensor. We're going to focus on digital sensors, those that are either on, or off. In my example, I'm going to integrate a vibration sensor, but you can use any other digital sensor.

Reading a sensor is just a matter of reading the specific pin the sensor is connected to:

```c
  long pin0Value = digitalRead(0);
  Serial.println(pin0Value);
```

In the above code, we're reading the value of pin 0.

[digitalRead reference](https://www.arduino.cc/reference/en/language/functions/digital-io/digitalread/)

Task 4
--------

Let's connect the sensor to the board. Disclaimer: You're about to be dealing with some low voltage electricity. If that makes you uncomfortable, don't continue.

The sensor has a VCC, GND, and DO pin.

- VCC is the voltage supply pin,
- GND is the ground pin, and
- DO is the digital out pin.

- Connect the **VCC** pin on the sensor, to the **VCC** pin of the board.
- Connect the **GND** pin on the sensor, to the **GND** pin of the board.
- Connect the **DO** pin on the sensor, to the **0** pin of the board.

Once the VCC and GND pins are connected, if your sensor has a status LED, it'll probably light up.

![task4-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task4-complete "Task 4 Complete")

Bonus points if you use a bread board.

Task 5
----------

We wired the sensor to the board, but we're not using it yet. Let's start reading the sensor value in our main loop. In my example, I connected the sensor to pin 0. Let's read pin 0.

Add the code for reading pin0 to the main loop. Once added, we should be able to manipulate the sensor, and see the value change. Update your main loop to read the pin0 value every 50ms. Then try to manipulate the sensor - what do you see?

![task5-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task5-complete.png "Task 5 Complete")

In my case, when I shake the vibration sensor, the pin reads 1. When I don't, it reads 0.

At this point, we have a board connected to a network, that is reading values of connected pins.

Reading Analog Sensor Pins
-----------------------

Let's quickly learn how to read an analog sensor. This is a sensor that can output a range of values. For example, a light sensor can detect a range (dark to bright).

When reading an analog pin, you'll need to use an analog pin on the board. These pins are prefixed with **A**. Let's take our sensor from before, and connect it to A1. Now, update your code to analogRead pin A1:

```c
  long analogPin0Value = analogRead(1);
  Serial.println(analogPin0Value);
```

[analogRead reference](https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/)

If your sensor is digital, you'll probably read a value near 0 (the min), or a value near 1023 (the max). These values are relative to the voltage across the sensor's output pin. If the sensor is analog, you should see a range of values, based on how you manipulate the sensor.
