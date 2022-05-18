/*
  Adaptado por jefferson henrique.
  
	<ArduinoWebsockets.h> By Gil Maimon	https://github.com/gilmaimon/ArduinoWebsockets
*/

#include <ArduinoWebsockets.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

ESP8266WiFiMulti wifiMulti;

const char* ssid = "brisa-1394585"; //Enter SSID
const char* password = "t5tb7ffx"; //Enter Password
const char* websockets_server_host = "safe-sea-86700.herokuapp.com"; //Enter server adress
const uint16_t websockets_server_port = 80; // Enter server port
bool conexao = false;
using namespace websockets;
#define LED 2

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    if(message.data() == "OFF")digitalWrite(LED, HIGH);
    if(message.data() == "ON")digitalWrite(LED, LOW);
    Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Websocket Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        conexao = false;
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

WebsocketsClient client;
void setup() {
    pinMode(LED, OUTPUT);
    
    Serial.begin(115200);
    Serial.print("Serial.begin");
    delay(500);

  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);
  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);
  // Register multi WiFi networks
  wifiMulti.addAP("brisa-", "t5tb7ffx");
  wifiMulti.addAP("BOKUS 01", "424F4B5553");
  wifiMulti.addAP("BOKUS 02", "424F4B5553");
  wifiMulti.addAP("BOKUS 03", "424F4B5553");
  wifiMulti.addAP("BOKUS 04", "424F4B5553");
  wifiMulti.addAP("BOKUS 05", "424F4B5553");
  // More is possible
  while(wifiMulti.run(3000) != WL_CONNECTED) {
    Serial.println("WiFi Disconnected");
    delay(1500);
  }
  Serial.print("WiFi connected  >> \"");
  Serial.print(WiFi.SSID());
  Serial.print("\" ");
  Serial.println(WiFi.localIP());


    // run callback when messages are received
    client.onMessage(onMessageCallback);
    // run callback when events are occuring
    client.onEvent(onEventsCallback);

    //client.send("Hello Server");


 ArduinoOTA.setHostname(" OTA  ");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();

}


void loop() {
    client.poll();
    ArduinoOTA.handle();
    while(!conexao){conexao = client.connect(websockets_server_host, websockets_server_port, "/");  Serial.println("Tentando reconexão com websocket");  delay(1000); }
    while(wifiMulti.run(3000) != WL_CONNECTED) { Serial.print("WiFi Disconnected");  delay(1500);}
}
