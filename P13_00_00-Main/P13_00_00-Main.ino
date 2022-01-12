#include <Bounce2.h>
#define Tack 4
#define RelayA 0
#define RelayB 1
#define RelayC 2
#define MenuLed 3
Bounce bounce = Bounce();
int LedState = LOW;
unsigned long TimeStart;
byte Menu = false;
byte RelaySet = false;
byte RelayCt;
byte MenuSel;
int LongPress = 1000;
int ExitPress = 2000;
byte i;


float Period = 10000;
int TimeA = 0;
int TimeB = 0;
int TimeC = 0;

unsigned long CycleStart;


void setup() {
  bounce.attach(Tack, INPUT);
  bounce.interval(5);
  
  pinMode(RelayA, OUTPUT);
  pinMode(RelayB, OUTPUT);
  pinMode(RelayC, OUTPUT);
  pinMode(MenuLed, OUTPUT);

  CycleStart = millis();
  digitalWrite(MenuLed, HIGH);
  delay(500);
  digitalWrite(MenuLed, LOW);
}

void loop() {
  //New period turn relays on if needed
  if((millis()-CycleStart)>Period){
    if(TimeA>0) digitalWrite(RelayA, HIGH);
    if(TimeB>0) digitalWrite(RelayB, HIGH);
    if(TimeC>0) digitalWrite(RelayC, HIGH);
    CycleStart = millis();
  }
  
  if((millis()-CycleStart)>TimeA) digitalWrite(RelayA, LOW);
  if((millis()-CycleStart)>TimeB) digitalWrite(RelayB, LOW);
  if((millis()-CycleStart)>TimeC) digitalWrite(RelayC, LOW);
  
  bounce.update();
  //To get into the menu hold the button for ~2seconds
  if(bounce.fell()){
    if(bounce.previousDuration() > LongPress){
      //Enter the power level menu
      Menu =  true;
      AllOff();
      MenuSel = 0;
    digitalWrite(MenuLed,HIGH);
      digitalWrite(MenuSel, HIGH);
    }
  }
  //Menu to set power levels
  while(Menu){
    bounce.update();
    if(bounce.fell()){
      //Exit Press
      if(bounce.previousDuration() > ExitPress){
        Menu =  false;
    AllOff();
        digitalWrite(MenuLed,LOW);       
      }
      //Select press 
      else if (bounce.previousDuration() > LongPress){
        Notification(MenuSel);
        RelayCt = 0;
        RelaySet=true;
        //Set the power level for MenuSel relay
        while(RelaySet){
          bounce.update();
          if(bounce.fell()){
            //Save power level for MenuSel relay
            if (bounce.previousDuration() > LongPress){
              //Flash to show setting
              Notification(MenuSel);
              delay(2000);
              //Blink power level setting
              for(i=1; i<=RelayCt; i++){
                digitalWrite(MenuSel, HIGH);
                delay(200);
                digitalWrite(MenuSel, LOW);
                delay(200);
              }
              //Save the values
              if(MenuSel==0) TimeA = Period*RelayCt/10;
              if(MenuSel==1) TimeB = Period*RelayCt/10;
              if(MenuSel==2) TimeC = Period*RelayCt/10;
              CycleStart = millis()-2*Period;
              //exit menu
              Menu =  false;
              digitalWrite(MenuLed,LOW);
              RelaySet=false;
            }
            //Increase power level for MenuSel relay
            else{
              RelayCt++;
              if(RelayCt > 10){
                RelayCt=0;
                digitalWrite(MenuSel, HIGH);
                delay(100);
                digitalWrite(MenuSel, LOW);
                delay(100);
              }
            }
          }
        }
      }
      //Short press
      else{
        MenuSel++;
        if (MenuSel>2){
          MenuSel = 0;
        }
        AllOff();
        digitalWrite(MenuSel, HIGH);
      }
    }
  }
  
}


void Notification(byte pin){
  AllOff();
  for(i=0; i<6; i++){
    digitalWrite(pin, HIGH);
    delay(50);
    digitalWrite(pin, LOW);
    delay(50);
  }
}
void AllOff(){
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
  digitalWrite(2, LOW);
}
