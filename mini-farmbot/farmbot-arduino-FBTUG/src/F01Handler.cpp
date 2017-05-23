

/*
 * F01Handler.cpp
 *
 *  Created on: 19 MAY 2017
 *      Author: Joe Hou
 */

#include "F01Handler.h"

static F01Handler *instance;

F01Handler *F01Handler::getInstance()
{
  if (!instance)
  {
    instance = new F01Handler();
  };
  return instance;
};

F01Handler::F01Handler()
{
}

int F01Handler::execute(Command *command)
{
   Serial.print(" F0 command T:");
   Serial.print(command->getT());

  //
  // Set the value 1 on an arduino pin 9(watering), wait for time T in milliseconds,
  // set value 0 on the arduino pin 9 to close the watering in mode 0 (digital=0/analog=1)
  // Function--> writePulse(command->getP(), command->getV(), command->getW(), command->getT(), command->getM());
  //
  PinControl::getInstance()->writePulse(WATER_PIN, ENABLE, DISABLE, command->getT(), DIGITAL);

  return 0;
}
