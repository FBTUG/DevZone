/*
 * G00Handler.cpp
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 */

#include "G00Handler.h"

static G00Handler *instance;

//------------------------------------------------------------------------------------------
G00Handler *G00Handler::getInstance()
{
  if (!instance){
    instance = new G00Handler();
  }
  
  return instance;
}

//------------------------------------------------------------------------------------------
G00Handler::G00Handler()
{
    //    
}

//------------------------------------------------------------------------------------------
int G00Handler::execute(Command *command)
{       
    /* 
  	Serial.print("G00 was here\r\n");   //-2018.02.24

  	Serial.print("R99");
  	Serial.print(" X ");
  	Serial.print(command->getX());
  	Serial.print(" Y ");
  	Serial.print(command->getY());
  	Serial.print(" Z ");
  	Serial.print(command->getZ());
    
  	Serial.print(" A ");
  	Serial.print(command->getA());
  	Serial.print(" B ");
  	Serial.print(command->getB());
  	Serial.print(" C ");
  	Serial.println(command->getC());
    
  	//Serial.print("\r\n");
    */
    
    //int StepperControl::moveToCoords(double xDestScaled, double yDestScaled, double zDestScaled,
    //                               unsigned int xMaxSpd, unsigned int yMaxSpd, unsigned int zMaxSpd,
    //                               bool xHome, bool yHome, bool zHome)
                                 
    StepperControl::getInstance()->moveToCoords(
        command->getX(), command->getY(), command->getZ(),
        command->getA(), command->getB(), command->getC(),
        false, false, false);
  
    if (LOGGING){
      CurrentState::getInstance()->print();
    }
    
    return 0;
}

//------------------------------------------------------------------------------------------

