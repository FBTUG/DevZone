/********************************************************

   PID watering control - simulation
    By faking watering relay control and fake weight measurement. Study and optimize the control method
   Features:
    1. High bound limit
   Reference PID RelayOutput Example: http://playground.arduino.cc/Code/PIDLibraryRelayOutputExample
 ********************************************************/

#include <PID_v1.h>
#define RelayPin 6

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT);

// unit definition: weight: g, volumn: cl, rate: g/second
int WindowSize = 5000; // relay control window size
int relay_control_dur = 200; //ms
unsigned long windowStartTime;
int prev_relay_stat = 0;
int cur_relay_stat = 0;
unsigned long prev_time = 0;
float prev_weight;
float ideal_rate = 10.0; // watering rate
float loss_rate = 0.1;     // nature loss rate
float act_rate = 0;
double dry_weight = 1950;  // initial weight
double target_weight = 2000; // control weight
double target_margin = 6;
double weight_cur = dry_weight;
double time_diff;


void set_relay(int stat) {
  if (weight_cur >= target_weight + target_margin) { //apply high bound
    stat = 0;
  }
  prev_relay_stat = cur_relay_stat;
  cur_relay_stat = stat;
  digitalWrite(RelayPin, stat);
}

// by relay on/off time calulation to estimate the weight
double fake_measure(unsigned long time_now) {
  time_diff = (time_now - prev_time) / 1000.0; // per second
  if (cur_relay_stat == 1) {
    weight_cur += time_diff * ideal_rate;
  }
  weight_cur -= time_diff * loss_rate;
  return weight_cur;
}


void setup()
{
  Serial.begin(9600);
  prev_weight = dry_weight;
  windowStartTime = millis();
  prev_time = windowStartTime;
  //initialize the variables we're linked to
  Setpoint = target_weight;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}

unsigned long loop_count = 0;
void loop()
{
  unsigned long now = millis();
  Input = fake_measure(now);
  Setpoint = target_weight;
  myPID.Compute();

  /************************************************
     turn the output pin on/off based on pid output
   ************************************************/

  if (now - windowStartTime > WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if (Output > now - windowStartTime) set_relay(HIGH);
  else set_relay(LOW);

  // debug print
  if (1) {
    if (loop_count % 1 == 0 ) {
      //weight_cur,Output,cur_relay_stat
      Serial.print(weight_cur);
      Serial.print(","); Serial.print(Output);
      Serial.print(","); Serial.println(cur_relay_stat);
    }
  } else { // for plotter
    Serial.println(weight_cur);
  }

  prev_time = now;
  loop_count++;
  delay(relay_control_dur);

}
