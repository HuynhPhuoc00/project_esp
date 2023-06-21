#include <Arduino.h>
#include "sys/_types.h"
#include "esp32-hal-adc.h"
#include "esp32-hal-gpio.h"
#include "HardwareSerial.h"
#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

#define On        1
#define Off       0
#define Forward   3
#define Reward    4

#define RL_1      25
#define RL_2      33
#define Coi       34
#define Motor1    26 
#define Motor2    27
#define MQ_6      A0
#define trig      32    
#define echo      35 

uint16_t thread =  600;

byte status_relay = LOW;

unsigned char device_arr[] = {RL_1,RL_2,Motor1};

uint8_t change = 1; 

unsigned long previousMillis = 0;
unsigned long interval = 1000;

const byte ROWS = 4;
const byte COLS = 4; 
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 0, 4, 16}; 
byte colPins[COLS] = {17, 5, 18, 19};
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void configure_motor(){
  pinMode(Motor1, OUTPUT);
  pinMode(Motor2, OUTPUT);
}

void configure_relay(){
  pinMode(RL_1, OUTPUT);
  pinMode(RL_2, OUTPUT);
  pinMode(Coi,OUTPUT);
}

void configure_lcd(){
  lcd.init();
  lcd.backlight();
}

void configure_hr04(){
  pinMode(trig,OUTPUT); 
  pinMode(echo,INPUT); 
}

void get_status_device(){
    if(digitalRead(RL_1)){
      lcd.setCursor(0,1);
      lcd.print(" On | ");
    }else{
      lcd.setCursor(0,1);
      lcd.print(" Of | ");
    }
    if(digitalRead(RL_2)){
      lcd.setCursor(5,1);
      lcd.print(" On | ");
    }else{
      lcd.setCursor(5,1);
      lcd.print(" Of | ");
    }
    if(digitalRead(Motor1) || digitalRead(Motor2) == 1){
      lcd.setCursor(10,1);
      lcd.print(" On ");
    }else{
      lcd.setCursor(10,1);
      lcd.print(" Of ");
    }
}

void device_control(char device, char Status){
  switch (device) {
    case RL_1:
      if(Status) digitalWrite(RL_1,HIGH);
      else       digitalWrite(RL_1,LOW);
      break;
    case RL_2:
      if(Status) digitalWrite(RL_2, HIGH);
      else       digitalWrite(RL_2, LOW);
      break;
    case Coi:
      if(Status) digitalWrite(Coi, HIGH);
      else       digitalWrite(Coi, LOW);
      break;
    case Motor1:
      if(Status){
        digitalWrite(Motor1,HIGH);
        digitalWrite(Motor2,LOW);
      }
      else if(Status == 0){
        digitalWrite(Motor1, LOW);
        digitalWrite(Motor2, LOW);
      }
    case Motor2:
      if(Status == Forward){
        digitalWrite(Motor1,HIGH);
        digitalWrite(Motor2,LOW);
      }
      else if(Status == Reward){ 
        digitalWrite(Motor1, LOW);
        digitalWrite(Motor2, HIGH);
      }
  }
}

int value_mq6;

void Sensor(){
  value_mq6 = analogRead(MQ_6);
  delay(50);
  if(value_mq6 >= thread){
    device_control(RL_1,Off);
    device_control(RL_2,Off);
    device_control(Motor1,Off);
  }
  Serial.println(value_mq6);
}

int distance; 

void Sensor1(){
  Serial.println(distance);
  unsigned long duration; 
  digitalWrite(trig,0);   
  delayMicroseconds(2);
  digitalWrite(trig,1);   
  delayMicroseconds(5);  
  digitalWrite(trig,0);  
  duration = pulseIn(echo,HIGH);  
  distance = int(duration/2/29.412);
  delay(50);
  if(distance <= 10 && distance >=5 ){
    if(digitalRead(Motor1) || digitalRead(Motor2) == 1){
      device_control(Motor1, 0);
      device_control(Motor2, 0);
    }
  }
  Serial.print(distance);
}

typedef struct menu{
  char MenuID;
  char List1[17];struct menu *Menu1;void(*On_Device)(char,char);
  char List2[17];struct menu *Menu2;void(*Off_Device)(char,char);
  void(*Get_Status_Device)();
  struct menu* down;
  struct menu* pre;
}Menu;

extern Menu AutoMenu,Manual,Relay,Motor,Motor11,Relay1,Relay2;

Menu AutoMenu ={
  NULL,
  " R1 | R2 | MTR",NULL,NULL,
  "              ",NULL,NULL,
  &get_status_device,
  NULL,
  NULL,
};

Menu Manual ={
  NULL,
  "   RELAY      ",&Relay,NULL,
  "   MOTOR      ",&Motor,NULL,
  NULL,
  NULL,
  NULL,
};

Menu Relay ={
  NULL,
  "   RELAY  1   ",&Relay1,NULL,
  "   RELAY  2   ",&Relay2,NULL,
  NULL,
  NULL,
  &Manual,
};

Menu Relay1 ={
  RL_1,
  "   ON         ",NULL,&device_control,
  "   OFF        ",NULL,&device_control,
  NULL,
  NULL,
  &Relay,
};

Menu Relay2 ={
  RL_2,
  "   ON         ",NULL,&device_control,
  "   OFF        ",NULL,&device_control,
  NULL,
  NULL,
  &Relay,
};

Menu Motor ={
  Motor1,
  "   ON    (MOR)",NULL,&device_control,
  "   OFF        ",NULL,&device_control,
  NULL,
  &Motor11,
  &Manual,
};

Menu Motor11 ={
  Motor2,
  "   OPEN   (*)",NULL,&device_control,
  "   CLOSE  (#)",NULL,&device_control,
  NULL,
  &Motor,
  &Manual,
};

Menu *menu;





