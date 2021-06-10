#include <RCSwitch.h>
#include <EEPROM.h> 

RCSwitch mySwitch = RCSwitch();
void Programmer_Mode();
void Relay_Funtions();
void Position_Leds();

#define Push_Erase  16 
#define Push_Save   15 
#define Jamper      14

#define LedInd      10
#define Relay1      9
#define Relay2      8
#define Relay3      7
#define Relay4      6

volatile boolean READ_Jamper;
volatile boolean STATE_Jamper;

int Record_Positions[4];

volatile int position;
volatile int valor;
volatile int count;
volatile int comparison;
volatile int add;

//**************************** setup *********************************/

void setup() 
{
  mySwitch.enableReceive(0);
  for (int i=14; i<17; i++)pinMode(i, INPUT_PULLUP); 
  for (int i=6; i<11; i++)pinMode(i, OUTPUT);
  for (int i=0; i<4; i++)EEPROM.get(i, Record_Positions[i]);
}

//*************************** Loop ***********************************/

void loop() 
{
  READ_Jamper=digitalRead(Jamper);
  Programmer_Mode();  
}

//*********************** Programmer_Mode ****************************/

void Programmer_Mode()
{
  if (STATE_Jamper==0)for (int i = 6; i <= 11; i+=1)digitalWrite(i, LOW);
  STATE_Jamper+=1;
  
  if (READ_Jamper==0)
  {
    digitalWrite(LedInd, LOW);
    
    if (mySwitch.available()) 
    {
      delay(150);
      valor=mySwitch.getReceivedValue();
      count=3; 
      for (int i = 8; i <= 256; i+=2)   
      {
        EEPROM.get(i, comparison);
        count+=1;
        if (count>3)count=0;      
        if (comparison==valor)Relay_Funtions();
      }
      mySwitch.resetAvailable();
    }
  }
  else
  {
    digitalWrite(LedInd, HIGH);
    STATE_Jamper=0;
    Position_Leds();
    //----------------------------- TECLA ERASE --------------------------------/
    static unsigned char StatePush_Eraser = digitalRead(Push_Erase);
    if (StatePush_Eraser != digitalRead(Push_Erase))
    {
      StatePush_Eraser = digitalRead(Push_Erase);
      if (StatePush_Eraser == LOW)
      {
        for (int i=0; i<=256 ; i+=1)
        {
          EEPROM.put(i, 0);
        }
       for (int i = 0; i < 4; i++)
        {
          switch (i)
          {
            case 1:
              Record_Positions[1]=10;
              EEPROM.put(1, Record_Positions[1]);
              break;
            case 2:
              Record_Positions[2]=12;
              EEPROM.put(2, Record_Positions[2]);
              break;
            case 3:
              Record_Positions[3]=14;
              EEPROM.put(3, Record_Positions[3]);
              break;
            default:
              Record_Positions[0]=8;
              EEPROM.put(0, Record_Positions[0]);
              break;
          }
        }
                
        position=0;
        for (int i = 6; i <= 10; i+=1)digitalWrite(i, LOW);
        digitalWrite(LedInd, LOW);
        delay(100);
        digitalWrite(LedInd, HIGH);
        delay(100);
        digitalWrite(LedInd, LOW);
        delay(100); 
      } 
    }
    //-------------------------------- FIN ----------------------------------/

    //----------------------------- TECLA SAVE ------------------------------/
    static unsigned char StatePush_Saver = digitalRead(Push_Save);
    if (StatePush_Saver != digitalRead(Push_Save))
    {
      StatePush_Saver = digitalRead(Push_Save);
      if (StatePush_Saver == LOW)
      {
         position+=1;
         if (position>3)position=0;
      } 
    }
    //-------------------------------- FIN ----------------------------------/
  if (mySwitch.available()) 
    {
      delay(100);
      valor=mySwitch.getReceivedValue();
      switch (position)
      {
        case 1:
          EEPROM.put(Record_Positions[1], valor);
          Record_Positions[1]+=8;
          if (Record_Positions[1]>256)Record_Positions[1]=10;
          EEPROM.put(1, Record_Positions[1]);
          break;
        case 2:
          EEPROM.put(Record_Positions[2], valor);
          Record_Positions[2]+=8;
          if (Record_Positions[2]>256)Record_Positions[2]=12;
          EEPROM.put(2, Record_Positions[2]);
          break;
        case 3:
          EEPROM.put(Record_Positions[3], valor);
          Record_Positions[3]+=8;
          if (Record_Positions[3]>256)Record_Positions[3]=14;
          EEPROM.put(3, Record_Positions[3]);
          break;
        default:
          EEPROM.put(Record_Positions[0], valor);
          Record_Positions[0]+=8;
          if (Record_Positions[0]>256)Record_Positions[0]=8;
          EEPROM.put(0, Record_Positions[0]);
          break;
      }
      
      mySwitch.resetAvailable();
      for (int i = 6; i <= 10; i+=1)digitalWrite(i, LOW);
      delay(200);
    } 
  }  
}

//*********************** Relay_Funtions ****************************/

void Relay_Funtions()
{
  switch (count)
  {
  case 1:
    digitalWrite(Relay4,digitalRead(Relay4) ^ 1);
    break;
  case 2:
    digitalWrite(Relay3,digitalRead(Relay3) ^ 1);
    break;
  case 3:
    digitalWrite(Relay2,digitalRead(Relay2) ^ 1);
    break;
  default:
    digitalWrite(Relay1,digitalRead(Relay1) ^ 1);
    break;
  }
}
//*********************** Position_Leds ****************************/

void Position_Leds()
{
  for (int i = 6; i <= 10; i+=1)digitalWrite(i, LOW);
  switch (position)
  {
  case 1:
    digitalWrite(Relay4,HIGH);
    break;
  case 2:
    digitalWrite(Relay3,HIGH);
    break;
  case 3:
    digitalWrite(Relay2,HIGH);
    break;
  default:
    digitalWrite(Relay1,HIGH);
    break;
  }
}

