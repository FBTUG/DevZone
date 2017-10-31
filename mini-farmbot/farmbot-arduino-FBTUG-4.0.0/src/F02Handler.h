/*
 * F02Handler.h
 *
 *  Created on: 19 MAY 2014
 *      Author: Joe Hou
 */

#ifndef F02HANDLER_H_
#define F02HANDLER_H_
#include "GCodeHandler.h"
#include "Config.h"
#include "CurrentState.h"
#include "pins.h"
#include "Config.h"
#include "PinControl.h"
#include "ParameterList.h"


class F02Handler : public GCodeHandler {
public:
        static F02Handler* getInstance();
        int execute(Command*);
private:
        F02Handler();
        F02Handler(F02Handler const&);
        void operator=(F02Handler const&);
};

#endif /* F02Handler_H_ */
