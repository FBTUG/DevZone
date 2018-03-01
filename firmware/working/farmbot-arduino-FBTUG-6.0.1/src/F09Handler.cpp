/*
 * F09Handler.cpp
 *
 *  Created on: 2017/04/26
 *      Author: Tim Evers
 */

#include "F09Handler.h"

static F09Handler *instance;

F09Handler *F09Handler::getInstance()
{
  if (!instance)
  {
    instance = new F09Handler();
  };
  return instance;
};

F09Handler::F09Handler()
{
}

int F09Handler::execute(Command *command)
{

  if (LOGGING){
    Serial.print("R99 Reset emergency stop\r\n");    
    #ifdef BT_Serial2_Use
      Serial2.print("R99 Reset emergency stop\r\n");    //-2018.02.25-CGH
    #endif  
  }

  CurrentState::getInstance()->resetEmergencyStop();

  return 0;
}

