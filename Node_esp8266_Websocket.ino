/*
  Adaptado por jefferson henrique.
 
*/
#include <WebSocketsClient_Generic.h>
#include <WebSocketsServer_Generic.h>
#include <Hash.h>
#include <Arduino_JSON.h>

#include <WiFiUdp.h>
#include <NTPClient.h>

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define LED 2
#define sizebuff 100
char pd[sizebuff];
JSONVar myObject;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "br.pool.ntp.org", -10800, 60000);


bool conexao_ws   = false;
bool obrig_ws     = false;
int  conexao_wifi = 0;  
bool conexao_mdns = false;


char html_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt-br">
  <head>
  <script>
  //function enviar();
  </script>
    <style>
      body {
        display: block;
        margin: 0px;
      }
      
      h1 {
        text-align: center;
        text-transform: uppercase;
        color: #4CAF50;
        font-size: 4.5vw;
      }

      .ta{
      min-height: 50vh;
          width: 90%;
          height: 50%
          margin-right: 5%;
          margin-left: 5%;
          height: 150px;
          padding: 12px 20px;
          box-sizing: border-box;
          border: 2px solid #ccc;
          border-radius: 4px;
          background-color: #f8f8f8;
          font-size: 16px;
          resize: none;
      }
      
      .but{
    margin-top: 5%;
        margin-left: 5%;
    font-size: 3vw;
        background-color: #04AA6D;
        border-radius: 6px;
        border-color: gray;
        color: white;
        padding: 2vw 4vw;
        text-decoration: none;
        cursor: pointer;
      }
   .classname{
    margin-top: 5%;
    width: 58vw;
        margin-left: 5%;
    font-size: 3vw;
        border-radius: 6px;
        padding: 2vw 4vw;
        cursor: pointer;
      }
    
    .divbut{
    align-itens: inline;
    }
    </style>
  </head>
  <body>
    <br>
    <h1> <label> Recebido da serial: </label> </h1>

    <form>
      <textarea class="ta" id="w3review" name="" rows="8" cols="50" readonly>
      
      </textarea>
      <br>
    </form>
  <div class="divbut">
    
  <form id="frm" >
    <button type="button"  class="but" onclick="enviar()">Enviar</button>
    <input type="text" class="classname" name="frm1" id="frm1"><br>
  </form>
  <br>
    <button type="button" class="but" onclick="limpar()">Limpar</button>
  </div>
  </body>

  <script>
    
    let ws_conect = false;
    let connection = new WebSocket('ws://192.168.4.1:81/'); 
      connection.onopen = function () {  
      ws_conect=true;
    }; 
    connection.onclose = function (error) {    
      console.log('WebSocket close ', error);
      ws_conect=false;
    };
    connection.onerror = function (error) {    
      console.log('WebSocket Error ', error);
    };
    connection.onmessage = function (e) { 
      myFunction(e.data);
      console.log(e.data);
    };
    setInterval(() => {
      if(ws_conect)connection.send("#");
    },100);

    document.getElementById("w3review").innerHTML = ""
    
  function enviar(){
    var input = document.querySelector("#frm1");
    var texto = input.value;
    if(texto != "")connection.send(texto);
    //if(texto != "")console.log(texto);
    document.getElementById("frm").reset();
  }
  
  
    function myFunction(t){
      atual = document.getElementById("w3review").innerHTML
      document.getElementById("w3review").innerHTML= atual + t + '\n'
      document.getElementById("w3review").scrollTop = document.getElementById("w3review").scrollHeight
    }
  function limpar(){
    document.getElementById("w3review").innerHTML="";
  }
  </script>
</html>
 )=====";


const char* id = "brisa-1394585"; //Enter SSID
const char* pass = "t5tb7ffx"; //Enter Password

const char* ssid = "ESP8266"; //Enter SSID
const char* password = "10203040"; //Enter Password

//const char* WS_SERVER = "safe-sea-86700.herokuapp.com"; //Enter server adress
const char* WS_SERVER = "node-red-nippon.herokuapp.com"; //Enter server adress
const uint16_t WS_PORT = 80; // Enter server port

ESP8266WebServer server(80);
WebSocketsClient webSocketc;
WebSocketsServer webSockets = WebSocketsServer(81);

bool flag_socketserial=false;
String socketserial;


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
        String(broadCast[3]); 
        webSockets.sendTXT(num,x); flag_socketserial=false;
        break;
      }


      if(payload[0] != '#')Serial.printf("%s", payload);
      if(flag_socketserial) { webSockets.sendTXT(num, "Serial:"+socketserial+"$"); flag_socketserial=false; }

      
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
      if (JSON.typeof(myObject) == "undefined") {
        socketserial = "Parsing input failed!";  flag_socketserial = true;
        return;
      }
      
      if (myObject.hasOwnProperty("output1")){
        if(String((const char*) myObject["output1"]) == "ON")digitalWrite(LED, LOW);
        if(String((const char*) myObject["output1"]) == "OFF")digitalWrite(LED, HIGH); 
        
        //Serial.println(timeClient.getFormattedTime());
        socketserial = "Socket: "+timeClient.getFormattedTime();  flag_socketserial = true;
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
  pinMode(LED, OUTPUT);

    
  Serial.begin(115200);
  //Serial.println("Serial.begin");
  delay(500);

  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);
  // Set WiFi to station mode
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);

  server.on("/", []() {
    server.send(200, "text/html", html_page);
  });
  server.begin();

  //server
  webSockets.begin();
  webSockets.onEvent(webSocketEvents);


  //client
  webSocketc.begin(WS_SERVER, WS_PORT, "/");
  webSocketc.onEvent(webSocketEventc);
  webSocketc.setReconnectInterval(5000);
  webSocketc.enableHeartbeat(15000, 3000, 2);




ArduinoOTA.setHostname("ESP_PRINCIPAL");
ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    //Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    //Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    //Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      //Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
     // Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      //Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      //Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      //Serial.println("End Failed");
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
          if(!conexao_mdns)conexao_mdns = MDNS.begin("principal");

          if(currentMillis > printMillis+1000){
              //Serial.println("WiFi");
              //Serial.println("conexao wifi: "+String(conexao_wifi));
              //Serial.println("conexao ws: "+String(conexao_ws));
              //Serial.println("conexao mdns: "+String(conexao_mdns));
              //Serial.println(" ");
              printMillis = currentMillis;
          }
  }

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  timeClient.begin();
}

uint32_t current_millis = 0;
uint32_t current_millis2 = 0;

void loop() {
  webSocketc.loop();
  webSockets.loop();
  server.handleClient();
  ArduinoOTA.handle();
  MDNS.update();
  
  if (Serial.available()) {
    String msg = Serial.readString();
    socketserial = msg;  flag_socketserial = true;
  }
  
  if(millis() > current_millis + 5000){ webSocketc.sendTXT(timeClient.getFormattedTime());   current_millis = millis();}  //  atualiza a hora a cada 100 millisegundos.
  if(millis() > current_millis2 + 100){ timeClient.update();   current_millis2 = millis();   }  //  atualiza a hora a cada 100 millisegundos.
  //while(!conexao_ws){conexao_ws = client.connect(websockets_server_host, websockets_server_port, "/");  Serial.println("Tentando reconexão com websocket");  delay(1000); }
  //while(WiFi.status() != WL_CONNECTED) { WiFi.begin(id,pass); Serial.print("WiFi Disconnected");  delay(6000);}
}
