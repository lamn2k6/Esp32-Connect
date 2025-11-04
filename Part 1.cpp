#define BLYNK_TEMPLATE_ID "TMPL6wyCye02G"
#define BLYNK_TEMPLATE_NAME "LedDHT11"
#define BLYNK_AUTH_TOKEN  "IBiiBYrILiAjb4Tn0JG-pnG_4hbXUpUC" 

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

DHT dht(30, DHT11);
BlynkTimer timer;

const char* ssid="TP-Link_2F80";
const char* pass="49828818";

BLYNK_WRITE(V1){
    int n=param.asInt();
    digitalWrite(2, n);
    Blynk.virtualWrite(V0, n);
}

void dht11(){
  double h=dht.readHumidity();
  double t=dht.readTemperature();
  Blynk.virtualWrite(V3, h);
  Blynk.virtualWrite(V2, t);
  if (t>35){
    Blynk.logEvent("high_temp", String("Nhiet do lon hon 35°C: ") + t);
  }
}


void setup() {
  pinMode(2, OUTPUT);
  dht.begin();
  WiFi.begin(ssid, pass);
  delay(2000);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  delay(2000);
  timer.setInterval(2000L, dht11);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(2000);
  }
   if (!Blynk.connected()) {
    Blynk.connect();
    delay(2000);
  }
  Blynk.run();
  timer.run();
}
