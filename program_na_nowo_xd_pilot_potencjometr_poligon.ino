/***********************Circuit Connections*******************
      Digital Pot  |   Arduino UNO R3    |
          CS       |     D10 (SS)        |
          SI       |     D11 (MOSI)      |
          CLK      |     D13 (SCK)       |
      VCC,PA0,PA1  |       VCC           |
      GND,PB0,PB1  |       GND           |
          W0       |                     |       CH1+
          W1       |                     |       CH2+
 ************************************************************/

#include <SPI.h>
#include <math.h>
#include <IRremote.h>
IRrecv irrecv(A4);
decode_results results;

#define PinA 2
#define PinB 3
#define PinC 8

//potentiometer select byte
const int POT0_SEL = 0x11;
const int POT1_SEL = 0x12;
const int BOTH_POT_SEL = 0x13;
const int POT0_SHUTDOWN = 0x21;
const int POT1_SHUTDOWN = 0x22;
const int BOTH_POT_SHUTDOWN = 0x23;
int POT0_Dn = 128;
int POT1_Dn = 128;
int BOTH_POT_Dn = 128;
void DigitalPotTransfer(int cmd, int value);

const int CS_PIN = 10;

unsigned long time = 0;
long ilosc_impulsow = 0;
long mute;
int wlaczanie = 0;
long licznik = 0;
void setup()
{
  Serial.begin(9600);

  irrecv.enableIRIn();
  IRrecv irrecv(A4);

  pinMode(CS_PIN, OUTPUT);
  SPI.begin();

  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  pinMode(PinA, INPUT);
  pinMode(PinB, INPUT);
  pinMode(PinC, INPUT);
  attachInterrupt(0, blinkA, LOW);
  attachInterrupt(1, blinkB, LOW);
  time = millis();
}

void loop()
{

    Serial.print("Ilość impulsów: ");
    Serial.println(ilosc_impulsow);
    Serial.print("Stan: ");
    Serial.println(wlaczanie);
    Serial.print("Procent: ");
    Serial.print(ilosc_impulsow * 0.3937);
    Serial.println("%");
    Serial.println("--------------------------------");

  DigitalPotWrite1(POT1_SEL, ilosc_impulsow);
  DigitalPotWrite0(POT0_SEL, ilosc_impulsow);

  if (wlaczanie == 1) {
    digitalWrite(A2, HIGH);
  } else {
    digitalWrite(A2, LOW);
  }
  if (digitalRead(8) == LOW) {
    if (wlaczanie == 1) {
      wlaczanie = 0;
      ilosc_impulsow = 0;
    } else {
      wlaczanie = 1;
      ilosc_impulsow = 10;
    }
    delay(300);
  }

  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0x81818877:
        if (wlaczanie == 1) {
          ilosc_impulsow = ilosc_impulsow + 20 ;
          if (ilosc_impulsow > 254) {
            ilosc_impulsow = 254;
          }
        }
        break;

      case 0x81819867:
        if (wlaczanie > 0) {
          ilosc_impulsow = ilosc_impulsow - 20 ;
          if (ilosc_impulsow < 0) {
            ilosc_impulsow = 0;
          }
        }
        break;

      case 0x8181906F:
        if (wlaczanie == 1) {
          if (ilosc_impulsow == 1) {
            ilosc_impulsow = mute;
            mute = 10;
          } else {
            mute = ilosc_impulsow;
            ilosc_impulsow = 1;
          }
        }
        break;

      case 0x8181807F:
        if (wlaczanie == 0) {
          wlaczanie = 1;
          ilosc_impulsow = 10;
        } else {
          wlaczanie = 0;
          ilosc_impulsow = 0;
        }
        break;
    }
    irrecv.resume();
  }

  while (licznik != ilosc_impulsow)
  {

    licznik = ilosc_impulsow;
    if (wlaczanie == 1) {
      digitalWrite(A0, HIGH);
    } else {
      digitalWrite(A0, LOW);
    }
    if (ilosc_impulsow > 50) {
      digitalWrite(A1, HIGH);
    } else {
      digitalWrite(A1, LOW);
    }
    if (ilosc_impulsow > 100) {
      digitalWrite(4, HIGH);
    } else {
      digitalWrite(4, LOW);
    }
    if (ilosc_impulsow > 150) {
      digitalWrite(5, HIGH);
    } else {
      digitalWrite(5, LOW);
    }
    if (ilosc_impulsow > 200) {
      digitalWrite(6, HIGH);
    } else {
      digitalWrite(6, LOW);
    }
    if (ilosc_impulsow > 250) {
      digitalWrite(7, HIGH);
    } else {
      digitalWrite(7, LOW);
    }
  }
}

void DigitalPotWrite1(int cmd, int val)
{
  // constrain input value within 0 - 255
  val = constrain(val, 0, 255);
  // set the CS pin to low to select the chip:
  digitalWrite(CS_PIN, LOW);
  // send the command and value via SPI:
  SPI.transfer(cmd);
  SPI.transfer(val);
  // Set the CS pin high to execute the command:
  digitalWrite(CS_PIN, HIGH);
}

void DigitalPotWrite0(int cmd, int val)
{
  // constrain input value within 0 - 255
  val = constrain(val, 0, 255);
  // set the CS pin to low to select the chip:
  digitalWrite(CS_PIN, LOW);
  // send the command and value via SPI:
  SPI.transfer(cmd);
  SPI.transfer(val);
  // Set the CS pin high to execute the command:
  digitalWrite(CS_PIN, HIGH);
}

void blinkA()
{
  if (wlaczanie == 1) {
    if ((millis() - time) > 3)
      ilosc_impulsow = ilosc_impulsow - 6;
    if (ilosc_impulsow < 1) {
      ilosc_impulsow = 1;
    }
    time = millis();
  }
}

void blinkB()
{
  if (wlaczanie == 1) {
    if ((millis() - time) > 3)
      ilosc_impulsow = ilosc_impulsow + 6 ;
    if (ilosc_impulsow > 255) {
      ilosc_impulsow = 255;
    }
    time = millis();
  }
}
