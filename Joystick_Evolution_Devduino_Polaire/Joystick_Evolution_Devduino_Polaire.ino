// =============================================================================
//                                PROJECT NAME
// =============================================================================
// Description:
/* This template aims to make your new project developpement easier than
 * starting from scratch.
 * It already includes basics methods so you can just uncomment/copy/paste/delete 
 * what you need or don't need and not waste too much time with syntax errors.
 * Hope it will help!
 */
// =============================================================================
// Date: 2019.03.04
// Author: Coillote
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/deed.fr
// =============================================================================
/*=============================================================================
                                   INCLUDES
==============================================================================*/
//#include <Wire.h>
//#include <SoftwareSerial.h>
#include <devduino.h>
#include "devduinoLogo.h"
#include <math.h>
/*=============================================================================
                                  DEFINITIONS
==============================================================================*/
//#define RED_LED     13
int YgauchePin = 9; //sortie pwm moteur gauche
int YdroitePin = 10; //sortie pwm moteur droite
int DirYgPin  = 8; // sortie direction moteur gauche
int DirYdPin  = 11; // sortie direction moteur droite
int JoyPinX = 0;                 // slider variable connecetd to analog pin 0
int JoyPinY = 1;                 // slider variable connecetd to analog pin 1
int BoutonPin = 13;
/*=============================================================================
                                GLOBAL VARIABLES
==============================================================================*/
//String messageFromUART= "";
int JoyX = 512;                  // variable to read the value from the analog pin 0
int JoyY = 512;                  // variable to read the value from the analog pin 1
bool DirYg;
bool DirYd;
bool Bouton;
long OzoneX, VX, VY, OzoneY, Ygauche, Ydroite, ZeroX, ZeroY, CalcY, CalcX, CalcYgauche, CalcYdroite, CalcXgauche, CalcXdroite, TrimBX, TrimBY, TrimHX, TrimHY;
float Rayon, Alpha;
/*=============================================================================
                                   INSTANCES
==============================================================================*/
//SoftwareSerial UART(10,12);

// =============================================================================
//                                INITIALIZATION
// =============================================================================

void setup() {
  /*============================================================================
                                  DEVDUINO INIT.
  =============================================================================*/
  devduino.begin();
  devduino.attachToIntButton(buttonPushed);
  display.drawSprite(devduinoLogo, 37, 0);
  display.flush();
  delay(200);
  display.clear();

  // ---------------------------------------------------------------------------
  Serial.begin(57600);
  pinMode(DirYgPin, OUTPUT);
  pinMode(DirYdPin, OUTPUT);
  pinMode(BoutonPin, INPUT);
  // faudrait test le PWM 247 Hz ...
  ZeroX = analogRead(JoyPinX);
  delay(10);
  ZeroY = analogRead(JoyPinY);
  OzoneX = 10; // ±10 de chaque coté du 0 X
  OzoneY = 10; // ±10 de chaque coté du 0 Y
  TrimBX = ZeroX - OzoneX;
  TrimHX = ZeroX + OzoneX;
  TrimBY = ZeroY - OzoneY;
  TrimHY = ZeroY + OzoneY; 
  /* ici faire le test des 3 clics pour prendre les valeur de Zero X et Y, 1er test en lecture directe   */
  // ---------------------------------------------------------------------------
  
  /*============================================================================
                                   DIGITAL IN
  =============================================================================*/
   //pinMode(RED_LED, OUTPUT);
   //digitalWrite(RED_LED, HIGH);
  pinMode(BoutonPin, INPUT);
  /*============================================================================
                                   DIGITAL OUT
  =============================================================================*/
  // pinMode(8, INPUT_PULLUP);
  // int buttonState = digitalRead(8);
  pinMode(DirYgPin, OUTPUT);
  pinMode(DirYdPin, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(7, LOW);
  /*============================================================================
                                   ANALOG OUT
  =============================================================================*/
  // pinMode(6, OUTPUT);
  // analogWrite(6, 255);
  /*============================================================================
                                   ANALOG IN
  =============================================================================*/
  // int sensorValue = analogRead(A0);
  // float voltage = sensorValue * (5.0 / 1023.0);
  /*============================================================================
                                CONSOLE/TERMINAL
  =============================================================================*/
  // console.println("... Your string here...");  
  /*============================================================================
                                   DISPLAY
  =============================================================================*/
  // display.print(String("... Your string here..."), 128, 64, &defaultFont);
  //display.setScale(1); // 1 or 2
  /*============================================================================
                                  SCROLL DOWN
  =============================================================================*/
  /*
  for (;;)
  {
    display.verticalScroll(-1, false);
    display.flush();
    delay(100);
  }
  */  
}

// =============================================================================
//                               INFINITE LOOP      
//                          -> -> -> -> -> -> -> ->
//                          |                     |              
//                          <- <- <- <- <- <- <- <-
// =============================================================================
void loop() {
  JoyX = analogRead(JoyPinX);  // reads the value of the variable resistor
  
  delay(10);                     // this small pause is needed between reading analog pins, otherwise we get the same value twice
  
  JoyY = analogRead(JoyPinY);    // reads the value of the variable resistor

  if ( JoyY < TrimBY )  {
      VY = 511 * (JoyY - TrimBY ) / TrimBY;
  } else if ( JoyY > TrimHY ) {
      VY = 511 * (JoyY - TrimHY ) / ( 1023 - TrimHY )  ;
 }
  else {
    VY = 0;  // PWM = 0; Dir = X => Frein moteur.
  }

if ( JoyX > TrimHX ) {
    VX = ( 511 * ( JoyX - TrimHX ) / ( 1023 - TrimHX ) );
  }
else if ( JoyX < TrimBX ) {
    VX = ( 511 * ( JoyX - TrimBX ) / TrimBX );
  }
else {
    VX = 0;  // PWM = 0; Dir = X => Frein moteur.
  }

  Rayon = sqrt((VX*VX)+(VY*VY));
  if (Rayon > OzoneX) { Alpha = atan2(VY,VX);} else { Alpha = 0;}

  if (Rayon < 255) { 
    CalcY = Rayon*sin(Alpha);
    CalcX = Rayon*cos(Alpha);
  } else {
    CalcY = 255*sin(Alpha);
    CalcX = 255*cos(Alpha);
  }

  CalcXgauche = CalcX / sqrt(2);
  CalcXdroite = - CalcX / sqrt(2);

  CalcYgauche = CalcXgauche + CalcY / sqrt(2) ;
  CalcYdroite = CalcXdroite + CalcY / sqrt(2);

  if (CalcYgauche > 0) {
    DirYg = true;
  }else {
    DirYg = false;
  }
  
  if (CalcYdroite > 0) {
    DirYd = true;
  }else {
    DirYd = false;
  }
  
  Ygauche = abs(CalcYgauche);
  Ydroite = abs(CalcYdroite);

if (Ygauche > 255) { Ygauche = 255; }
if (Ydroite > 255) { Ydroite = 255; }

  analogWrite(YgauchePin, Ygauche);
  analogWrite(YdroitePin, Ydroite);
  digitalWrite(DirYgPin, DirYg);
  digitalWrite(DirYdPin, DirYd);

  
  /*============================================================================
                                    DISPLAY
  =============================================================================*/  

display.clear();
  display.drawRectangle(32,0,64,64);
  display.fillCircle(64,31,1);

  display.drawCircle(((JoyX-512)/16)+65,((JoyY-512)/16)+30,1);
  //display.drawSprite(thanksSprite, 32 + 32*cos(I * M_PI /180 ),32 - 32* sin(I* M_PI /180 ));
    //display.verticalScroll(1, false);
  if (DirYg) { display.fillRectangle(14,31,5, Ygauche/11);} else {display.drawRectangle(14,32 - (Ygauche/11) ,5, Ygauche/11);}
  if (DirYd) { display.fillRectangle(110,31,5,Ydroite/11);} else {display.drawRectangle(110,32 - (Ydroite/11) ,5, Ydroite/11);}
  display.print(String("X:"),0,0, &defaultFont);
  display.print(String(JoyX),12,0, &defaultFont);
  display.print(String("Y:"),98,0, &defaultFont);
  display.print(String(JoyY),110,0, &defaultFont);
  
  display.print(String("G:"),0,56, &defaultFont);
  display.print(String(Ygauche),12,56, &defaultFont);
  display.print(String("D:"),98,56, &defaultFont);
  display.print(String(Ydroite),110,56, &defaultFont);
  
  display.flush();
  
   
 //Serial.print("ZX");
  Serial.print(ZeroX);
  Serial.print("\t");
  //Serial.print(" ZY");
  Serial.print(ZeroY);  
  Serial.print("\t");
  //Serial.print("   TBX");
  Serial.print(TrimBX);
  Serial.print("\t");
  //Serial.print(" THX");
  Serial.print(TrimHX);  
  Serial.print("\t");
  //Serial.print(" TBY");
  Serial.print(TrimBY);
  Serial.print("\t");
  //Serial.print(" THY");
  Serial.print(TrimHY);  
  Serial.print("\t");
  //Serial.print("   X=");
  Serial.print(JoyX);
  Serial.print("\t");
  //Serial.print(" Y=");
  Serial.print(JoyY);
  Serial.print("\t");
  //Serial.print(" DG=");
  Serial.print(DirYg);
  Serial.print("\t");
  //Serial.print("   Yg=");
  Serial.print(Ygauche);
  Serial.print("\t");
  //Serial.print("   Yd=");
  Serial.print(Ydroite);
  Serial.print("\t");
  //Serial.print("  DD=");
  Serial.print(DirYd);
  Serial.print("\t");
  //Serial.print("  CalcY= ");
  Serial.print(CalcY);
  Serial.print("\t");
  //Serial.print("  CXGauche=");
  Serial.print(CalcXgauche);
  Serial.print("\t");
  //Serial.print("  CXDroite");
  Serial.println(CalcXdroite);



  // ---------------------------------------------------------------------------
  
  delay(1);
  /*============================================================================
                                 UART READING
  =============================================================================*/  
  /*    
        
   while(UART.available()) 
   {
    delay(3);
     char c = UART.read();
    messageFromUART+= c;
   }
   if (messageFromUART.length() >0) 
   {  
    console.println(messageFromUART);
    if ((messageFromUART.indexOf("D00") != -1)) 
    {
     if ((messageFromUART.indexOf("ON") != -1)) 
     {
      digitalWrite(0,HIGH);      
     }
    }
    messageFromUART="";
   }   
  /*============================================================================
                               TEMPERATURE READING
  =============================================================================*/
  /*
  float currentTemperature = temperature.read();
 
  display.clear();  
  display.setScale(2);
  display.print(String(currentTemperature) + " C", 10, 10, &defaultFont);
  display.print(String("o"), 40, 12, &defaultFont);  
  display.flush();
  display.setScale(1);

  */
  /*============================================================================
                                  RTC READING
  =============================================================================*/
  /*
  
  DateTime currentClock = rtc.now();
  uint8_t hour = currentClock.getHour();
  uint8_t minute = currentClock.getMinute();
  uint8_t second = currentClock.getSecond();
  
  char dateBuffer[10];
  sprintf(dateBuffer, "%04d-%02d-%02d", currentClock.getYear(), currentClock.getMonth(), currentClock.getDay());
  char timeBuffer[9];
  sprintf(timeBuffer, "%02dh%02dm%02ds", currentClock.getHour(), currentClock.getMinute(), currentClock.getSecond());
  display.clear();
  display.print(String(timeBuffer), 30, 30, &defaultFont);
  display.flush();
  
  */    
}


// =============================================================================
//                                INTERRUPTION                        
//                                     ||          
//                                     ||          
//                               O-----------O    
//                          ------O         0-------                       
// =============================================================================
void buttonPushed()
{
  /*============================================================================
                                DEBOUCING FILTER
  =============================================================================*/
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 200)
  {
    // ---------------------------------------------------------------------------
    // DO YOUR INTERRUPTION HERE...   
    // ---------------------------------------------------------------------------
  }   
  last_interrupt_time = interrupt_time;
}
