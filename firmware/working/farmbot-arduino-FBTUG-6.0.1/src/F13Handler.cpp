
/*
 * F13Handler.cpp
 *
 *  Created on: 2014/07/21
 *      Author: MattLech
 */

#include "F13Handler.h"

static F13Handler *instance;

F13Handler *F13Handler::getInstance()
{
  if (!instance)
  {
    instance = new F13Handler();
  };
  return instance;
};

F13Handler::F13Handler()
{
}

int F13Handler::execute(Command *command)
{

  if (LOGGING)
  {
    Serial.print("R99 HOME Z\r\n");    
    #ifdef BT_Serial2_Use
      Serial2.print("R99 HOME Z\r\n");   //-2018.02.25-CGH
    #endif  
  }

  StepperControl::getInstance()->moveToCoords(0, 0, 0, 0, 0, 0, false, false, true);

  if (LOGGING)
  {
    CurrentState::getInstance()->print();
  }
  return 0;
}

