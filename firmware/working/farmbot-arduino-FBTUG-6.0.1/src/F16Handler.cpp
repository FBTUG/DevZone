
/*
 * F16Handler.cpp
 *
 *  Created on: 2014/07/21
 *      Author: MattLech
 */

#include "F16Handler.h"

static F16Handler *instance;

F16Handler *F16Handler::getInstance()
{
  if (!instance)
  {
    instance = new F16Handler();
  };
  return instance;
};

F16Handler::F16Handler()
{
}

int F16Handler::execute(Command *command)
{
  int ret = 0;

  if (LOGGING){
    Serial.print("R99 HOME Z\r\n");    
    #ifdef BT_Serial2_Use
      Serial2.print("R99 HOME Z\r\n");   //-2018.02.25-CGH
    #endif   
  }

  ret = StepperControl::getInstance()->calibrateAxis(2);

  if (ret == 0) {
    StepperControl::getInstance()->moveToCoords(0, 0, 0, 0, 0, 0, false, false, true);
  }


  if (LOGGING)
  {
    CurrentState::getInstance()->print();
  }

  return 0;
}

