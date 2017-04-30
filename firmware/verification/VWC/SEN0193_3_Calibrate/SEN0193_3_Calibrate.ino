/* 3 sensors VWC audo calibrate sample code
 *  Features:
 *    load/save VWC boundry value at EEPROM address 0 ( size = 2* int )
 *    Press button to start/stop calibrate mode. 
 *    Calibrating with LED on
 *  
 *  Connection: 
 *    Button : D2, default high
 *    VWC sensor: A3,A4,A5
 *    LED 13, embedded
 *    
 *  Behavior:
 *    per second print out 
 *    (Sensor_ADC, VWC(%), Bound_Down, Bound_Top )*3
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
#define PIN_LED 13
#define CALINIT_TOP 400
#define CALINIT_DOWN 350
#define DEF_TOP 520
#define DEF_DOWN 261
#define VWC_CNT 3
struct ADC_Bound {
  int v_top; // value sense at air
  int v_down; // value sense at water
};
ADC_Bound bound[VWC_CNT];
int pin_vwc[VWC_CNT] = {A3,A4,A5};
int mode =0; //mode 0: normal mode , mode 1: calibrate mode
int count=0; // lopp count 

void save_setting(){
  for(int i=0;i<VWC_CNT;i++){
    EEPROM.put(0+i*sizeof(ADC_Bound), bound[i]);  
  }
}
void load_setting(){
  for(int i=0;i<VWC_CNT;i++){
    EEPROM.get(0+i*sizeof(ADC_Bound), bound[i]);  
    
    if(bound[i].v_top==0 || bound[i].v_down ==0 ){
      bound[i].v_top = DEF_TOP;
      bound[i].v_down = DEF_DOWN;   
    }
  }
}

void setup() {  
  Serial.begin(115200);
  pinMode(PIN_BUTTON,INPUT);
  for(int i=0;i<VWC_CNT;i++){
    pinMode(pin_vwc[i],INPUT);
    bound[i].v_top = DEF_TOP;
    bound[i].v_down = DEF_DOWN; 
  }
  pinMode(PIN_LED,OUTPUT);
  load_setting();

}



void sensing(){
  // sense and pring
  int val[VWC_CNT];
  float vwc[VWC_CNT];

  for(int i=0;i<VWC_CNT;i++){
    val[i] = analogRead(pin_vwc[i]); //connect sensor to Analog 0
    if( mode == 1 ){
      if(val[i]> bound[i].v_top) bound[i].v_top = val[i];
      if(val[i]< bound[i].v_down ) bound[i].v_down = val[i];
    }
  
    vwc[i] = 1.0-((float(val[i])-bound[i].v_down)/(bound[i].v_top-bound[i].v_down));
  //Serial.println(vwc*100); //print the value to serial port
  Serial.print(val[i]); Serial.print(",");
  Serial.print(vwc[i]*100); Serial.print(",");
  Serial.print(bound[i].v_down); Serial.print(",");
  Serial.print(bound[i].v_top);
  if(i != VWC_CNT-1 ) Serial.print(",");
  }
  Serial.println("");
    
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
      for(int i=0;i<VWC_CNT;i++){
        bound[i].v_top = CALINIT_TOP;
        bound[i].v_down = CALINIT_DOWN;
      }
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
