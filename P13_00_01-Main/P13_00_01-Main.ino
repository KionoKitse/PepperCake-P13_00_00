/*
  Nomal mode:
  > If not on time
  > > Check for button press
  > > Display blink if needed
  Menue mode:
  > Press and hold until led blinks (3sec)
  > Click button to set percent on
  > Press and hold to set
  > LED blinks out the percentage
  Running mode
  > Turn on MOSFET
  > Wait until time to turn off
  > Check the voltage average of 3
  > Turn off MOSFET
  > Normal mode 
  
*/

int led = 1;    //Physical pin 6
int sense = A2; //Physical pin 3
int button = 3; //Physical pin 2
int mosfet = 2; //Physical pin 7

int cutoff = 825;
int voltage = 900;
int index = 0;
int readings[3];

unsigned long cycleStart;
float period = 10000;
int onTime = 0;

#include <Bounce2.h>
Bounce bounce = Bounce();
int longPress = 1000;

byte menu = false;
byte onCt = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  analogReference(INTERNAL);
  
  bounce.attach(button, INPUT);
  bounce.interval(5);
  
  pinMode(led,OUTPUT);
  pinMode(mosfet,OUTPUT);
  pinMode(sense,INPUT);

  cycleStart = millis();
  RegisterBlink();
}

// the loop function runs over and over again forever
void loop() {
  //New period turn relays on if needed
  if((millis()-cycleStart)>period){
    if(onTime>0) digitalWrite(mosfet, HIGH);
    cycleStart = millis();
  }
  
  //Check voltage underload before turning off
  if((millis()-cycleStart)>onTime){
    CheckVoltage();
    delay(5000);
    digitalWrite(mosfet, LOW);
  }
  
  //Check low battery
  if(voltage < cutoff){
    LowBat();
  }
  
  //Check if need to enter menu
  bounce.update();
  if(bounce.fell()){
    if(bounce.previousDuration() > longPress){
      //Enter the power level menu
      menu =  true;
      onCt = 0;
      digitalWrite(mosfet, LOW);
      RegisterBlink();
    }
  }

  //Menu control
  while(menu){
    bounce.update();
    //Botton pressed
    if(bounce.fell()){
      //Set value and exit menu
      if(bounce.previousDuration() > longPress){
        
        //Blink out the reading
        RegisterBlink();
        delay(2000);
        for(byte i=0; i<onCt; i++){
          digitalWrite(led, HIGH);
          delay(500);
          digitalWrite(led, LOW);
          delay(500);
        }

        //Set the values and exit
        onTime = period*onCt/10;
        cycleStart = millis()-2*period;
        menu = false;
      }
      
      //Change the value
      else{
        onCt++;
        //if roll over reset to zero and blink once
        if(onCt > 10){
          onCt = 0;
          digitalWrite(led, HIGH);
          delay(100);
          digitalWrite(led, LOW);
          delay(100);
        }
      }
    }
    
  }
  
  
}

void LowBat(){
  while (true){
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led,LOW);
    delay(1000);
  }
}
void CheckVoltage(){
  voltage = analogRead(sense);
}
void RegisterBlink(){
  for(byte i=0; i<6; i++){
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
  }
}
