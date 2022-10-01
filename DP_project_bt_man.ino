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
int gpios[21]={2,3,4,5,12,13,14,15,16,17,18,19,20,21,22,23,25,26,27,32,33};
int BUZZER=2;

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

bool UNSET=true;
void setup() {
  Serial.begin(115200);
  setup_wifi();
  configTime(gmtOffset_sec, 0, ntpServer1, ntpServer2);
  setup_mem();
  for(int i=0; i<21; i++)
  {
    pinMode(gpios[i],OUTPUT);
  }
  pinMode(BUZZER,OUTPUT);
  
}
void set_out(bool *arr)
{
  for(int i=0; i<5; i++)
  {
    if(arr[i]) digitalWrite(pins[i],HIGH);
    else digitalWrite(pins[i],LOW);
  }
}
int lastIndex=0;
void set_out_unmulti(int i)
{
  setBuzzer(i>0);
  if(i==0) {digitalWrite(gpios[lastIndex],LOW);lastIndex=0;return;}
  if(lastIndex!=i-1) digitalWrite(gpios[lastIndex],LOW);
  lastIndex=i-1;
  digitalWrite(gpios[i-1],HIGH);
}
void setBuzzer(bool val)
{
  if(val) digitalWrite(BUZZER,HIGH);
  else digitalWrite(BUZZER,LOW);
}
void loop() {
     int gpioindex;
    struct tm t;
    server.handleClient();
    //Serial.print("UNset=");
    //Serial.println(UNSET);
    if(UNSET and getLocalTime(&t))
    {
      //Serial.println(t.tm_wday);
      //Serial.println(t.tm_hour*100+t.tm_min);
      time_loop(t.tm_wday,t.tm_hour*100+t.tm_min);
      //set_out(arr);
      rtc.setTimeStruct(t);
      UNSET=false;
    }
    else if(!UNSET)
    {
      gpioindex=time_loop(rtc.getDayofWeek(),rtc.getHour(true)*100+rtc.getMinute());
      //Serial.println(rtc.getDayofWeek());
      //Serial.println(rtc.getHour()*100+rtc.getMinute());
      //set_out(arr);
      set_out_unmulti(gpioindex);
      if(gpioindex!=lastIndex)
      {
        Serial.println(gpioindex);
        if(gpioindex>0) Serial.println(gpios[gpioindex-1]);
      }
      //setBuzzer(gpioindex>0);
    }
    
  //Serial.println(WiFi.status()==WL_CONNECTED);
  //Serial.println(WiFi.localIP());
  
}
