#include <EEPROM.h>

int slots[7][3];

// inital values 

void initSlot(){
  for(int i=0; i<7; i++){
    for(int j=0; j<3; j++){
      slots[i][j]=2500; // setting default values to 25:00 so that it never matches
    }
  }
  
  // Adding initial slots to EEPROM
  for (int i = 0 ; i < 21 ; i++) {
      EEPROM.write(i, slots[i/3][i%3]);
  }
  EEPROM.write(25,1); // set 1 as it is initialized
}


// Updating any slot
void update(int d, int s, int time){
  slots[d][s]=time;
  EEPROM.write(d*3+s, time);
}

// sets up memory
void setup_mem()
{
  int isSet = EEPROM.read(25);
  if(!isSet) initSlot();
  // Adding initial slots to EEPROM
  for (int i = 0 ; i < 21 ; i++) {
      slots[i/3][i%3] = EEPROM.read(i);
  }
}


void time_loop(int day, int curr_t)
{
  for(int s=0; s<3; s++){
    if(slots[day][s]<=curr_t && curr_t<slots[day][s]+2){
      //Serial.println("Matched");
      beepStart();
      ledStart(day, s);
      delay(6000);
      beepStop();
      ledStop();
    }
  }
}
