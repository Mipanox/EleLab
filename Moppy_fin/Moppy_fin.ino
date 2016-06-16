#include <TimerOne.h>

boolean firstRun = true; // Used for one-run-only stuffs;

//First pin being used for floppies, and the last pin. Used for looping over all pins.
const byte FIRST_PIN = 2;
const byte PIN_MAX = 17;
#define RESOLUTION 40 //Microsecond resolution for notes

byte MAX_POSITION[] = {0,0,158,0,158,0,158,0,158,0,158,0,158,0,158,0,158,0};
byte currentPosition[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int currentState[] = {0,0,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW};
unsigned int currentPeriod[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned int currentTick[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

float p = 6300;

#define enable 4
#define load 2
#define clock 3
#define data 5

#define debugg

byte in_1 = 0;  // 01001000 # for debugging
byte in_2 = 0;  // 10011111
byte in_3 = 0;
byte in_4 = 0;
byte in_5 = 0;

float per[129]={}; float pe2[129]={}; float pe3[129]={}; float pe4[129]={}; float pe5[129]={};

//Setup pins (Even-odd pairs for step control and direction
void setup(){
  pinMode(13, OUTPUT);// step
  pinMode(12, OUTPUT); // d
  pinMode(11, OUTPUT); // s
  pinMode(10, OUTPUT); // d
  pinMode(9, OUTPUT); // s
  pinMode(8, OUTPUT); // d

  pinMode(enable, OUTPUT);
  pinMode(load, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, INPUT);
  digitalWrite(load, HIGH);
  digitalWrite(enable, HIGH);

  Timer1.initialize(RESOLUTION); // Set up a timer at the defined resolution
  Timer1.attachInterrupt(tick); // Attach the tick function

  // C1
  per[1]  = p/32.70; per[2]  = p/34.65; per[4]  = p/36.71; per[8]   = p/38.89; // D#1
  per[16] = p/41.20; per[32] = p/43.65; per[64] = p/46.25; per[128] = p/49.00; // G1
  // G#1
  pe2[1]  = p/51.91; pe2[2]  = p/55.00; pe2[4]  = p/58.27; pe2[8]   = p/61.74; // B1
  pe2[16] = p/65.41; pe2[32] = p/69.30; pe2[64] = p/73.42; pe2[128] = p/77.78; // D#2
  // E2
  pe3[1]  = p/82.41 ; pe3[2]  = p/87.31 ; pe3[4]  = p/92.50 ; pe3[8]   = p/98.00;  // G2
  pe3[16] = p/103.83; pe3[32] = p/111.00; pe3[64] = p/116.54; pe3[128] = p/123.47; // B2
  // C3
  pe4[1]  = p/130.81; pe4[2]  = p/138.59; pe4[4]  = p/146.83; pe4[8]   = p/155.56; // D#3
  pe4[16] = p/164.81; pe4[32] = p/174.61; pe4[64] = p/185.00; pe4[128] = p/196.00; // G3
  // G#3
  pe5[1]  = p/207.65; pe5[2]  = p/220.00; pe5[4]  = p/233.08; pe5[8]   = p/246.94; // B3
  pe5[16] = p/261.63; pe5[32] = p/277.18; pe5[64] = p/293.66; pe5[128] = p/311.13; // D#4
  Serial.begin(9600);
}


uint32_t last = 0;
byte memory = 0; byte memor2 = 0; byte memor3 = 0; byte memor4 = 0; byte memor5 = 0;
#define floppy

void loop()
{
  //The first loop, reset all the drives, and wait 2 seconds...
  if (firstRun)
  {
    firstRun = false;
    resetAll();
    delay(2000);
  }
  
  byte d_1 = 0; byte d_2 = 0; byte d_3 = 0; byte d_4 = 0; byte d_5 = 0; 
  while (micros() - last < 50) // prevents instantaneous change not recorded by arduino
  {
    digitalWrite(load,LOW);  delayMicroseconds(2);
    digitalWrite(load,HIGH); delayMicroseconds(2);
    digitalWrite(clock,HIGH);
    digitalWrite(enable,LOW);
    in_1 = ~shiftIn(data,clock,MSBFIRST) & 0B11111111;
    in_2 = ~shiftIn(data,clock,MSBFIRST) & 0B11111111;
    in_3 = ~shiftIn(data,clock,MSBFIRST) & 0B11111111;
    in_4 = ~shiftIn(data,clock,MSBFIRST) & 0B11111111;
    in_5 = ~shiftIn(data,clock,MSBFIRST) & 0B11111111;
    digitalWrite(enable,HIGH);
    d_1 |= in_1;
    d_2 |= in_2;
    d_3 |= in_3;
    d_4 |= in_4;
    d_5 |= in_5;
  }

  #ifdef debug
    for (int i=7; i>=0; i--){ Serial.print(bitRead(in_1,i)); }
    Serial.print(" ");
    for (int i=7; i>=0; i--){ Serial.print(bitRead(in_2,i)); }
    Serial.println(" "); Serial.println("---------");
  #endif

  #ifdef debugg
    for (int i=7; i>=0; i--){ Serial.print(bitRead(d_1,i)); }
    Serial.print(" ");
    for (int i=7; i>=0; i--){ Serial.print(bitRead(d_2,i)); }
    Serial.print(" "); 
    for (int i=7; i>=0; i--){ Serial.print(bitRead(d_3,i)); }
    Serial.print(" ");
    for (int i=7; i>=0; i--){ Serial.print(bitRead(d_4,i)); }
    Serial.print(" ");
    for (int i=7; i>=0; i--){ Serial.print(bitRead(d_5,i)); }
    Serial.println(" "); Serial.println("---------");
  #endif

  if( d_1 ^ memory || d_2 ^ memor2 || d_3 ^ memor3 || d_4 ^ memor4 || d_5 ^ memor5)
  {
    byte temp = d_1 ^ memory; 
    byte tem2 = d_2 ^ memor2;
    byte tem3 = d_3 ^ memor2;
    byte tem4 = d_4 ^ memor2;
    byte tem5 = d_5 ^ memor2;

    // key pressed
    if (d_1 > memory)
    {
      for (int j=1; j<255; j*=2) // do NOT use 'byte' ! otherwise j will return to 0 when > 256 !->infinite loop!
      { byte jj = j;
        if (temp & jj) // if more than 1 keys pressed
        {
          if (currentPeriod[8]==0) { currentPeriod[8] = per[j]; }
          else if (currentPeriod[10]==0) { currentPeriod[10] = per[j]; }
          else if (currentPeriod[12]==0) { currentPeriod[12] = per[j]; }
        }
      }
    } 
    else if (d_2 > memor2)
    {
      for (int j=1; j<255; j*=2)
      { byte jj = j;
        if (tem2 & jj)
        {
          if (currentPeriod[8]==0) { currentPeriod[8] = pe2[tem2]; }
          else if (currentPeriod[10]==0) { currentPeriod[10] = pe2[tem2]; }
          else if (currentPeriod[12]==0) { currentPeriod[12] = pe2[tem2]; }
        }
      }
    }
    else if (d_3 > memor3)
    {
      for (int j=1; j<255; j*=2)
      { byte jj = j;
        if (tem3 & jj)
        {
          if (currentPeriod[8]==0) { currentPeriod[8] = pe3[tem3]; }
          else if (currentPeriod[10]==0) { currentPeriod[10] = pe3[tem3]; }
          else if (currentPeriod[12]==0) { currentPeriod[12] = pe3[tem3]; }
        }
      }
    }
    else if (d_4 > memor4)
    {
      for (int j=1; j<255; j*=2)
      { byte jj = j;
        if (tem4 & jj)
        {
          if (currentPeriod[8]==0) { currentPeriod[8] = pe4[tem4]; }
          else if (currentPeriod[10]==0) { currentPeriod[10] = pe4[tem4]; }
          else if (currentPeriod[12]==0) { currentPeriod[12] = pe4[tem4]; }
        }
      }
    }
    else if (d_5 > memor5)
    {
      for (int j=1; j<255; j*=2)
      { byte jj = j;
        if (tem5 & jj)
        {
          if (currentPeriod[8]==0) { currentPeriod[8] = pe5[tem5]; }
          else if (currentPeriod[10]==0) { currentPeriod[10] = pe5[tem5]; }
          else if (currentPeriod[12]==0) { currentPeriod[12] = pe5[tem5]; }
        }
      }
    }

    // key released
    
    else if (d_1 < memory)
    {
      if (currentPeriod[8]==per[temp]) { currentPeriod[8] = 0; }
      else if (currentPeriod[10]==per[temp]) { currentPeriod[10] = 0 ;}
      else if (currentPeriod[12]==per[temp]) { currentPeriod[12] = 0 ;}
    }
    else if (d_2 < memor2)
    {
      if (currentPeriod[8]==pe2[tem2]) { currentPeriod[8] = 0; }
      else if (currentPeriod[10]==pe2[tem2]) { currentPeriod[10] = 0 ;}
      else if (currentPeriod[12]==pe2[tem2]) { currentPeriod[12] = 0 ;}
    }
    else if (d_3 < memor3)
    {
      if (currentPeriod[8]==pe3[tem3]) { currentPeriod[8] = 0; }
      else if (currentPeriod[10]==pe3[tem3]) { currentPeriod[10] = 0 ;}
      else if (currentPeriod[12]==pe3[tem3]) { currentPeriod[12] = 0 ;}
    }
    else if (d_4 < memor4)
    {
      if (currentPeriod[8]==pe4[tem4]) { currentPeriod[8] = 0; }
      else if (currentPeriod[10]==pe4[tem4]) { currentPeriod[10] = 0 ;}
      else if (currentPeriod[12]==pe4[tem4]) { currentPeriod[12] = 0 ;}
    }
    else
    {
      if (currentPeriod[8]==pe5[tem5]) { currentPeriod[8]  = 0 ;}
      else if (currentPeriod[10]==pe5[tem5]) { currentPeriod[10] = 0 ;}
      else if (currentPeriod[12]==pe5[tem5]) { currentPeriod[12] = 0 ;}
    }
  }
  memory = d_1; memor2 = d_2; memor3 = d_3; memor4 = d_4; memor5 = d_5;
  last = micros();

}

void tick()
{
  if (currentPeriod[2]>0){
    currentTick[2]++;
    if (currentTick[2] >= currentPeriod[2]){
      togglePin(2,3);
      currentTick[2]=0;
    }
  }
  if (currentPeriod[4]>0){
    currentTick[4]++;
    if (currentTick[4] >= currentPeriod[4]){
      togglePin(4,5);
      currentTick[4]=0;
    }
  }
  if (currentPeriod[6]>0){
    currentTick[6]++;
    if (currentTick[6] >= currentPeriod[6]){
      togglePin(6,7);
      currentTick[6]=0;
    }
  }
  if (currentPeriod[8]>0){
    currentTick[8]++;
    if (currentTick[8] >= currentPeriod[8]){
      togglePin(8,9);
      currentTick[8]=0;
    }
  }
  if (currentPeriod[10]>0){
    currentTick[10]++;
    if (currentTick[10] >= currentPeriod[10]){
      togglePin(10,11);
      currentTick[10]=0;
    }
  }
  if (currentPeriod[12]>0){
    currentTick[12]++;
    if (currentTick[12] >= currentPeriod[12]){
      togglePin(12,13);
      currentTick[12]=0;
    }
  }
  if (currentPeriod[14]>0){
    currentTick[14]++;
    if (currentTick[14] >= currentPeriod[14]){
      togglePin(14,15);
      currentTick[14]=0;
    }
  }
  if (currentPeriod[16]>0){
    currentTick[16]++;
    if (currentTick[16] >= currentPeriod[16]){
      togglePin(16,17);
      currentTick[16]=0;
    }
  }
  
}

void togglePin(byte pin, byte direction_pin) {
  
  //Switch directions if end has been reached
  if (currentPosition[pin] >= MAX_POSITION[pin]) {
    currentState[direction_pin] = HIGH;
    digitalWrite(direction_pin,HIGH);
  }
  else if (currentPosition[pin] <= 0) {
    currentState[direction_pin] = LOW;
    digitalWrite(direction_pin,LOW);
  }
  
    //Update currentPosition
  if (currentState[direction_pin] == HIGH){
    currentPosition[pin]--;
  }
  else {
    currentPosition[pin]++;
  }
  
  //Pulse the control pin
  digitalWrite(pin,currentState[pin]);
  currentState[pin] = ~currentState[pin];
}

byte shiftIn(int myDataPin, int myClockPin) { 
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);

  for (i=7; i>=0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(0.2);
    temp = digitalRead(myDataPin);
    if (temp) 
    {
      pinState = 1;
      myDataIn = myDataIn | (1 << i);
    }
    else 
    {
      pinState = 0;
    }
    digitalWrite(myClockPin, 1);
  }
  return myDataIn;
}


//
//// UTILITY FUNCTIONS
//

//Not used now, but good for debugging...
void blinkLED(){
  digitalWrite(13, HIGH); // set the LED on
  delay(250); // wait for a second
  digitalWrite(13, LOW);
}

//For a given controller pin, runs the read-head all the way back to 0
void reset(byte pin)
{
  digitalWrite(pin+1,HIGH); // Go in reverse
  for (byte s=0;s<MAX_POSITION[pin];s+=2){ //Half max because we're stepping directly (no toggle)
    digitalWrite(pin,HIGH);
    digitalWrite(pin,LOW);
    delay(5);
  }
  currentPosition[pin] = 0; // We're reset.
  digitalWrite(pin+1,LOW);
  currentPosition[pin+1] = 0; // Ready to go forward.
}

//Resets all the pins
void resetAll(){
  
  // Old one-at-a-time reset
  //for (byte p=FIRST_PIN;p<=PIN_MAX;p+=2){
  // reset(p);
  //}
  
  // New all-at-once reset
  for (byte s=0;s<80;s++){ // For max drive's position
    for (byte p=FIRST_PIN;p<=PIN_MAX;p+=2){
      digitalWrite(p+1,HIGH); // Go in reverse
      digitalWrite(p,HIGH);
      digitalWrite(p,LOW);
    }
    delay(5);
  }
  
  for (byte p=FIRST_PIN;p<=PIN_MAX;p+=2){
    currentPosition[p] = 0; // We're reset.
    digitalWrite(p+1,LOW);
    currentState[p+1] = 0; // Ready to go forward.
  }
  
}
