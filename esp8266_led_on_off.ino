#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "Radisson_Guest";
const char* password = "";

ESP8266WebServer server(80);

const int led = 2;

void handleRoot() {
  server.send(200, "text/html", "<html><head><meta name=\"viewport\" content=\"width=device-width\"><meta name=\"viewport\" content=\"initial-scale=1.0\"><title>LED on/off via ESP8266</title></head><body><a href=\"/on\"><h2>On</h2></a><br/><a href=\"/off\"><h2>Off</h2></a></body></html>");
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/on", [](){
    digitalWrite(led, HIGH);
    server.sendHeader("Location", String("/"), true);
    server.send (302, "text/plain", "");
  });
  server.on("/off", [](){
    digitalWrite(led, LOW);
    server.sendHeader("Location", String("/"), true);
    server.send (302, "text/plain", "");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
