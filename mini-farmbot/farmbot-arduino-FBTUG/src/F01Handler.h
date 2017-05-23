/*
 * F01Handler.h
 *
 *  Created on: 19 MAY 2017
 *      Author: Joe Hou
 */

#ifndef F01HANDLER_H_
#define F01HANDLER_H_
#include "GCodeHandler.h"
#include "Config.h"
#include "CurrentState.h"
#include "pins.h"
#include "Config.h"
#include "PinControl.h"

class F01Handler : public GCodeHandler
{
public:
  static F01Handler *getInstance();
  int execute(Command *);

private:
  F01Handler();
  F01Handler(F01Handler const &);
  void operator=(F01Handler const &);
};

#endif /* F44HANDLER_H_ */
