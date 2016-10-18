/*
 * Software for home brew beer at home using LCD shild, ds18b20 temperature sensor
 * Libs :
 * https://github.com/milesburton/Arduino-Temperature-Control-Library
 *
 */

#include <OneWire.h>
#include <DallasTemperature.h>
#define  ONE_WIRE_BUS 1 // temperature sensor pin...
#include <LiquidCrystal.h>

#include <EEPROM.h>


LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const long intervalBuzzerAlarm = 500;
int keypad_pin = A0;
int keypad_value = 0;
int keypad_value_old = 0;

byte eeprom_time_positions[4]={1,3,5,7};
byte eeprom_temperature_positions[4]={0,2,4,6};

char btn_push;

byte mainMenuPage = 1;
byte mainMenuPageOld = 1;
byte mainMenuTotal = 7;

void setup()
{
  sensors.begin();
  for(int i =0; i<=255;i++){
    if(EEPROM.read(i)==255){
    EEPROM.write(i,0);
    }

  }
    lcd.begin(16,2);  //Initialize a 2x16 type LCD

    MainMenuDisplay();
    delay(1000);
}
void loop()
{
    lcd.noBlink();
    btn_push = ReadKeypad();

    MainMenuBtn();

    if(btn_push == 'S')//enter selected menu
    {
        WaitBtnRelease();
        switch (mainMenuPage)
        {
            case 1:
              Rampa1();
              break;
            case 2:
              Rampa2();
              break;
            case 3:
              Rampa3();
              break;
            case 4:
              Rampa4();
              break;
            case 5:
              menuReset();
              break;
            case 6:
              start();
              break;
            case 7:
              monitor();
              break;
        }

          MainMenuDisplay();
          WaitBtnRelease();
    }



    delay(10);

}//--------------- End of loop() loop ---------------------
void Rampa1()
{
  insideMenu(1);
}
void Rampa2()
{
  insideMenu(2);
}
void Rampa3()
{
  insideMenu(3);
}
void Rampa4()
{
  insideMenu(4);
}
void menuReset()
{
  reset();
}

void MainMenuDisplay()
{
    lcd.clear();
    lcd.setCursor(0,0);
    switch (mainMenuPage)
    {
        case 1:
          lcd.print("Rampa 1");
          break;
        case 2:
          lcd.print("Rampa 2");
          break;
        case 3:
          lcd.print("Rampa 3");
          break;
        case 4:
          lcd.print("Rampa 4");
          break;
        case 5:
          lcd.print("Reset");
          break;
        case 6:
          lcd.print("Start");
          break;
        case 7:
          lcd.print("Monitor");
          break;
    }
}

void MainMenuBtn()
{
    WaitBtnRelease();
    if(btn_push == 'U')
    {
        mainMenuPage++;
        if(mainMenuPage > mainMenuTotal)
          mainMenuPage = 1;
    }
    else if(btn_push == 'D')
    {
        mainMenuPage--;
        if(mainMenuPage == 0)
          mainMenuPage = mainMenuTotal;
    }

    if(mainMenuPage != mainMenuPageOld) //only update display when page change
    {
        MainMenuDisplay();
        mainMenuPageOld = mainMenuPage;
    }
}

char ReadKeypad()
{
  /* Keypad button analog Value
  no button pressed 1023
  select  741
  left    503
  down    326
  up      142
  right   0
  */
  keypad_value = analogRead(keypad_pin);

  if(keypad_value < 100)
    return 'R';
  else if(keypad_value < 200)
    return 'U';
  else if(keypad_value < 400)
    return 'D';
  else if(keypad_value < 600)
    return 'L';
  else if(keypad_value < 800)
    return 'S';
  else
    return 'N';

}

void WaitBtnRelease()
{
    while( analogRead(keypad_pin) < 800){}
}
byte* getTime(){
  long fullSeconds   = (long) millis()/1000;
  long fullMinutes   = fullSeconds / 60;
  byte hours         = fullMinutes / 60;
  byte minutes       = fullMinutes % 60;
  byte seconds       = fullSeconds % 60;
  byte ret[3]        = {hours,minutes,seconds};

  return ret;
}

void writeProcessTime(byte hours,byte minutes,byte seconds){
  EEPROM.write(0,hours);
  EEPROM.write(1,minutes);
  EEPROM.write(2,seconds);
}

void insideMenu(int menuNumber){



    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("R" + String(menuNumber) +", Temp:");

    lcd.setCursor(0,1);
    lcd.print("* Tempo:");
    lcd.setCursor(0,1);



    int temperature = EEPROM.read(eeprom_temperature_positions[menuNumber-1]);
    int timing      = EEPROM.read(eeprom_time_positions[menuNumber-1]);
    int idr1        = 0;
    while(ReadKeypad()!= 'L')
    {
      lcd.blink();


        if(idr1==0){
           lcd.setCursor(10,0);
            if(ReadKeypad()== 'U'){
              delay(100);
              temperature ++;

            }

            if(ReadKeypad()== 'D'){
              delay(100);
              temperature --;
            }


            lcd.print(temperature);
            lcd.print("*C");
        }
        if(idr1==1){
            if(ReadKeypad()== 'U'){
              delay(100);
              timing ++;

            }

            if(ReadKeypad()== 'D'){
              delay(100);
              timing --;
            }
            lcd.setCursor(8,1);

            lcd.print(timing);
            lcd.print("m");
        }
        if(ReadKeypad() == 'R'){
          delay(500);
          idr1++;
        }

        if(idr1!=1&&idr1!=0) idr1 ==0;

    }

    EEPROM.write(eeprom_temperature_positions[menuNumber-1],temperature);
    EEPROM.write(eeprom_time_positions[menuNumber-1],timing);
}
void reset(){
    for(int i =0; i<=20;i++){
    EEPROM.write(i,0);

    }

     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("     Rampas     ");
     lcd.setCursor(0,1);
     lcd.print("Reiniciadas     ");
      while(ReadKeypad()!= 'L')
    {
    }
}

void start(){
  lcd.clear();
  lcd.setCursor(0,0);
  int secondsI = millis()/1000;

  while(ReadKeypad()!= 'L')
    {

      int seconds = (millis()/1000) - secondsI;


        //lcd.clear();

        getTemperature();
        lcd.setCursor(0, 1);

         lcd.print(seconds/60);
    }
}

void monitor(){

  lcd.clear();
  int index =0;

   while(ReadKeypad()!= 'L'){
     if(index > 3){
       index = 0;
     }
     getTemperature();
     lcd.setCursor(0,1);

     lcd.print("R");
     lcd.print(index+1);

     lcd.print("T:");
     lcd.print(readErpromTemperature(index));
     lcd.print("*C|R:");
     lcd.print(readErpromTiming(index));
     lcd.print("m");
     if(ReadKeypad()== 'U'){
       delay(100);
       index ++;
     }

     if(ReadKeypad()== 'D'){
       delay(100);
       index --;
     }

   }
 }


void getTemperature(){
  sensors.requestTemperatures();
  lcd.setCursor(0, 0);
  lcd.print(sensors.getTempCByIndex(0));
  lcd.print(" *C");
}

byte readErpromTemperature(byte index){
  return  EEPROM.read(eeprom_temperature_positions[index]);
}

byte readErpromTiming(byte index){
  return  EEPROM.read(eeprom_time_positions[index]);
}
