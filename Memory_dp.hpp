#include <EEPROM.h>

int slots[7][3];

// inital values
void initSlot(){
  for(int i=0; i<7; i++){
      slots[i][0]=800; // setting default values to 25:00 so that it never matches
      slots[i][1]=1700;
      slots[i][2]=2200;
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
void update(int d, int s, int time){
  slots[d][s]=time;
  EEPROM.write(d*3+s, time);
}

// Geting the time of a slot
int getSlot(int d, int s){
  return slots[d][s];
}

// To get status of led
bool[] getStatus(int d, int s){
  int num = d*3 + s;
  int arr[5];
  for(int i=0; i<=4; i++){
    arr[i]=num%2;
    num=num/2;
  }
  return arr;
}

// sets up memory
void setup_mem()
{
  int isSet = EEPROM.read(25);
  if(!isSet) initSlot();
  else loadSlot();
}


void time_loop(int day, int curr_t)
{
  for(int s=0; s<3; s++){
    if(slots[day][s]<=curr_t && curr_t<slots[day][s]+2){
      beepStart();
      ledStart(day, s);
      delay(6000);
      beepStop();
      ledStop();
    }
  }
}
