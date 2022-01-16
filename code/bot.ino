#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <LiquidCrystal.h>
#include <ESP32Servo.h>
#define SOUND_SPEED 0.034
#define echoPin 35 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 32 //attach pin D3 Arduino to pin Trig of HC-SR04
#include <QueueList.h>
int LED = 02;
long duration; // variable for the duration of sound wave travel
int distance;
Servo myservo;
LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

int servoPin = 33;
QueueList <String> queue;
const char* mqtt_server = "broker.mqttdashboard.com"; //mqtt server
const char* ssid = "m";
const char* password = "12344321";

WiFiClient espClient;
PubSubClient client(espClient); //lib required for mqtt
void setup() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400);
  lcd.begin(16, 2);
  lcd.print("A hello form!");
  lcd.setCursor(0, 1);
  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  WiFi.begin(ssid, password);
  Serial.println("connected");
  client.setServer(mqtt_server, 1883);//connecting to mqtt server
  client.setCallback(callback);
  //delay(5000);
  connectmqtt();
  myservo.write(0);
}

void callback(char* topic, byte* payload, unsigned int length) {   //callback includes topic and payload ( from which (topic) the payload is comming)
   char tempPayload[15] = "";
    if (length > 15){
      length = 15;
    }
    for (int i = 0; i < length; i++) {
      tempPayload[i] = (char)payload[i];
  }
  if (length == 15){
    tempPayload[15] = '\0';
  }
  queue.push(String(tempPayload));
  lcd.clear();
  lcd.setCursor(0,0);             // Start at top-left corner
  lcd.print("A hello from");
  lcd.print(" ");
  lcd.print(queue.count());
  lcd.setCursor(0,1);
  lcd.print(String(tempPayload));
  char msg[50];
  strcpy(msg,"Hi5 Robot: Got a Hi5 from ");
  strcat(msg,tempPayload);
  myservo.write(70);
  delay(100);
  myservo.write(100);
  delay(100);
  myservo.write(70);
}



void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("testsai")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopicSai", "Nodemcu connected to MQTT");
      // ... and resubscribe
      client.subscribe("inTopicSai");

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
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
   duration = pulseIn(echoPin, HIGH);
   distance = duration * 0.034 / 2;
  if(distance < 10){
    if (!queue.isEmpty()){
      lcdCollectingHi5();;
      myservo.write(20);
      delay(500);
      myservo.write(100);
      delay(1000);
     }
     else{
      lcd.clear();
      lcd.print("A hello from!");
     }
    
  }
  if (queue.isEmpty()){
    myservo.write(120);
  }
 
  delay(500);
  }


void connectmqtt()
{
  client.connect("testSai");  // ESP will connect to mqtt broker with clientID
  {
    Serial.println("connected to MQTT");
    // Once connected, publish an announcement...

    // ... and resubscribe
    client.subscribe("inTopicSai"); //topic=Demo
    client.publish("outTopicSai",  "connected to MQTT");

    if (!client.connected())
    {
      reconnect();
    }
  }
}
void lcdCollectingHi5(){
  lcd.clear();
  lcd.setCursor(0,0);       
  lcd.print("A hello from ");
  lcd.print(queue.count()-1);
  lcd.setCursor(0,1);   
  lcd.print(queue.pop());
}
