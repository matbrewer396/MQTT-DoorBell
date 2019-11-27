int PIR;

int postBoolStatus(int oldstatus, int newStatus, char* topic)
{ 
  if( (newStatus != oldstatus || oldstatus == -1) && newStatus == 0)
  {
    client.publish(topic,"true", true);    
    return newStatus;
  }
  else if((newStatus != oldstatus || oldstatus == -1) && newStatus == 1)
  {
    client.publish(topic,"false", true);
    return newStatus;
  }else {
    return oldstatus;
  };
  return newStatus;
  
}


void postActiveSenors() {
  PIR = postBoolStatus(PIR,digitalRead(pirPIN),MQTT_PIR_TOPIC);
  getDoorBell();
}