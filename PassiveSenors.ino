
void postPassiveSenors ()
{
  float humidity =  0;
  humidity = dht.getHumidity();                                 // Get humidity value
  float temperature = dht.getTemperature();                     // Get temperature value
  StaticJsonDocument<200> doc;

  if (dht.getStatusString() == "OK") {
    doc["temperature"] = (String)temperature;
    doc["humidity"] = (String)humidity;
    doc["lightlevel"] = (String)getLightLevel();
  } else {
    doc["dhterror"] = dht.getStatusString();
    doc["lightlevel"] = (String)getLightLevel();
  }
  char data[200];
  serializeJson(doc, data);
  client.publish(MQTT_SENSOR_TOPIC, data, true);
  yield();
  
}


// Get light level
float getLightLevel() {
  int sensorValue = analogRead(A0);                               // read the input on analog pin 0
  float lightLevel = (sensorValue * (3.3 / 1023.0))/3.000 *100;   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V) - Then to percentages
  return lightLevel;
}