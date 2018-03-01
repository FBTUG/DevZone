/*
 * F20Handler.cpp
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 */

#include "F20Handler.h"

static F20Handler *instance;

F20Handler *F20Handler::getInstance()
{
  if (!instance)
  {
    instance = new F20Handler();
  };
  return instance;
};

F20Handler::F20Handler()
{
}

int F20Handler::execute(Command *command)
{

  ParameterList::getInstance()->readAllValues();
  Serial.print("R20");
  #ifdef BT_Serial2_Use
    Serial2.print("R20");   //-2018.02.25-CGH
  #endif   
  CurrentState::getInstance()->printQAndNewLine();

  return 0;
}

