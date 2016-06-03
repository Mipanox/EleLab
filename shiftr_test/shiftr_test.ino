#define enable 4
#define load 2
#define clock 3
#define data 5
#define data2 6

#define debug

void setup()
{
pinMode(enable,OUTPUT);
pinMode(load,OUTPUT);
pinMode(clock,OUTPUT);
pinMode(data,INPUT);
pinMode(data2,INPUT);
digitalWrite(load,HIGH);
digitalWrite(enable,HIGH);
Serial.begin(9600);
}
 
void loop()
{
digitalWrite(load,LOW);
delayMicroseconds(2);
//delay(50);
digitalWrite(load,HIGH);
delayMicroseconds(2);
//delay(50);
digitalWrite(clock,HIGH);
digitalWrite(enable,LOW);
byte incoming=shiftIn(data,clock,MSBFIRST);
byte incomin2=shiftIn(data2,clock,MSBFIRST);
digitalWrite(enable,HIGH);

  #ifdef debug
  Serial.print(incoming,BIN); Serial.print(" ");
  Serial.print(incomin2,BIN); Serial.println(" "); Serial.println("---------");
  #endif
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
    if (temp) {
      pinState = 1;
      myDataIn = myDataIn | (1 << i);
    }
    else {
      pinState = 0;
    }
    digitalWrite(myClockPin, 1);

  }
  return myDataIn;
}
