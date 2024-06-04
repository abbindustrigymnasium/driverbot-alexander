#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

#include "MQTTConnector.h"
#include "Credentials.h"

#define motorPinRightDir  0//D2
#define motorPinRightSpeed 5//D1
Servo servo;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

boolean mqttInitCompleted = false;
String clientId = "IoTPractice-" + String(ESP.getChipId());

 int speed = 1024;
  float l = 0;
  float r = 180;
  float dir = 1;
/* Incoming data callback. */
void dataCallback(char* topic, byte* payload, unsigned int length)
{
  char payloadStr[length + 1];
  memset(payloadStr, 0, length + 1);
  strncpy(payloadStr, (char*)payload, length);
  Serial.printf("Data    : dataCallback. Topic : [%s]\n", topic);
  Serial.printf("Data    : dataCallback. Payload : %s\n", payloadStr);
  pinMode(motorPinRightDir, OUTPUT);
  pinMode(motorPinRightSpeed, OUTPUT);
  servo.attach(D4); // D4
  servo.write(90);
 

  if(strcmp(payloadStr, "Gear1") == 0)
    {
      speed = 64;
      l = 0;
      r = 180;
      dir = 1;
      Serial.print("Porsche");
      Serial.print(speed);
    }

  if(strcmp(payloadStr, "Gear2") == 0)
    {
      speed = 128;
      l = 20;
      r = 160;
      dir = 1;
    }

  if(strcmp(payloadStr, "Gear3") == 0)
    {
      speed = 192;
      l = 40;
      r = 140;
      dir = 1;
    }

  if(strcmp(payloadStr, "Gear4") == 0)
    {
      speed = 256;
      l = 60;
      r = 120;
      dir = 1;
      Serial.print(speed);
    }

  if(strcmp(payloadStr, "GearR") == 0)
    {
      dir = 0;
    }
  
  if(strcmp(payloadStr, "Forward") == 0)
  {

   
    digitalWrite(motorPinRightDir, dir);
    analogWrite(motorPinRightSpeed, speed);
    Serial.print("BMW");
    Serial.print(speed);
  }
  if(strcmp(payloadStr, "NoForward") == 0)
  {
   
    int dir = 1;
 
    digitalWrite(motorPinRightDir, dir);
    analogWrite(motorPinRightSpeed, 0);
    Serial.print("CrashOut");
  }
  if(strcmp(payloadStr, "Backwards") == 0)
  {
    
    int dir = 0;

   
    digitalWrite(motorPinRightDir, dir);
    analogWrite(motorPinRightSpeed, speed);
    Serial.print("Audi");
  } 
  if(strcmp(payloadStr, "NoBackward") == 0)
  {
    
    int dir = 0;

   
    digitalWrite(motorPinRightDir, dir);
    analogWrite(motorPinRightSpeed, 0);
    Serial.print("Volvo");
  }
  if(strcmp(payloadStr, "SwerveLeft") == 0)
  {
    servo.write(l);
  }
  if(strcmp(payloadStr, "NoSwerveLeft") == 0)
  {
    servo.write(90);
  }
  if(strcmp(payloadStr, "SwerveRight") == 0)
  {
    servo.write(r);
    Serial.print("Aston Martin");
  }
  if(strcmp(payloadStr, "NoSwerveRight") == 0)
  {
    servo.write(90);
    Serial.print("Alfa romeo");
  }
  if(strcmp(payloadStr, "uTurn") == 0)
  {
    servo.write(180);
    delay(200);
    digitalWrite(motorPinRightDir, 1);
    analogWrite(motorPinRightSpeed, 256);
    delay(200);
    digitalWrite(motorPinRightDir, 1);
    analogWrite(motorPinRightSpeed, 0);
    delay(200);
    servo.write(90);
    delay(200);
    digitalWrite(motorPinRightDir, 1);
    analogWrite(motorPinRightSpeed, 256);
    delay(200);
    digitalWrite(motorPinRightDir, 1);
    analogWrite(motorPinRightSpeed, 0);
    delay(200);
    servo.write(180);
    delay(200);
    digitalWrite(motorPinRightDir, 1);
    analogWrite(motorPinRightSpeed, 256);
    delay(200);
    digitalWrite(motorPinRightDir, 1);
    analogWrite(motorPinRightSpeed, 0);
  }
  

  
}

void performConnect()
{
  uint16_t connectionDelay = 5000;
  while (!mqttClient.connected())
  {
    Serial.printf("Trace   : Attempting MQTT connection...\n");
    if (mqttClient.connect(clientId.c_str(), MQTT_USERNAME, MQTT_KEY))
    {
      Serial.printf("Trace   : Connected to Broker.\n");

      /* Subscription to your topic after connection was succeeded.*/
      MQTTSubscribe(TOPIC);
    }
    else
    {
      Serial.printf("Error!  : MQTT Connect failed, rc = %d\n", mqttClient.state());
      Serial.printf("Trace   : Trying again in %d msec.\n", connectionDelay);
      delay(connectionDelay);
    }
  }
}

boolean MQTTPublish(const char* topic, char* payload)
{
  boolean retval = false;
  if (mqttClient.connected())
  {
    retval = mqttClient.publish(topic, payload);
  }
  return retval;
}

boolean MQTTSubscribe(const char* topicToSubscribe)
{
  boolean retval = false;
  if (mqttClient.connected())
  {
    retval = mqttClient.subscribe(topicToSubscribe);
  }
  return retval;
}

boolean MQTTIsConnected()
{
  return mqttClient.connected();
}

void MQTTBegin()
{
  mqttClient.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  mqttClient.setCallback(dataCallback);
  mqttInitCompleted = true;
}

void MQTTLoop()
{
  if(mqttInitCompleted)
  {
    if (!MQTTIsConnected())
    {
      performConnect();
    }
    mqttClient.loop();
  }
}
