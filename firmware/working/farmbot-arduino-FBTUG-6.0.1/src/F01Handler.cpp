/*
 * F01Handler.cpp
 *
 *  Created on: 19 MAY 2017
 *      Author: Joe Hou
 */

#include "F01Handler.h"

static F01Handler *instance;

//------------------------------------------------------------------------------------------
F01Handler *F01Handler::getInstance()
{
  if (!instance){
    instance = new F01Handler();
  }
  
  return instance;
}

//------------------------------------------------------------------------------------------
F01Handler::F01Handler()
{
    //    
}

//------------------------------------------------------------------------------------------
int F01Handler::execute(Command *command)
{
volatile double WaterFlowTimeDefault = 0;
volatile double Watercapacity = 0;
long time = 0;

    if (command->getN() != 0){
        WaterFlowTimeDefault = ParameterList::getInstance()->getValue(WATER_FLOW_TIME_RATIO);
        Watercapacity = command->getN();

    	  // Example: N 500   (watering 500ml)
    	  // Time = 2000ms * (500ml / 100ml) = 10000ms = 10secs
        time = WaterFlowTimeDefault * (Watercapacity/100);
    
        Serial.print("F0 command N:\n Watercapacity = "); Serial.print(command->getN()); Serial.print("ml\n");
        Serial.print(" WaterFlowTimeDefault = "); Serial.print(WaterFlowTimeDefault); Serial.print("ms\n");
        Serial.print(" Time = "); Serial.print(time); Serial.print("ms\n");
        #ifdef BT_Serial2_Use
          Serial2.print("F0 command N:\n Watercapacity = "); Serial2.print(command->getN()); Serial2.print("ml\n");   //-2018.02.25-CGH
          Serial2.print(" WaterFlowTimeDefault = "); Serial2.print(WaterFlowTimeDefault); Serial2.print("ms\n");    //-2018.02.25-CGH
          Serial2.print(" Time = "); Serial2.print(time); Serial2.print("ms\n");    //-2018.02.25-CGH
        #endif    
    }
  
    if (command->getT() != 0){
        time = command->getT();
        Serial.print("F0 command T:\n  Time = "); Serial.print(time); Serial.print("ms\n");
        #ifdef BT_Serial2_Use     
          Serial2.print("F0 command T:\n  Time = "); Serial2.print(time); Serial2.print("ms\n");    //-2018.02.25-CGH
        #endif  
    }
    
    // Set the value 1 on an arduino pin 9(watering), wait for time T in milliseconds,
    // set value 0 on the arduino pin 9 to close the watering in mode 0 (digital=0/analog=1)
    // Function--> writePulse(command->getP(), command->getV(), command->getW(), command->getT(), command->getM());
    PinControl::getInstance()->writePulse(WATER_PIN, ENABLE, DISABLE, time, DIGITAL);
  
    return 0;
}

//------------------------------------------------------------------------------------------

