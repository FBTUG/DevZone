
/*
 * F12Handler.cpp
 *
 *  Created on: 2014/07/21
 *      Author: MattLech
 */

#include "F12Handler.h"

static F12Handler *instance;

F12Handler *F12Handler::getInstance()
{
  if (!instance)
  {
    instance = new F12Handler();
  };
  return instance;
};

F12Handler::F12Handler()
{
}

int F12Handler::execute(Command *command)
{

  if (LOGGING){
    Serial.print("R99 HOME Y\r\n");
    #ifdef BT_Serial2_Use
      Serial2.print("R99 HOME Y\r\n");   //-2018.02.25-CGH
    #endif
  }

  StepperControl::getInstance()->moveToCoords(0, 0, 0, 0, 0, 0, false, true, false);

  if (LOGGING)
  {
    CurrentState::getInstance()->print();
  }
  return 0;
}

