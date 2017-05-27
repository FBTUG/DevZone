/*
 * F02Handler.cpp
 *
 *  Created on: 19 MAY 2017
 *      Author: Joe Hou
 */

#include "F02Handler.h"


static F02Handler* instance;
int WaterCountsDefault = 0;
volatile double WaterFlow = 0;
volatile double Watercapacity = 0;
bool StartWatering = false;

F02Handler * F02Handler::getInstance() {
  if (!instance) {
    instance = new F02Handler();
  };
  return instance;
}
;

F02Handler::F02Handler() {
}

//Measure the quantity of square wave
void pulse() {
  WaterFlow += (1.0 / WaterCountsDefault);
  Serial.print("WaterFlow:");
  Serial.print(WaterFlow*1000);
  Serial.print("ml\n");
  if ((WaterFlow*1000) >= Watercapacity) {
    //
    // Disable the IRQ3 and water pump
    //
    detachInterrupt(WATER_FLOW_INT);
    PinControl::getInstance()->writeValue(WATER_PIN, DISABLE, DIGITAL);
    StartWatering = false;
    WaterFlow = 0;
  }
}

int F02Handler::execute(Command* command) {
  WaterCountsDefault = ParameterList::getInstance()->getValue(WATER_FLOW_COUNTS);
  Watercapacity = command->getN();

  Serial.print("WaterCountsDefault = ");
  Serial.print(WaterCountsDefault);
  Serial.print("\n");
  Serial.print("Watercapacity = ");
  Serial.print(Watercapacity);
  Serial.print("\n");
  if ((Watercapacity > 0) && (!StartWatering)) {
    //
    // Enable the water pump
    //
    PinControl::getInstance()->writeValue(WATER_PIN, ENABLE, DIGITAL);
	//
    // For Water flow sonser, PIN 20 (Interrupt 3)
    //
    pinMode(WATER_FLOW_PIN  , INPUT_PULLUP);
    attachInterrupt(WATER_FLOW_INT, pulse, RISING);  //DIGITAL Pin 20: Interrupt 3
    StartWatering = true;
  }
  return 0;
}
