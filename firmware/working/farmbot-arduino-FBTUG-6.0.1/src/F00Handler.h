/*
 * F00Handler.h
 *
 *  Created on: 2018/02/26
 *      Author: Chen GeHom
 */

#ifndef F00HANDLER_H_
#define F00HANDLER_H_
#include "GCodeHandler.h"
#include "Config.h"
#include "CurrentState.h"
#include "ParameterList.h"    //-2018.02.26

class F00Handler : public GCodeHandler
{
public:
  static F00Handler *getInstance();
  int execute(Command *);

private:
  F00Handler();
  F00Handler(F00Handler const &);
  void operator=(F00Handler const &);
};

#endif /* F00HANDLER_H_ */

