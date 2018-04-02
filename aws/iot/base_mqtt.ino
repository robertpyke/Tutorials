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
StaticJsonBuffer<2000> jsonBuffer;

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
    JsonObject& root = jsonBuffer.parseObject(buf);
    // Test if parsing succeeds.
    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    } else {
      Serial.println("parseObject() parsed it real good");
    }
    // TODO: Do something with root["state"]
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
