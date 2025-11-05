#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

DHT dht(30, DHT11);
WebServer server(80);

const char* ssid="TP-Link_2F80";
const char* pass="49828818";

bool n=false;
String html(){
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  String html=
"<html>"
"<head>"
    "<meta charset='UTF-8'>"
    "<title>ESP_HTML</title>"
    "<style>"
        "body {"
            "font-family: 'Times New Roman', Times, serif;"
            "text-align: center;"
            "margin-top: auto;"
        "}"
        "button {"
            "padding: 10px 20px;"
            "font-size: 16px;"
        "}"
        ".info-box{"
            "text-align: left;"
        "}"
    "</style>"
"</head>"
"<body>"
    "<h1>Mu vô địch</h1>"
    "<img src='https://upload.wikimedia.org/wikipedia/en/thumb/7/7a/Manchester_United_FC_crest.svg/800px-Manchester_United_FC_crest.svg.png' width='100'>"
    "<div class='info-box'>"
    "<h2>"
      "Nhiệt độ: "+ String(t)+ "°C<br>"
      "Độ ẩm: "+ String(h)+"%<br>"
      "LED: <span id='status'>" + String(n ? "Siuu ON" : "Siuu OFF") +"</span>"
    "</h2>"
    "</div>"
    "<button onclick=\"fetch('/api/led?state='+("+String(!n)+"),{method:'POST'}).then(()=>location.reload())\">ON/OFF Siuu</button>"
"</body>"
"</html>";
return html;
}

void beginHTTP(){
  server.send(200, "text/html", html());
}

//Get
void API_sensor(){
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  String json="{\"Nhiệt độ\": "+ String(t) + ",\"Độ ẩm\":" + String(h)+",\"led\":" + String(n ? 1:0)+"}";
  server.send(200, "application/json", json);
}

//Post
void API_SWITCH(){
  int state = server.arg("state").toInt();
  if (state==0){
    digitalWrite(2, LOW);
  }else if(state==1){
    digitalWrite(2, HIGH);
  }

  n = (state == 1);
  String json = "{\"led\": " + String(n ? 1 : 0) + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  dht.begin();
  WiFi.begin(ssid, pass);
  delay(5000);
  Serial.println(WiFi.localIP());

  server.on("/", beginHTTP);
  server.on("/api/sensor", HTTP_GET, API_sensor);
  server.on("/api/led", HTTP_ANY, API_SWITCH);

  server.begin();

}

void loop() {
if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(2000);
  }
server.handleClient();
}
