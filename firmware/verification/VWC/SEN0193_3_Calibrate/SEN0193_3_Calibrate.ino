/* 3 sensors VWC audo calibrate sample code
 *  Features:
 *    load/save VWC boundry value at EEPROM address 0 ( size = 2* int )
 *    Press button to start/stop calibrate mode. 
 *    Calibrating with LED blinking
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
 *    Press Button - see LED blink (about per-second)
 *    In any sequence in the Air and Water. Suggest have 30 seconds in each state
 *    When air and water tested, LED blink fast
 *    Press Button - see LED off, setting will be save only when blink fast
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
#define SYN_AVG_CNT 3 // Get dynamic average from SYN_AVG_CNT sensing
#define BOUND_MARGIN 10 
struct ADC_Bound {
  int v_top; // value sense at air
  int v_down; // value sense at water
};
ADC_Bound bound[VWC_CNT];
int pin_vwc[VWC_CNT] = {A3,A4,A5};
int mode =0; //mode 0: normal mode , mode 1: calibrate mode
int count=0; // lopp count
int bound_change[VWC_CNT*2]; // 0: not changed, 1: changed 
int led_state = 0; //0: LED off, 1:LED on
float dyn_avg[VWC_CNT];

void save_setting(){
  Serial.println("Save setting!");
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

void update_avg(int i,int value){
  if(dyn_avg[i]==0){
    dyn_avg[i]=value;
  }else{
    dyn_avg[i] = (dyn_avg[i] * (SYN_AVG_CNT-1) + value )/SYN_AVG_CNT;
  }
}
void setup() {  
  Serial.begin(9600);
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
    update_avg(i,val[i]);
    if( mode == 1 ){
      if(dyn_avg[i]> bound[i].v_top - BOUND_MARGIN) {
        bound[i].v_top = round(dyn_avg[i]);
        bound_change[i*2+0] =1;
      }
      if(dyn_avg[i]< bound[i].v_down + BOUND_MARGIN ) {
        bound[i].v_down = round(dyn_avg[i]);
        bound_change[i*2+ 1] =1;
      }
    }
  
    vwc[i] = 1.0-((float(val[i])-bound[i].v_down)/(bound[i].v_top-bound[i].v_down));
  //Serial.println(vwc*100); //print the value to serial port
  Serial.print(val[i]); Serial.print(",");
  Serial.print(vwc[i]*100); Serial.print(",");
  Serial.print(bound[i].v_down); Serial.print(",");
  Serial.print(bound[i].v_top); Serial.print(" ");
  if(i != VWC_CNT-1 ) Serial.print(",");
  }
  Serial.println("");
    
}
int calibrate_complete_check(){
  int change_cnt =0;
  for(int i=0;i<VWC_CNT*2;i++){
    if(bound_change[i]==1){
      change_cnt++;
    }
  }
  if(change_cnt==6) return 1;
  else return 0;
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
      if(calibrate_complete_check()) save_setting();
      else load_setting();
    }else{ //mode = 1 , cal start
      Serial.println("Calibrate Start!");
      digitalWrite(PIN_LED,1);
      for(int i=0;i<VWC_CNT;i++){
        bound[i].v_top = CALINIT_TOP;
        bound[i].v_down = CALINIT_DOWN;
        bound_change[i*2+0] = 0;
        bound_change[i*2+1] = 0;
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
  int toggle_set;
  mode_check_and_run();
  if(count % 20 == 0 ){
    sensing();
  }
  if( mode == 1 ){
    toggle_set = 10; // estimate on/off per second
    if(calibrate_complete_check()){
      toggle_set = 3; // about 3 time faster
    }
    if(count % toggle_set == 0 ){
      led_state = 1 - led_state;
      digitalWrite(PIN_LED,led_state);
    }
  }
  count++;
  delay(50);
}
