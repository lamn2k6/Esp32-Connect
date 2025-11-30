#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <WiFiClientSecure.h>

const char* ssid= "TP-Link_2F80";
const char* pass= "49828818";

DHT dht(27, DHT11);
WiFiClientSecure espSecure;
PubSubClient client(espSecure);


void Pub(){
  float h=dht.readHumidity();
  float t= dht.readTemperature();
  unsigned long ts=millis();

  StaticJsonDocument<128> doc;
  doc["temp"]=t;
  doc["humi"]=h;
  doc["ts"]=ts;

  char json[128];
  serializeJson(doc, json);
  client.publish("vju/4/sensor", json);

  if (digitalRead(2)==HIGH){
    client.publish("vju/4/state", "ON");
  }else{
    client.publish("vju/4/state", "OFF");
  }
delay(2000);
}

void Sub(char* topic, byte* payload, unsigned int length){
  String n;
  for (int i=0; i< length; i++){
    n+=(char)payload[i];
  }
  n.trim();
  Serial.println(n);
  
  if (String(topic)== "vju/4/cmd/led"){
  if (n=="1"){
    digitalWrite(2, HIGH);
  }else{
    digitalWrite(2, LOW);
  }}
  Pub();
}

void connect_and_last_will(){
  client.connect("ESP32-4", "lamn2k6", "12345678.Aa", "vju/4/lastwill", 1, true, "offline");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  WiFi.begin(ssid, pass);
  delay(5000);

  espSecure.setInsecure();
  client.setServer("b22faecbf48544dcaf34b3dcebfa352b.s1.eu.hivemq.cloud", 8883);
  delay(5000);
  client.setCallback(Sub);



}
void loop() {
  if(!client.connected()){
    connect_and_last_will();
    delay(5000);
    client.subscribe("vju/4/cmd/led");
  }
  if(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, pass);
    delay(5000);
    connect_and_last_will();
  }
  client.loop();
  Pub();
  delay(1000);
}
