#include <ESP32Time.h>

#include <ArduinoJson.h>

#include <WiFi.h>
#include <WebServer.h>
//#include "BluetoothSerial.h"
#include "Memory_dp.hpp"
#include "time.h"
#include "sntp.h"

ESP32Time rtc;
//BluetoothSerial SerialBT;
WebServer server(80);
bool WIFI_ON=false;

const int TIMEOUT=5000;

char* ssid     = "Warmachine"; //add your hotspot name here
char* password = "pihugolu1"; // add your hotspot pwd here

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 19800;

const String days[7]={"sun","mon","tue","wed","thu","fri","sat"};

int pins[5]={25,26,27,12,14};


void save_obj(DynamicJsonDocument obj)
{
  for(int d=0; d<7; d++)
  {
    String day_tt=obj[days[d]]; /*[t1,t2,t3]*/
    int len = day_tt.length();
    int day_slots[3];
    int j=0;
    String temp = "";
    for(int i=0; i<len; i++){
      if(day_tt[i]>='0' && day_tt[i]<='9'){
        temp.concat(day_tt[i]);
      }
      else if(temp.length()>0){
        day_slots[j]=temp.toInt();
        j++;
        temp="";
      }
    }
    for(int i=0; i<3; i++){
      //Serial.println(day_slots[i]);
      update_mem(d, i, day_slots[i]);
      Serial.println(slots[d][i]);
    }
  }
}

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
    Serial.println("");
    save_obj(obj);
/*    String s=obj["mon"];
 *    Serial.println(s);
 *    object structure -
 *
 *  obj["mon"]="[t1,t2,t3]"
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
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
    }
      server.on("/", handleRoot);
      server.on("/get", HTTP_GET, handleGet);
  
      Serial.println(__DATE__);
      Serial.println(__TIME__);
      Serial.println(WiFi.localIP());
      server.begin();
      WIFI_ON=true;
    return 1;
}

//int setup_bt()
//{
//  // WiFi.scanNetworks will return the number of networks found
//    int n = WiFi.scanNetworks();
//    Serial.println("scan done");
//    if (n == 0) {
//       Serial.println("no networks found");
//    } else 
//    {
//      SerialBT.println("Choose a wifi name:-\n");
//      SerialBT.println(n);
//      SerialBT.println(" networks found");
//      for (int i = 0; i < n; ++i) 
//      {
//      // Print SSID and RSSI for each network found
//        SerialBT.print(i + 1);
//        SerialBT.print(": ");
//        SerialBT.print(WiFi.SSID(i));
//        //Serial.print(" (");
//        //Serial.print(WiFi.RSSI(i));
//        //Serial.print(")");
//        SerialBT.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
//        delay(10);
//      }
//      SerialBT.println("");
//      SerialBT.println("Choose the serial number corresponding to wifi name...:");
//      int x=SerialBT.readString().toInt();
//      SerialBT.println("Enter wifi password:");
//      password=SerialBT.readString();
//      ssid=WiFi.SSID(x-1);
//      int res=setup_wifi();
//      if(res)
//      {
//        SerialBT.println("Connected Successfully:-");
//        SerialBT.println("Device IP-");
//        SerialBT.println(WiFi.localIP());
//        SerialBT.println("Copy above and paste it in the mobile app!");
//        return 1; //success
//      }
//      else 
//      {
//        SerialBT.println("Try again sorry");
//        return 0;
//      }
//      
//    }
//
//}

//void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
//  if(event == ESP_SPP_SRV_OPEN_EVT){
//    if(SerialBT.available())
//    if(setup_bt())
//    {
//  

//    }
//    else WIFI_ON=false;
//  }
//  else if(event == ESP_SPP_CLOSE_EVT ){
//    Serial.println("Client disconnected");
//  }
//}
bool UNSET=true;
void setup() {
  Serial.begin(115200);
  setup_wifi();
  configTime(gmtOffset_sec, 0, ntpServer1, ntpServer2);
  setup_mem();
  for(int i=0; i<5; i++)
  {
    pinMode(pins[i],OUTPUT);
  }
  
}
void set_out(bool *arr)
{
  for(int i=0; i<5; i++)
  {
    if(arr[i]) digitalWrite(pins[i],HIGH);
    else digitalWrite(pins[i],LOW);
  }
}
void loop() {
  bool arr[5];
    struct tm t;
    server.handleClient();
    //Serial.print("UNset=");
    //Serial.println(UNSET);
    if(UNSET and getLocalTime(&t))
    {
      Serial.println(t.tm_wday);
      Serial.println(t.tm_hour*100+t.tm_min);
      time_loop(arr,t.tm_wday,t.tm_hour*100+t.tm_min);
      set_out(arr);
      rtc.setTimeStruct(t);
      UNSET=false;
    }
    else if(!UNSET)
    {
      time_loop(arr,rtc.getDayofWeek(),rtc.getHour(true)*100+rtc.getMinute());
      //Serial.println(rtc.getDayofWeek());
      //Serial.println(rtc.getHour()*100+rtc.getMinute());
      set_out(arr);
    }
    
  //Serial.println(WiFi.status()==WL_CONNECTED);
  //Serial.println(WiFi.localIP());
  
}
