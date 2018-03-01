/*
 * F00Handler.cpp
 *
 *  Created on: 2018/02/26
 *      Author: Chen GeHom
 */

#include "F00Handler.h"

static F00Handler *instance;

//------------------------------------------------------------------------------------------
F00Handler *F00Handler::getInstance()
{
  if (!instance){
    instance = new F00Handler();
  }
  
  return instance;
}

//------------------------------------------------------------------------------------------
F00Handler::F00Handler()
{
    //
}

//------------------------------------------------------------------------------------------
int F00Handler::execute(Command *command)
{    
int vFuncCode = 0;

    //ParameterList::getInstance()->readValue(command->getP());
    //ParameterList::getInstance()->writeValue(command->getP(), command->getV());
    
    vFuncCode = command->getP();
    //Serial.print("FuncCode = "); Serial.println(vFuncCode);

    if(vFuncCode == 1){   //-F00 P1 Reset EEPROM to Default-2018.02.26
        Serial.print("R00 Reset AllValuesToDefault, writeAllValuesToEeprom\r\n");    
        #ifdef BT_Serial2_Use
          Serial2.print("R00 Reset AllValuesToDefault, writeAllValuesToEeprom\r\n");    //-2018.02.25-CGH
        #endif  
              
        //writeValueEeprom(0, 0);   //-2018.02.26-Set to Default   
        ParameterList::getInstance()->setAllValuesToDefault();
        ParameterList::getInstance()->writeAllValuesToEeprom();
    }
         
    return 0;
}

//------------------------------------------------------------------------------------------
