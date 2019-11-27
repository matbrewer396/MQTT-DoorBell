void checkIn()
{
  client.publish(MQTT_CHECKIN_TOPIC,"OK");
}

void reconnect() 
{
  // Loop until we're reconnected
  int retries = 0;
  while (!client.connected()) {
    if(retries < 15)
    {
        Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect(DEVICENAME, MQTT_USER, MQTT_PASSWORD, MQTT_DOORBELL_AVAIL, 0,false,"offline")) {
        Serial.println("INFO: MQTT connected");
        client.publish(MQTT_DOORBELL_AVAIL,"online");
        // Once connected, publish an announcement...
        if(boot == true) {
          client.publish(MQTT_CHECKIN_TOPIC,"Rebooted");
          boot = false;
        }
        else if(boot == false) {
          client.publish(MQTT_CHECKIN_TOPIC,"Reconnected"); 
        }
        // resubscribe to topic
        client.subscribe(MQTT_DOORBELL_COMMAND_TOPIC);
        // client.subscribe(MQTT_LIGHT_COMMAND_TOPIC);

        digitalWrite(LED_BUILTIN, HIGH);
      } 
      else {
        Serial.print("ERROR: MQTT failed, rc=");
        Serial.print(client.state());
        Serial.println("DEBUG: try again in 5 seconds");
        retries++;
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
    if(retries > 14) {
      ESP.restart();
    }
  }
}


// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  String newTopic = p_topic;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  Serial.print(newTopic);
  Serial.print(payload);
  if (newTopic == MQTT_DOORBELL_COMMAND_TOPIC) 
  {

    if (payload == "Play Doorbell")
    {
      playBell();
      client.publish(MQTT_DOORBELL_STATE_TOPIC,"Play Doorbell", true);
    } else if (payload == "Change Volume")
    {
      changeVol();
      client.publish(MQTT_DOORBELL_STATE_TOPIC,"Change Volume", true);
    }
  }
}
