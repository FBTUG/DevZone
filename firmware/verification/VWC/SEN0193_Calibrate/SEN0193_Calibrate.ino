/* VWC audo calibrate sample code
 *  load/save VWC boundry value at EEPROM address 0 ( size = 2* int )
 *  Press button to start/stop calibrate mode. 
 *  Calibrating with LED on
 *  
 *  Connection: 
 *    Button : D2, default high
 *    VWC sensor: A3
 *    LED 13, embedded
 *    
 *  Behavior:
 *    per second print out 
 *    Sensor_ADC, VWC, Bound_Down, Bound_Top
 *    
 *  Calibrate procedure:
 *    Power On
 *    Press Button - see LED on
 *    In any sequence in the Air and Water. Suggest have 10 seconds in each state
 *    Press Button - see LED off
 *    
 *  Detail:
 *    Without calibrate, the default value is DEF_TOP, DEF_DOWN
 *    Calibrate start initial bound to CALINIT_TOP, CALINIT_DOWN
 *    Sensor : http://www.icshop.com.tw/product_info.php/products_id/24987
 *    
 *  Author: Wuulong 2017/4/24
 */
#include <EEPROM.h>

#define PIN_BUTTON 2
#define PIN_VWC A3
#define PIN_LED 13
#define CALINIT_TOP 400
#define CALINIT_DOWN 350
#define DEF_TOP 523
#define DEF_DOWN 261
struct ADC_Bound {
  int v_top; // value sense at air
  int v_down; // value sense at water
};
ADC_Bound bound;
int mode =0; //mode 0: normal mode , mode 1: calibrate mode
int count=0; // lopp count 

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUTTON,INPUT);
  pinMode(PIN_VWC,INPUT);
  pinMode(PIN_LED,OUTPUT);
  bound.v_top = DEF_TOP;
  bound.v_down = DEF_DOWN; 
  
  load_setting();

}

void save_setting(){
  EEPROM.put(0, bound);  
}
void load_setting(){
  EEPROM.get(0, bound);
  if(bound.v_top==0 || bound.v_down ==0 ){
    bound.v_top = DEF_TOP;
    bound.v_down = DEF_DOWN;   
  }
}

void sensing(){
  // sense and pring
  int val;
  float vwc;

  val = analogRead(PIN_VWC); //connect sensor to Analog 0
  if( mode == 1 ){
    if(val> bound.v_top) bound.v_top = val;
    if(val< bound.v_down ) bound.v_down = val;
  }

  vwc = 1.0-((float(val)-bound.v_down)/(bound.v_top-bound.v_down));
  //Serial.println(vwc*100); //print the value to serial port
  Serial.print(val); Serial.print(",");
  Serial.print(vwc); Serial.print(",");
  Serial.print(bound.v_down); Serial.print(",");
  Serial.println(bound.v_top);
  
  
}
void mode_check_and_run(){
  int btn_state;
  //detect if btn click
  btn_state = digitalRead(PIN_BUTTON);
  if( btn_state ==0 ){ // button down
    mode = 1- mode;
    if( mode == 0 ){ // cal complete
      Serial.println("Calibrate Complete!");
      digitalWrite(PIN_LED,0);
      save_setting();
    }else{ //mode = 1 , cal start
      Serial.println("Calibrate Start!");
      digitalWrite(PIN_LED,1);
      bound.v_top = CALINIT_TOP;
      bound.v_down = CALINIT_DOWN;
    }
    delay(500); // to avoid single click with multiple action
  }
  
}
/*
 * normal mode sensor to console every 1 second
 */
//

void loop() {
  mode_check_and_run();
  if(count % 20 == 0 ){
    sensing();
  }
  count++;
  delay(50);
}
