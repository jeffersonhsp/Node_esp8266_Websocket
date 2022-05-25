#include <WebSocketsClient_Generic.h>
#include <WebSocketsServer_Generic.h>
#include <Hash.h>
#include <Arduino_JSON.h>
#include <ESP8266HTTPUpdateServer.h>
#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


#define _WEBSOCKETS_LOGLEVEL_     2
#define sizebuff 128
#define LED 2

char pd[sizebuff];
JSONVar myObject;

String socketserials;
String socketserials_out;
bool flag_recebe_ser_esp=false;
bool flag_env_ser_esp=false;

bool conexao_ws   = false;


char ssid[30]; //Enter SSID
char password[30];  //Enter Password



const char* host = "node-red-nippon.herokuapp.com"; //Enter server adress
const uint16_t port = 80; // Enter server port

#define LED 2

ESP8266WebServer serverl(80);
ESP8266HTTPUpdateServer httpUpdater;

WebSocketsClient webSocketc;
WebSocketsServer webSockets = WebSocketsServer(81);

#include "html_page.h"
#include "html_login.h"



//Server
void webSocketEvents(const uint8_t& num, const WStype_t& type, uint8_t * payload, const size_t& length)
{
  (void) length;
  
  switch (type)
  {
    case WStype_DISCONNECTED:
      //Serial.printf("[%u] Disconnected!\n", num);
      break;
      
    case WStype_CONNECTED:
      {
        IPAddress ip = webSockets.remoteIP(num);
        //Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        //webSockets.sendTXT(num, "Connected");
      }
      break;
      
    case WStype_TEXT:
      
      if(payload[0]=='I' && payload[1]=='p' && payload[2]=='#'){
        IPAddress broadCast = WiFi.localIP();
        String x = "ip do esp na rede :"+\
        String(broadCast[0]) + String(".") +\
        String(broadCast[1]) + String(".") +\
        String(broadCast[2]) + String(".") +\
        String(broadCast[3]) + "\nssid: " + ssid +"\n" +\
        "password: "+ password;
        
        
        webSockets.sendTXT(num,x);
        break;
      }

      
      if(payload[0] == 's' && payload[1] == 's' && payload[2] == 'i' && payload[3] == 'd' && payload[4] == '='){
        for(int i=0;i<30;i++)ssid[i]=0;
        for(int i=5;i<128;i++){
          ssid[i-5] = payload[i];
          if(payload[i+1] == 0)i=128; 
        }
        for(int i=0;i<30;i++)EEPROM.write(i, ssid[i]);
        EEPROM.commit();
        delay(50);
        socketserials_out = "Senha alterada, Reboot...\n";
        flag_env_ser_esp = true;
        delay(50);
        ESP.restart();
      }
      if(payload[0]=='p' && payload[1]=='a' && payload[2]=='s' && payload[3]=='s' && payload[4]=='='){
        for(int i=0;i<30;i++)password[i]=0;
        for(int i=5;i<128;i++){
          password[i-5] = payload[i];
          if(payload[i+1] == 0)i=128; 
        }
        for(int i=0;i<30;i++)EEPROM.write(i+30, password[i]);
        EEPROM.commit();
        delay(50);
        socketserials_out = "Senha alterada, Reboot...\n";
        flag_env_ser_esp = true;
        delay(50);
        ESP.restart();
      }

      
      
      if(payload[0] != '#'){
        String msg3 ="";
        for(int i=0;i<128;i++){
          msg3 += (char)payload[i];
          if(payload[i+1] == 0)i=256; 
        }
        socketserials_out=msg3+"\n";
        flag_env_ser_esp = true;
        if(flag_recebe_ser_esp) { webSockets.sendTXT(num, "< "+socketserials); flag_recebe_ser_esp=false; socketserials = ""; }
      }
      break;
      
    case WStype_BIN:
      //Serial.printf("[%u] get binary length: %u\n", num, length);
      //hexdump(payload, length);
      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;

    default:
      break;
  }
}





//client
void webSocketEventc(const WStype_t& type, uint8_t * payload, const size_t& length)
{
  switch (type)
  {
    case WStype_DISCONNECTED:
      if (conexao_ws)
      {
        //Serial.println("[WSc] Disconnected!");
        conexao_ws = false;
      }
      break;
    case WStype_CONNECTED:
      {
        conexao_ws = true;
        //Serial.print("[WSc] Connected to url: ");
        //Serial.println((char *) payload);

        // send message to server when Connected
        //webSocketc.sendTXT("Connected");
      }
      break;
    case WStype_TEXT:
      for(uint8_t i=0;i<sizebuff;i++)pd[i] = 0;
      for(uint8_t i=0;i<sizebuff;i++){
        if(payload[i])pd[i]=payload[i];
        else i=sizebuff;
      }


      
      myObject = JSON.parse(pd);
      if(JSON.typeof(myObject) == "undefined"){
        //socketserialc = "Parsing input failed!";  flag_recebe_ser_esp = true;
        return;
      }
      
      if (myObject.hasOwnProperty("output1")){
        if(String((const char*) myObject["output1"]) == "ON")digitalWrite(LED, LOW);
        if(String((const char*) myObject["output1"]) == "OFF")digitalWrite(LED, HIGH); 
        
        socketserials = String((const char*) myObject);  
        flag_recebe_ser_esp = true;
        
        //Serial.println(timeClient.getFormattedTime());
        //socketserials = "Socket:"+timeClient.getFormattedTime();  ;
      }
      
      break;
    case WStype_BIN:
      //Serial.printf("[WSc] get binary length: %u\n", length);
      //hexdump(payload, length);

      // send data to server
      //webSocketc.sendBIN(payload, length);
      break;

    case WStype_PING:
      // pong will be send automatically
      //Serial.printf("[WSc] get ping\n");
      break;
      
    case WStype_PONG:
      // answer to a ping we send
      //Serial.printf("[WSc] get pong\n");
      break;

    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;

    default:
      break;
  }
}







 
void setup() {
    EEPROM.begin(64);
    for(int i=0;i<30;i++){
      ssid[i] = EEPROM.read(i);
      password[i] = EEPROM.read(i+30);
    }
    pinMode(LED, OUTPUT);
    Serial.begin(115200);
    
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("APP", "password");
    WiFi.begin(ssid, password);
    // Wait some time to connect to wifi
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }
    // Check if connected to wifi
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("No Wifi!");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("APP", "password");
        Serial.println("Somente  AP");
    }
 


    if (MDNS.begin("principal")) {
      Serial.println("MDNS responder started");
    }

    
    serverl.on("/serial", []()
    {
      serverl.send(200, "text/html", html_page);
    });
    serverl.on("/", handleRoot);
    serverl.on("/login", handleLogin);
    serverl.onNotFound(handleNotFound);
    
    //ask server to track these headers
    serverl.collectHeaders("User-Agent", "Cookie");


  

    httpUpdater.setup(&serverl);
    serverl.begin();

    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);

    
        //ws server
    webSockets.begin();
    webSockets.onEvent(webSocketEvents);
  
  
    //ws client
    webSocketc.begin(host, port, "/");
    webSocketc.onEvent(webSocketEventc);
    webSocketc.setReconnectInterval(5000);
    webSocketc.enableHeartbeat(15000, 3000, 2);


  
     
    Serial.println("HTTP server started");

   


}



uint32_t c_millis = 0;
int sensorValue = 0;
void loop() {
    webSocketc.loop();
    webSockets.loop();
    serialEvent();
    
    if(millis() > c_millis + 100){
      //digitalWrite(LED,!(digitalRead(LED)));  
      sensorValue = analogRead(A0);
      webSocketc.sendTXT(String(sensorValue));
      c_millis = millis();
    }
    
    serverl.handleClient();
    MDNS.update();
}






void serialEvent(){

      
  if(flag_env_ser_esp){
      Serial.print(socketserials_out);
      flag_env_ser_esp=false; 
      socketserials_out = ""; 
  }


  
  while (Serial.available()){
    char inChar = (char)Serial.read();
    socketserials += inChar;
    if (inChar == '\n') {
      flag_recebe_ser_esp = true;
    }
  }
}
