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

When reading an analog pin, you'll need to use an analog pin on the board. These pins are prefixed with **A**. The following code shows how to read an analog pin (A1):

```c
  long analogPin1Value = analogRead(1);
  Serial.println(analogPin1Value);
```

[analogRead reference](https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/)

If your sensor is digital, you'll probably read a value near 0 (the min), or a value near 1023 (the max). These values are relative to the voltage across the sensor's output pin. If the sensor is analog, you should see a range of values, based on how you manipulate the sensor.

Task 6
--------------------

Let's take our sensor from before, and connect it to A1. Now, update your code to read analog pin 1. Try to manipulate your sensor. What do you see?

![task6-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task6-complete.png "Task 6 Complete")

In my case, my sensor is a digital sensor. It's either on, or off. For that reason, I see nearly 1023 (full voltage) when I shake the vibration sesnor, and ~20 (nearly no voltage) when it's idle. Why isn't it exactly 0 or 1023? This is a physical circuit, the real world requires tolerance (*get it, tolerance, the allowable error*).

Task 7
----------------------

Before we proceed, return the code to a digital read (on pin 0). I'm going to expect you to be using pin 0, with a digital sensor, later in this lab.

Writing Pins
============

You're doing great, you've read a sensor. What can you do with that? Great question.. thanks for asking.. let's take that sensor value, and turn our built-in LED on/off based on its value.

To turn on the LED, we just need to set its pin high (1). We can turn it off by setting it low (0).

```c
void setup() {
  ...
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  ...
}

void loop() {
  ...
  digitalWrite(LED_BUILTIN, ledValue);
}
```  

In the above, we do two things: 

1. In the setup, we set the LED pin as an output, i.e. we're going to write to it.
2. In the main loop, we write to the pin, whatever ledValue is.

Task 7
---------

You already know how to perform a digital read, and now you know how to write to the LED. Rather than logging the sensor value when you read it, let's set the LED to on/off, based on the sensor value. Consider using a shorter delay for this loop (now you're not printing) - 10ms should be good.

At this point, you should be reading a sensor value, and turning on/off the LED based on that sensor value.

AWS IOT Setup
==========================

You have an internet connected device, and you're in a tutorial folder called aws/iot... hopefully you saw this coming... it's time to start sharing to the world the value of your inner most pins (using AWS IOT).

We're going to first get your device an established MQTT client connection to AWS IOT. MQTT is a lightweight protocol for sending data. In this case, we'll use MQTT to send messages to AWS IOT, and to read messages from AWS IOT.

Let's replace your main .ino file with the following:

```c
#include "arduino_secrets.h"
#include <SPI.h>
#include <WiFi101.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;  // the WiFi radio's status

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Also have to actually override this in the PubSubClient.h
#define MQTT_MAX_PACKET_SIZE 4096

char mqttServer[]     = MQTT_IP;
char clientId[]       = THING_NAME;
char publishTopic[]   = "$aws/things/" THING_NAME "/shadow/update";
char publishPayload[MQTT_MAX_PACKET_SIZE];
char *subscribeTopic[5] = {
  "$aws/things/" THING_NAME "/shadow/update/accepted",
  "$aws/things/" THING_NAME "/shadow/update/rejected",
  "$aws/things/" THING_NAME "/shadow/update/delta",
  "$aws/things/" THING_NAME "/shadow/get/accepted",
  "$aws/things/" THING_NAME "/shadow/get/rejected"
};


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

  // MQTT connection
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

/**
   This method is called whenever we receive a message on our subscribed MQTT topics.
*/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("MQTT Callback");
  char buf[length];

  strncpy(buf, (const char *)payload, length);
  buf[length] = '\0'; // Ensure null termination.

  Serial.println(topic);
  Serial.println(buf);

  if (String(topic) == "$aws/things/" THING_NAME "/shadow/update/delta") {
    StaticJsonBuffer<MQTT_MAX_PACKET_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(buf);
    // Test if parsing succeeds.
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    } else {
      Serial.println("parseObject() parsed it real good");
    }
    // Do something with root["state"]
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");

      for (int i = 0; i < 5; i++) {
        Serial.print("Subscribing to: ");
        Serial.print(subscribeTopic[i]);
        if (client.subscribe(subscribeTopic[i]), 1) {
          Serial.println(" ... succeeded");
        } else {
          Serial.println(" ... failed");
        }
      }

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  delay(500);
  Serial.println("looping..");
  // Loop our client (check for outstanding messages, etc.)
  client.loop();
}

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
You'll also need to update the following values in your arduino_secrets.h file:

```c
#define AWS_EP "XXXXXXXXXX.iot.us-east-1.amazonaws.com"
#define MQTT_IP "XX.XXX.XXX.XXX"
#define THING_NAME "THING_X"
```

If you're lucky enough to be in a physical lab with me, doing this, you should get the real values we'll be using. I've already set up some specific things for you.

I'm going to briefly go over the above code:
```c
  PubSubClient client(wifiClient);
```
<code>client</code> is our MQTT client. It is a [PubSubClient](https://pubsubclient.knolleary.net/api.html). It uses our existing wifi client.

```c
  char publishTopic[]   = "$aws/things/" THING_NAME "/shadow/update";
```
<code>publishTopic</code> is the MQTT topic we will send device updates to. This will be where our IOT device reports its real status to (which updates the device shadow).

```c
char *subscribeTopic[5] = {
  "$aws/things/" THING_NAME "/shadow/update/accepted",
  "$aws/things/" THING_NAME "/shadow/update/rejected",
  "$aws/things/" THING_NAME "/shadow/update/delta",
  "$aws/things/" THING_NAME "/shadow/get/accepted",
  "$aws/things/" THING_NAME "/shadow/get/rejected"
};
```
These are topics we'll listen on. Each topic has a different use case. In a real application, you would only listen to topics you intended to act on.

We're going to ignore all of these for the moment, except the <code>update/delta</code>. The delta topic tells us the difference between the desired state of a device, and the reported (actual) state. Let's say you set the desired state for the LED to on, but the reported LED state is off. A delta will be sent to the device. If the device's reported and the desired state are in alignment, no delta message will be sent.

```c
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");

      for (int i = 0; i < 5; i++) {
        Serial.print("Subscribing to: ");
        Serial.print(subscribeTopic[i]);
        if (client.subscribe(subscribeTopic[i]), 1) {
          Serial.println(" ... succeeded");
        } else {
          Serial.println(" ... failed");
        }
      }

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  ...
}
```

This <code>reconnect</code> function manages connecting to our MQTT endpoint, and subscribing to the MQTT topics. If you lose network connection, the reconnect function will get you back onto the MQTT topics.

```c
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("MQTT Callback");
  char buf[length];

  strncpy(buf, (const char *)payload, length);
  buf[length] = '\0'; // Ensure null termination.

  Serial.println(topic);
  Serial.println(buf);

  if (String(topic) == "$aws/things/" THING_NAME "/shadow/update/delta") {
    StaticJsonBuffer<MQTT_MAX_PACKET_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(buf);
    // Test if parsing succeeds.
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    } else {
      Serial.println("parseObject() parsed it real good");
    }
    // Do something with root["state"]
  }
}
```

Okay, this looks a little more hairy. It's actually just a basic callback. Whenever the MQTT topics we're subscribed to receives a message, our callback runs, and deserializes the message. The <code>root</code> is a parsed JSON object root. We'll expand upon this callback later.

Task 8
---------

Update the code above with your real values, and then run it. You should get the following:

![task8-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task8-complete.png "Task 8 Complete")

Task 9
--------

Now that you've got the device connected, and it's looping, I want you to update the shadow state of the device (via the AWS console).
Change the desired state for a pin. For example:

```json
{
  "desired": {
    "pin0": 1
  },
  "reported": {
    "welcome": "aws-iot"
  }
 }
```

![task9-progress-1](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task9-progress-1.png "Task 9 In-Progress 1")

As you hit save, keep an eye on the serial monitor for your device. Assuming you're updating the device you're subscribed to, you should see a couple of messages:

![task9-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task9-complete.png "Task 9 Complete")

At this point, your device is now connected to AWS IOT, and you're able to receive the updated desired state for your device.

Respond to Delta
==================

You received a message telling you that your pin has a different desired value, to what you reported.

First, we'll just get the value out of the JSON, and make sure we're parsing everything correctly:
```c
    ...
    // Check that the JSON object has everything we want/need
    if (root["state"].success() && root["state"]["pin0"].success() && root["state"]["pin0"].is<int>()) {
      // Pull out pin0 as an int.
      int pin0Value = root["state"]["pin0"].as<int>();
      Serial.print("Pin 0, desired: ");
      Serial.println(pin0Value);
    }
```

Task 10
---------

Let's update our code to read the desired pin0 value. Assuming you leveraged the code above, you should be seeing the following output when you update the shadow:
```
MQTT Callback
$aws/things/THING_1/shadow/update/delta
{"version":3632,"timestamp":1522998303,"state":{"pin0":1},"metadata":{"pin0":{"timestamp":1522998303}}}
parseObject() parsed it real good
Pin 0, desired: 1
```

Task 11
-----------

Let's use pin0 (when talking with AWS), as a proxy for the LED. If you have a real output device (e.g. a buzzer), you can use that instead.

In our setup, update your code to use the LED as an output again.

```c
void setup() {
  ...
  
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  ...
}
```

Then update your callback, so that when you receive a new desired state for pin0, you update the LED to match the value:
```c
    ...
    // Check that the JSON object has everything we want/need
    if (root["state"].success() && root["state"]["pin0"].success() && root["state"]["pin0"].is<int>()) {
      // Pull out pin0 as an int.
      int pin0Value = root["state"]["pin0"].as<int>();
      Serial.print("Pin 0, desired: ");
      Serial.println(pin0Value);
      digitalWrite(LED_BUILTIN, pin0Value);
      Serial.print("LED set to: ");
      Serial.println(pin0Value); 
    }
```

Run the code, and then update your desired value for pin0 between 0 and 1. You should see the LED turn on/off.

![task11-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task11-complete.png "Task 11 Complete")

Update Reported
================

At this point we are receiving updates for our device, and we're internally responding to those updates, but we're not telling IOT our actual state. Let's create a periodic update function. We'll send some basic information ahout the device every so many loops. Again, I'm going to pretend my LED is pin0. So I'll report the LED state as pin0. If you have a real device connected to pin0, you don't need to pretend, you can just read/write pin0.

First, let's look at how to publish an MQTT message:

```c
  sprintf(publishPayload, "{\"state\":{\"reported\":{\"pin0\":%d, \"rssi\":%d }},\"clientToken\":\"%s\"}",
    digitalRead(LED_BUILTIN),
    WiFi.RSSI(),
    clientId
  );
  client.publish(publishTopic, publishPayload);
```

In the above, we are formatting a json message to send to the MQTT <code>publishTopic</code>.

Task 12
--------------

Update your code to put the publish in your main loop. Don't publish every loop though, only publish one in every 10 (or so) loops. If you do it every loop, it'll break. Why? Because you'll be receiving an accept for every publish, and you'll fall behind in the MQTT receive (<code>client.loop()</code>). Don't stress it if you don't understand, just trust me, and publish on every 10 iterations.

*Cheat Code Entered* - **SPOILER ALERT**

```c
int count = 0;
void loop() {
  count++;
  if (!client.connected()) {
    reconnect();
  }
  delay(100);
  Serial.println("looping..");
  // Loop our client (check for outstanding messages, etc.)
  client.loop();

  if (count >= 10) {
    Serial.println("publish state..");
    sprintf(publishPayload, "{\"state\":{\"reported\":{\"pin0\":%d, \"rssi\":%d }},\"clientToken\":\"%s\"}",
      digitalRead(LED_BUILTIN),
      WiFi.RSSI(),
      clientId
    );
    client.publish(publishTopic, publishPayload);
    count = 0;
  }
}
```
![task12-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task12-complete.png "Task 12 Complete")

At this point, assuming you did something like the above, you'll be sending your device state to IOT, and you'll be receiving deltas whenever the shadow is updated so that it no longer matches the reported. Your LED should turn on/off as you set the pin0 value in the desired. The pin0 value should be updated in the reported section, along with the rssi value. At this point, we're only missing one key thing... we don't read the initial delta. If a delta is reported while you're not on MQTT, you won't see it. Let's fix that.

Feel free to throw a few extra sensors in, and report their value. The next bit can wait..

Get Desired in Connect/Reconnect
==================================

As noted above, we don't read the initial state when we connect. It's entirely possible for an IOT device to lose connection for a long time (let's say it's a solar device, and it shuts down overnight). Let's make it so our device asks for the shadow state when it first connects. We do this by publishing an empty message to our get topic - [device shadow mqtt docs](https://docs.aws.amazon.com/iot/latest/developerguide/device-shadow-mqtt.html#get-pub-sub-topic). AWS IOT will respond to the message with the current state of the device.

```c
Serial.println("refreshing state..");
client.publish("$aws/things/" THING_NAME "/shadow/get", "");
```

Task 13
----------

Using the above, trigger a state refresh whenever you need to reconnect your MQTT (including the first time connect).

You should see something like:

![task13-complete](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/task13-complete.png "Task 13 Complete")

Task 14
----------

Notice the refresh state doesn't come through on the same topic as the delta. Update your code to support processing the delta from the refresh.

Task 15
--------

Ask Alexa to interface with your shadow (if you're in the lab with me, mention you made it this far).

Task 16
---------

If you haven't already, throw some more sensors on (using pins 0, 1 and 7). See if you can report the state of multiple sensors at the same time.


Task 17
--------

At this point, you're done with the Lab - these are just some extensions if you're curious.

Add a IOT rule, and publish rssi to CloudWatch... The rules are managed through the "Act" interface in IOT. You can find an example on THING_1 (if you're in the lab with me).

![rule-creation-1](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/rule_creation_1.png "rule-creation-1")
![rule-creation-2](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/rule_creation_2.png "rule-creation-2")
![rule-metric](https://github.com/robertpyke/Tutorials/raw/master/aws/iot/pics/rule_metric.png "rule-metric")
