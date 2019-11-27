#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleTimer.h> //https://github.com/jfturcot/SimpleTimer
#include <ArduinoOTA.h> //https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
#include "config.h";
#include "DHT.h"

// Dependances Setup
//----------------------------------------------------------------------
WiFiClient wifiClient;
PubSubClient client(wifiClient);
SimpleTimer timer;
DHT dht;
bool boot = true;


// Door Bell
bool alreadyTriggered = false;
const int doorBellPin = 16;
const int playBellPin = 5;
//other pins
const int pirPIN = 12;

const int dhtPIN = 4;

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
  dht.setup(dhtPIN);


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
  timer.setInterval(100, postActiveSenors);
  timer.setInterval(5000, postPassiveSenors);
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
