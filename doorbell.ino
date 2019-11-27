void getDoorBell()
{
  if(digitalRead(doorBellPin) == 1 && alreadyTriggered == false)
  {
    client.publish(MQTT_DOORBELL_STATE_TOPIC, "Ding");
    alreadyTriggered = true;
    timer.setTimeout(6000, resetDoorBellTriggered); // wait 6 second before triggering
  }
}
void resetDoorBellTriggered()
{
  alreadyTriggered = false;
  client.publish(MQTT_DOORBELL_STATE_TOPIC, "OFF");
}



void playBell() {
  client.publish(MQTT_DOORBELL_STATE_TOPIC,"Play Doorbell", true);
  alreadyTriggered = true;
  timer.setTimeout(6000, resetDoorBellTriggered); // wait 6 second before triggering
  int d = 200;
  digitalWrite(playBellPin, HIGH);
  delay(d);
  digitalWrite(playBellPin, LOW);
  delay(d);

  digitalWrite(playBellPin, HIGH);
  delay(d);
  digitalWrite(playBellPin, LOW);
  delay(d);

  digitalWrite(playBellPin, HIGH);
  delay(d);
  digitalWrite(playBellPin, LOW);
  delay(d);
}


void changeVol() {
  alreadyTriggered = true;
  timer.setTimeout(6000, resetDoorBellTriggered); // wait 6 second before triggering
  int d = 200;
  digitalWrite(playBellPin, HIGH);
  delay(d);
  digitalWrite(playBellPin, LOW);
  delay(d);
}