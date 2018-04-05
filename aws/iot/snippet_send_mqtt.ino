char publishPayload[MQTT_MAX_PACKET_SIZE];
char publishTopic[]   = "$aws/things/" THING_NAME "/shadow/update";

void setup() {
// Assuming client is an mqtt client.
sprintf(publishPayload, "{\"state\":{\"reported\":{\"led\":%d }},\"clientToken\":\"%s\"}",
          digitalRead(LED_BUILTIN),
          clientId
         );
  client.publish(publishTopic, publishPayload);
}
