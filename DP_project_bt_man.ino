#include <ESP32Time.h>

#include <ArduinoJson.h>

#include <WiFi.h>
#include <WebServer.h>
#include "BluetoothSerial.h"
#include "Memory_dp.hpp"

ESP32Time rtc;
BluetoothSerial SerialBT;
WebServer server(80);

const int TIMEOUT=5000;

char* ssid     = "Warmachine"; //add your hotspot name here
char* password = "pihugolu1"; // add your hotspot pwd here

void handleRoot() {
  server.send(200, "text/plain", "Ready");
}

void handleGet() {
  if (server.hasArg("data")) {
    String data = server.arg("data");
    //JSONVar obj=JSON.parse(data);
    DynamicJsonDocument obj(1024);
    deserializeJson(obj, data);
    Serial.println("Data: " + data);
    serializeJson(obj, Serial);
/*    String s=obj["mon"];
 *    Serial.println(s);
 *    object structure -
 *
 *  obj["mon"]=[t1,t2,t3]
 *  obj["tue"]=[t1,t2,t3]
 *  ...
 *  obj["sun"]=[t1,t2,t3]
 *  assume any type for t1,t2,t3 as per your need, send request accordingly
*/
// to do - store the info whole table
  }
  server.send(200, "text/plain", "Data Received");
}

int setup_wifi()
{
    WiFi.begin(ssid, password);
    int curr=TIMEOUT;
    while (curr<TIMEOUT) {
        if(WiFi.status() != WL_CONNECTED) Serial.print(".");
        else break;
        delay(500);
        cur+=00;
    }
    if(WiFi.status()!=WL_CONNECTED)
    {
      Serial.println("Could not connect to wifi, timeout!");
      return 0;
    }
    return 1;
}

int setup_bt()
{
  // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
       Serial.println("no networks found");
    } else 
    {
      SerialBT.println("Choose a wifi name:-\n");
      SerialBT.println(n);
      SerialBT.println(" networks found");
      for (int i = 0; i < n; ++i) 
      {
      // Print SSID and RSSI for each network found
        SerialBT.print(i + 1);
        SerialBT.print(": ");
        SerialBT.print(WiFi.SSID(i));
        //Serial.print(" (");
        //Serial.print(WiFi.RSSI(i));
        //Serial.print(")");
        SerialBT.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
        delay(10);
      }
      SerialBT.println("");
      SerialBT.println("Choose the serial number corresponding to wifi name...:");
      int x=SerialBT.readString().toInt();
      SerialBT.println("Enter wifi password:");
      password=SerialBT.readString();
      ssid=WiFi.SSID(x-1);
      int res=setup_wifi();
      if(res)
      {
        SerialBT.println("Connected Successfully:-");
        SerialBT.println("Device IP-");
        SerialBT.println(WiFi.localIP());
        SerialBT.println("Copy above and paste it in the mobile app!");
        return 1; //success
      }
      else 
      {
        SerialBT.println("Try again sorry");
        return 0;
      }
      
    }

}
bool WIFI_ON=false;

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    if(SerialBT.available())
    if(setup_bt())
    {
  
      WiFi.softAP("ESP32");
      server.on("/", handleRoot);
      server.on("/get", HTTP_GET, handleGet);
  
      Serial.println(__DATE__);
      Serial.println(__TIME__);
      WIFI_ON=true;
    }
    else WIFI_ON=false;
  }
  else if(event == ESP_SPP_CLOSE_EVT ){
    Serial.println("Client disconnected");
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("MedPods");
  SerialBT.register_callback(callback);
}

void loop() {
  if(WIFI_ON) server.handleClient();
  
}
