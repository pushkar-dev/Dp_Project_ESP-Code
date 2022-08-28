#include <EEPROM.h>

int slots[7][3];

// inital values
void initSlot(){
  for(int i=0; i<7; i++){
      slots[i][0]=800; // setting default values to 25:00 so that it never matches
      slots[i][1]=1400;
      slots[i][2]=2000;
  }
  // Adding initial slots to EEPROM
  for (int i = 0 ; i < 21 ; i++) {
      EEPROM.write(i, slots[i/3][i%3]);
  }
  EEPROM.write(25,1); // set 1 as it is initialized
}

// Loading slots from EEPROM to slots[][]
void loadSlot(){
  for (int i = 0 ; i < 21 ; i++) {
      slots[i/3][i%3] = EEPROM.read(i);
  }
}

// Updating any slot
void update_mem(int d, int s, int t){
  slots[d][s]=t;
  EEPROM.write(d*3+s, t);
}

// Geting the time of a slot
int getSlot(int d, int s){
  return slots[d][s];
}

// To get status of led
void to_bin(bool *res, int a)
{
  for(int i=0; i<5; i++)
  {
    res[i]=a%2;
    a/=2;
  }
}

// sets up memory
void setup_mem()
{
  int isSet = EEPROM.read(25);
  if(!isSet) initSlot();
  else loadSlot();
}


void time_loop(bool * arr,int day, int curr_t)
{
  to_bin(arr,0);
  for(int s=0; s<3; s++){
    Serial.println(slots[day][s]);
    Serial.println(curr_t);
    if(slots[day][s]<=curr_t && curr_t<slots[day][s]+2){
//      beepStart();
//      ledStart(day, s);
//      delay(6000);
//      beepStop();
//      ledStop();
        to_bin(arr,day+s+1);
        Serial.print("alarm for:");
        Serial.println(day+s);
//        Serial.println(arr[0]);
//        Serial.println(arr[1]);
//        Serial.println(arr[2]);
//        Serial.println(arr[3]);
//        Serial.println(arr[4]);
    }
  }
  
}
