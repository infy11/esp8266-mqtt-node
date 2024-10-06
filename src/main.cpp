#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "certificate.h"

#define CERT mqtt_broker_cert
#define MSG_BUFFER_SIZE (50)

//--------------------------------------
// config (edit here before compiling)
//--------------------------------------
//#define MQTT_TLS // uncomment this define to enable TLS transport
//#define MQTT_TLS_VERIFY // uncomment this define to enable broker certificate verification
const char* ssid = "Airtel_varshi";
const char* password = "Varshi@203";
const char* mqtt_server = "broker.hivemq.com"; // eg. your-demo.cedalo.cloud or 192.168.1.11
const uint16_t mqtt_server_port = 1883; // or 8883 most common for tls transport
const char* mqttUser = "user";
const char* mqttPassword = "pass";
const char* mqttTopicIn = "varnit-mqtt-in";
const char* mqttTopicOut = "varnit-mqtt-out";
int motor1Pin1 = 12; 
int motor1Pin2 = 14; 
int enable1Pin = 13; 
//--------------------------------------
// globals
//--------------------------------------
#ifdef MQTT_TLS
  WiFiClientSecure wifiClient;
#else
  WiFiClient wifiClient;
#endif
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
PubSubClient mqttClient(wifiClient);

//--------------------------------------
// function setup_wifi called once
//--------------------------------------
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();

#ifdef MQTT_TLS
  #ifdef MQTT_TLS_VERIFY
    X509List *cert = new X509List(CERT);
    wifiClient.setTrustAnchors(cert);
  #else
    wifiClient.setInsecure();
  #endif
#endif

  Serial.println("WiFi connected");
}

//--------------------------------------
// function callback called everytime 
// if a mqtt message arrives from the broker
//--------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: '");
    Serial.println("running motors");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH); 
   digitalWrite(LED_BUILTIN, LOW);
  delay(5000);
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW); 
   digitalWrite(LED_BUILTIN, HIGH);
  Serial.print(topic);
  Serial.print("' with payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String myCurrentTime = timeClient.getFormattedTime();
  //mqttClient.publish(mqttTopicOut,("ESP8266: Cedalo Mosquitto is awesome. ESP8266-Time: " + myCurrentTime).c_str());
}

//--------------------------------------
// function connect called to (re)connect
// to the broker
//--------------------------------------
void connect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    String mqttClientId = "";
    if (mqttClient.connect(mqttClientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("connected");
      mqttClient.subscribe(mqttTopicIn);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" will try again in 5 seconds");
      delay(5000);
    }
  }
}

//--------------------------------------
// main arduino setup fuction called once
//--------------------------------------
void setup() {
  Serial.begin(115200);
  setup_wifi();
  mqttClient.setServer(mqtt_server, mqtt_server_port);
  mqttClient.setCallback(callback);
  Serial.println("settting up motor pins");
   pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

//--------------------------------------
// main arduino loop fuction called periodically
//--------------------------------------
void loop() {
  if (!mqttClient.connected()) {
    connect();
  }
 
  mqttClient.loop();
  timeClient.update();
}
