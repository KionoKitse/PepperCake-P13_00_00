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

int led = 4;    //Physical pin 3
int button = 3; //Physical pin 2
int mosfet = 0; //Physical pin 5


unsigned long cycleStart;
float period = 10000;
int onTime = 0;

#include <Bounce2.h>
Bounce bounce = Bounce();
int longPress = 1000;
byte buttonStart = false;
long buttonTime = 0;
byte menu = false;
byte onCt = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  bounce.attach(button, INPUT);
  bounce.interval(5);
  
  pinMode(led,OUTPUT);
  pinMode(mosfet,OUTPUT);

  cycleStart = millis();
  RegisterBlink();
}

void loop() {
  //Do this if onTime is set
  if(onTime>0){
    //Do this if it is time to turn on the mosfet (start of the period)
    if((millis()-cycleStart)>period){
      digitalWrite(mosfet,HIGH);
      analogWrite(led, HIGH);
      cycleStart = millis();
    }
    //Do this if it is time to turn off the mosfet
    if((millis()-cycleStart)>onTime){
      digitalWrite(mosfet, LOW);
      digitalWrite(led,LOW);
    }
  }
  
  //Check if need to enter menu
  bounce.update();

  //Conditions to enter the menu
  if(bounce.fell())buttonStart = false;
  if(bounce.rose()){
    buttonStart = true;
    buttonTime = millis();
  }
  
  if(buttonStart){
    if((millis()-buttonTime) > longPress){
      RegisterBlink();
      digitalWrite(led,HIGH);
      menu =  true;
      onCt = 0;
      digitalWrite(mosfet, LOW);
      buttonStart = false;
      //Wait for button release before moving on
      while(true){
        bounce.update();
        if(bounce.fell()) break;
      }
    }
  }

  //Menu process
  while(menu){
    bounce.update();
    if(bounce.rose()){
      buttonStart = true;
      buttonTime = millis();
    }
    //Exit condition
    if(buttonStart){
      if((millis()-buttonTime) > longPress){
        //Blink out the reading
        RegisterBlink();
        delay(2000);
        for(byte i=0; i<onCt; i++){
          digitalWrite(led, HIGH);
          delay(200);
          digitalWrite(led, LOW);
          delay(200);
        }
        delay(500);
        
        menu =  false;
        onTime = period*onCt/10;
        buttonStart = false;
      }
    }
    //Increase ct case
    if(bounce.fell()){
      buttonStart = false;
      onCt++;
      //if roll over reset to zero and blink once
      if(onCt > 10){
        onCt = 0;
        digitalWrite(led, LOW);
        delay(100);
        digitalWrite(led, HIGH);
        delay(100);
      }
    }
  }
  
}

void RegisterBlink(){
  for(byte i=0; i<6; i++){
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
  }
}
