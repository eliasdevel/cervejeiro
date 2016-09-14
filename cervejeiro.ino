#include <OneWire.h>
#include <DallasTemperature.h>


// Programa : Teste LCD 16x2 com Keypad
// Autor : Arduino e Cia

/*
 * Definition temperatures in EEPROM
 * Addr 0 to 2 will stay the time of te process
 * Addr 21 to 40 the hill's times
 * Addr 41 at 60 the hill's temperatures..
 * 
 */

#define MAX_DS1820_SENSORS 2
#include <LiquidCrystal.h>  
#include <EEPROM.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  


 OneWire ds(3);

 
 
int keypad_pin = A0;
int keypad_value = 0;
int keypad_value_old = 0;
 
char btn_push;
 
byte mainMenuPage = 1;
byte mainMenuPageOld = 1;
byte mainMenuTotal = 6;
 
void setup()
{
  //sensors.begin();
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
  byte ret[3] = {hours,minutes,seconds};
  
  return ret;
}

void writeProcessTime(byte hours,byte minutes,byte seconds){
  EEPROM.write(0,hours);
  EEPROM.write(1,minutes);
  EEPROM.write(2,seconds);
}

void insideMenu(int menuNumber){
  byte menus[4]={1,3,5,7}   ;
  
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("R" + String(menuNumber) +", Temp:");
    int mnbr = menus[--menuNumber];
    lcd.setCursor(0,1);
    lcd.print("* Tempo:");
    lcd.setCursor(0,1);
    
    
    
    int temperature = EEPROM.read(mnbr -1);
    int timing = EEPROM.read(mnbr);
    int idr1 = 0;
    while(ReadKeypad()!= 'L')
    {
      lcd.blink();
       
        
        if(idr1==0){
            if(ReadKeypad()== 'U'){
              delay(100);
              temperature ++;
              
            }
            
            if(ReadKeypad()== 'D'){
              delay(100);
              temperature --;
            }
            lcd.setCursor(10,0);
            
            lcd.print(temperature);
            lcd.print("gC");
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
        
        if(idr1>1) idr1 ==0;
       
    }
    
    EEPROM.write(mnbr-1,temperature);
    EEPROM.write(mnbr,timing);
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
  
  
  while(ReadKeypad()!= 'L')
    {
      lcd.clear();
      
       getTemperature();
      delay(500); 
    } 
}


void getTemperature(){


byte addr[MAX_DS1820_SENSORS][8];

  lcd.setCursor(0,0);
  lcd.print("DS1820 Test");
  if (!ds.search(addr[0])) 
  {
    lcd.setCursor(0,0);
    lcd.print("No more addresses.");
    ds.reset_search();
    delay(250);
    return;
  }
  if ( !ds.search(addr[1])) 
  {
    lcd.setCursor(0,0);
    lcd.print("No more addresses.");
    ds.reset_search();
    delay(250);
    return;
  }

int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
char buf[20];

  byte i, sensor;
  byte present = 0;
  byte data[12];

  for (sensor=0;sensor<MAX_DS1820_SENSORS;sensor++)
  {
    if ( OneWire::crc8( addr[sensor], 7) != addr[sensor][7]) 
    {
      lcd.setCursor(0,0);
      lcd.print("CRC is not valid");
      return;
    }

    if ( addr[sensor][0] != 0x10) 
    {
      lcd.setCursor(0,0);
      lcd.print("Device is not a DS18S20 family device.");
      return;
    }

    ds.reset();
    ds.select(addr[sensor]);
    ds.write(0x44,1);         // start conversion, with parasite power on at the end

    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = ds.reset();
    ds.select(addr[sensor]);    
    ds.write(0xBE);         // Read Scratchpad

    for ( i = 0; i < 9; i++) 
    {           // we need 9 bytes
      data[i] = ds.read();
    }

    LowByte = data[0];
    HighByte = data[1];
    TReading = (HighByte << 8) + LowByte;
    SignBit = TReading & 0x8000;  // test most sig bit
    if (SignBit) // negative
    {
      TReading = (TReading ^ 0xffff) + 1; // 2's comp
    }
    Tc_100 = (TReading*100/2);    

    Whole = Tc_100 / 100;  // separate off the whole and fractional portions
    Fract = Tc_100 % 100;

    sprintf(buf, "%d:%c%d.%d\337C     ",sensor,SignBit ? '-' : '+', Whole, Fract < 10 ? 0 : Fract);

    lcd.setCursor(0,0);
    lcd.print(buf);
  
  }

 
}
