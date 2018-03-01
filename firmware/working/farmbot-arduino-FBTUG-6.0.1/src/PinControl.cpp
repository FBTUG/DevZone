
#include "PinControl.h"

static PinControl *instance;

PinControl *PinControl::getInstance()
{
  if (!instance){
    instance = new PinControl();
  };
  return instance;
};

PinControl::PinControl()
{
  for (int pinNr = 1; pinNr <= 52; pinNr++){
    pinWritten[0][pinNr] = false;
    pinWritten[1][pinNr] = false;
  }

}

int PinControl::setMode(int pinNr, int mode)
{
  pinMode(pinNr, mode);
  return 0;
}

int PinControl::writeValue(int pinNr, int value, int mode)
{
  if (pinNr > 0 && pinNr <= 52 && (mode == 0 || mode == 1))
  {
    pinWritten[mode][pinNr] = true;

    if (mode == 0)
    {
      digitalWrite(pinNr, value);
      return 0;
    }
    if (mode == 1)
    {
      analogWrite(pinNr, value);
      return 0;
    }
  }
  return 1;
}

// Set all pins that were once used for writing to zero
void PinControl::resetPinsUsed()
{
  for (int pinNr = 1; pinNr <= 52; pinNr++)
  {
    if (pinWritten[0][pinNr]){
      Serial.print("R99"); Serial.print(" resetting digital pin"); Serial.print(pinNr); Serial.print("\r\n");
      #ifdef BT_Serial2_Use
        Serial2.print("R99"); Serial2.print(" resetting digital pin"); Serial2.print(pinNr); Serial2.print("\r\n");    //-2018.02.25-CGH 
      #endif
      
      digitalWrite(pinNr, false);
      readValue(pinNr, 0);
      pinWritten[0][pinNr] = false;
    }
    if (pinWritten[1][pinNr]){
      Serial.print("R99"); Serial.print(" resetting analog pin"); Serial.print(pinNr); Serial.print("\r\n");
      #ifdef BT_Serial2_Use
        Serial2.print("R99"); Serial2.print(" resetting analog pin"); Serial2.print(pinNr); Serial2.print("\r\n");    //-2018.02.25-CGH 
      #endif  

      analogWrite(pinNr, 0);
      readValue(pinNr, 1);
      pinWritten[1][pinNr] = false;
    }
  }
}

int PinControl::readValue(int pinNr, int mode)
{

  int value = 0;

  if (mode == 0)
  {
    if (digitalRead(pinNr) == 1)
    {
      value = 1;
    }
  }
  if (mode == 1)
  {
    value = analogRead(pinNr);
  }

  if (mode == 0 || mode == 1)
  {

    Serial.print("R41"); Serial.print(" "); Serial.print("P"); Serial.print(pinNr); Serial.print(" "); Serial.print("V"); Serial.print(value);
    //Serial.print("\r\n");
    #ifdef BT_Serial2_Use
      Serial2.print("R41"); Serial2.print(" "); Serial2.print("P"); Serial2.print(pinNr); Serial2.print(" "); Serial2.print("V"); Serial2.print(value);   //-2018.02.25-CGH 
      //Serial2.print("\r\n");    //-2018.02.25-CGH
    #endif
    CurrentState::getInstance()->printQAndNewLine();

    return 0;
  }
  else
  {
    return 1;
  }
}

int PinControl::writePulse(int pinNr, int valueOne, int valueTwo, long time, int mode)
{
  writeValue(pinNr, valueOne, mode);
  delay(time);
  writeValue(pinNr, valueTwo, mode);
  return 0;
}

