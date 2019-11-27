#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleTimer.h> //https://github.com/jfturcot/SimpleTimer
#include <ArduinoOTA.h> //https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
#include "config.h";

// Dependances Setup
//----------------------------------------------------------------------
WiFiClient wifiClient;
PubSubClient client(wifiClient);
SimpleTimer timer;
bool boot = true;


// Door Bell
bool alreadyTriggered = false;
const int doorBellPin = 16;
const int playBellPin = 5;
//other pins
const int pirPIN = 12;
int PIR;



void reconnect() 
{
  // Loop until we're reconnected
  int retries = 0;
  while (!client.connected()) {
    if(retries < 15)
    {
        Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect(DEVICENAME, MQTT_USER, MQTT_PASSWORD)) {
        Serial.println("INFO: MQTT connected");
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

        digitalWrite(LED_BUILTIN, LOW);
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


//   // handle message topic
//   if (String(MQTT_LIGHT_COMMAND_TOPIC).equals(p_topic)) {
  
//   } 
}

void checkIn()
{
  client.publish(MQTT_CHECKIN_TOPIC,"OK");
}






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
}


void playBell() {
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


void readRealTimeSenors() {
  PIR = postBoolStatus(PIR,digitalRead(pirPIN),MQTT_PIR_TOPIC);
  getDoorBell();
}



void setup() {
   // init the serial
  Serial.begin(115200);

  ArduinoOTA.setHostname(DEVICENAME);
  ArduinoOTA.begin(); 

  // pin
  pinMode(doorBellPin, INPUT_PULLDOWN_16);
  pinMode(playBellPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);


  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  WiFi.hostname(DEVICENAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.print("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection

  client.setCallback(callback);
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);

  // Confirm status every two minutes
  timer.setInterval(CHECKIN_INTERVAL, checkIn);
  timer.setInterval(100, readRealTimeSenors);
}

void loop() {
  if (!client.connected()) {
    digitalWrite(LED_BUILTIN, HIGH);
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
  timer.run();

}
