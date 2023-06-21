#include "userdef.h"

unsigned char select = 0;

void setup(){
  Serial.begin(115200);
  configure_relay();
  configure_lcd();
  configure_motor();
  configure_hr04();
  delay(1000);
}

void MenuDisplay(Menu *menu,unsigned char select){
  lcd.setCursor(0,0);
  lcd.print(menu->List1);
  lcd.setCursor(0,1);
  lcd.print(menu->List2);
  lcd.setCursor(0,select);
  lcd.print(">");
}

void MenuDisplay(Menu *menu){
  lcd.setCursor(0,0);
  lcd.print(menu->List1);
  lcd.setCursor(0,1);
  lcd.print(menu->List2);
}

void loop(){ 
  bool Exit1 = true;
  bool Exit = true;
  Menu *menu;
  menu = &Manual;
  switch(change){ 
    case 1:
      menu = &Manual;
      while(Exit){
        char customKey = customKeypad.getKey();  
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          MenuDisplay(menu,select);
        }
        if(customKey){ 
          switch(customKey){
            case '2':
              if(select == 0){
                menu =(menu->down == NULL)?menu:menu->down;
                select = 1;
              }
              else if(select == 1){
                select -= 1;
              }
              break;
            case '4':
              menu = (menu->pre == NULL)? menu:menu->pre;
              select = 0;
              Serial.println(customKey);
              break;
            case '5':
              switch(select){
                case 0:
                if(menu->MenuID == Motor2){
                  if(menu->On_Device != NULL)menu->On_Device(menu->MenuID,Reward);
                }
                else{
                  if(menu->On_Device != NULL)menu->On_Device(menu->MenuID,On);
                }
                break;
                case 1:
                if(menu->MenuID == Motor2){
                  if(menu->On_Device != NULL)menu->On_Device(menu->MenuID,Forward);
                }
                else{
                  if(menu->Off_Device != NULL)menu->Off_Device(menu->MenuID,Off);
                }
                break;
              }
              Serial.println(customKey);
              break;
            case '6':
              switch(select){
                case 0:
                  menu = (menu->Menu1 == NULL)? menu:menu->Menu1;
                break;
                case 1:
                  menu = (menu->Menu2 == NULL)? menu:menu->Menu2;
                break;
              }
              select = 0;
              Serial.println(customKey);
              break;
            case '8':
              if(select == 1){
                menu =(menu->down == NULL)?menu:menu->down;
                select = 0;
              }
              else if(select == 0){
                select += 1;
              }
              Serial.println(customKey);
              break;
            case '9':
            break;
            case 'A':
              menu =  &Motor;
              break;
            case 'B':
              menu = &Relay;
              break;
            case '*': 
              select = 0;
              Serial.println(customKey);
              Exit = false;
              break;
            default:
            break;
          }
      }
  }
      change = 2;
      break;
    case 2:
      menu = &AutoMenu;
      MenuDisplay(menu);
      while(Exit1){
        menu->Get_Status_Device();
        Sensor();
        Sensor1();
        char customKey = customKeypad.getKey();  
        if(customKey){
          Serial.println(customKey);
          if(customKey == '*') Exit1 = false;
        }
      }
      change = 1;
      break;
  }
}
