#include "ServoControl.h"
#include "TimerOne.h"
#include <Servo.h>

/*
Servo pin layout
D11 D6 D5 D4
*/

static ServoControl *instance;

Servo servos[2];

int CurrentWaterAngle = 0;     //  Initial the angle to zero for close water
ServoControl *ServoControl::getInstance()
{
  if (!instance)
  {
    instance = new ServoControl();
  };
  return instance;
};

ServoControl::ServoControl()
{
}

int ServoControl::attach() {
  Serial.print(" ServoControl::attach");
	servos[0].attach(SERVO_0_PIN);
	servos[1].attach(SERVO_1_PIN);
  
	//
	// Initial the angle to zero for close water first
	//
  servos[0].write(0);
  servos[1].write(0);
}

int ServoControl::setAngle(int pin, int angle)
{

	Serial.print("R99");
	Serial.print(" ");
	Serial.print("SERVO");
	Serial.print(" ");
	Serial.print("pin");
	Serial.print(" ");
	Serial.print(pin);
	Serial.print(" ");
	Serial.print("angle");
	Serial.print(" ");
	Serial.print(angle);
	Serial.print(" ");
	Serial.print("\r\n");


  switch (pin)
  {
  case 4:
    // CurrentWaterAngle = angle;
	if ((angle > MAX_WATER_SERVO_ANGLE) || (angle < MIN_WATER_SERVO_ANGLE)) {
	    Serial.print("The angle is out of control");
        return 1;
	}
    servos[0].write(angle);
    break;
  case 5:
    servos[1].write(angle);
    break;
  default:
    return 1;
  }

  return 0;
}
