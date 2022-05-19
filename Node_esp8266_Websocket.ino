/*
  Adaptado por jefferson henrique.
  
	<ArduinoWebsockets.h> By Gil Maimon	https://github.com/gilmaimon/ArduinoWebsockets
*/

#include <ArduinoWebsockets.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define LED 2

bool conexao_ws   = false;
int  conexao_wifi = 0;  
bool conexao_mdns = false;
bool obrig_ws     = false;

char html_page[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="pt-br">
    <head>
    </head>
    <body>
    <h1> Hello word </h1>
    </body>
</html>
 )=====";


const char* id = "brisa-1394585"; //Enter SSID
const char* pass = "t5tb7ffx"; //Enter Password

const char* ssid = "ESP8266"; //Enter SSID
const char* password = "10203040"; //Enter Password

const char* websockets_server_host = "safe-sea-86700.herokuapp.com"; //Enter server adress
const uint16_t websockets_server_port = 80; // Enter server port


using namespace websockets;

WebsocketsClient client;
ESP8266WebServer server(80);


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
        conexao_ws = false;
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}




void setup() {
    pinMode(LED, OUTPUT);
    
    Serial.begin(115200);
    Serial.println("Serial.begin");
    delay(500);

  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);
  // Set WiFi to station mode
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);

/*  // More is possible
  Serial.print("WiFi connected  >> \"");
  Serial.print(WiFi.SSID());
  Serial.print("\" ");
  Serial.println(WiFi.localIP());

  */
  server.on("/", []() {
    server.send(200, "text/html", html_page);
  });
  server.begin();





  // run callback when messages are received
  client.onMessage(onMessageCallback);
  // run callback when events are occuring
  client.onEvent(onEventsCallback);
  //client.send("Hello Server");





ArduinoOTA.setHostname("ESP_PRINCIPAL");
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
  
  


  int currentMillis=0, ledMillis=0, printMillis=0;
  conexao_wifi = WiFi.begin(id,pass);
  while(conexao_wifi != WL_CONNECTED  ||  !conexao_mdns  ||  ( !conexao_ws && obrig_ws ) ) {
          currentMillis = millis();
          
          server.handleClient();

          if(currentMillis > ledMillis+50){digitalWrite(LED, !digitalRead(LED));  ledMillis = currentMillis;}

          if(conexao_wifi != WL_CONNECTED)conexao_wifi = WiFi.status();
          if(!conexao_ws)conexao_ws = client.connect(websockets_server_host, websockets_server_port, "/");
          if(!conexao_mdns)conexao_mdns = MDNS.begin("principal");

          if(currentMillis > printMillis+1000){
              Serial.println("WiFi");
              Serial.println("conexao wifi: "+String(conexao_wifi));
              Serial.println("conexao ws: "+String(conexao_ws));
              Serial.println("conexao mdns: "+String(conexao_mdns));
              Serial.println(" ");
              printMillis = currentMillis;
          }
  }


  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);

}


void loop() {
    server.handleClient();
    client.poll();
    ArduinoOTA.handle();
    MDNS.update();
    while(!conexao_ws){conexao_ws = client.connect(websockets_server_host, websockets_server_port, "/");  Serial.println("Tentando reconexão com websocket");  delay(1000); }
    while(WiFi.status() != WL_CONNECTED) { WiFi.begin(id,pass); Serial.print("WiFi Disconnected");  delay(6000);}
}
