/*
 * T01Handler.h
 *
 *  Created on: 29 MAY 2017
 *      Author: Joe Hou
 */

#ifndef T01HANDLER_H_
#define T01HANDLER_H_
#include "GCodeHandler.h"
#include "Config.h"
#include "CurrentState.h"
#include "pins.h"
#include "Config.h"
#include "PinControl.h"
#include "ParameterList.h"

class T01Handler : public GCodeHandler
{
public:
  static T01Handler *getInstance();
  int execute(Command *);

private:
  T01Handler();
  T01Handler(T01Handler const &);
  void operator=(T01Handler const &);
};

#endif /* T01HANDLER_H_ */
