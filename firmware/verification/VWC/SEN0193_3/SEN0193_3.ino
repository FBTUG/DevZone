int count=0;
int relay=0;
float bound_hi=523;
float bound_low=261;
#define PIN1 A3
#define PIN2 A4
#define PIN3 A5
  
void setup() {
  Serial.begin(9600); // open serial port, set the baud rate as 9600 bps
  pinMode(PIN1,INPUT);
  pinMode(PIN2,INPUT);
  pinMode(PIN3,INPUT);
}

void loop() {
  int val1,val2,val3;
  float vwc1,vwc2,vwc3;

  val1 = analogRead(PIN1); //connect sensor to Analog 0
  val2 = analogRead(PIN2);
  val3 = analogRead(PIN3);
  vwc1 = 1-((val1-bound_low)/(bound_hi-bound_low));
  vwc2 = 1-((val2-bound_low)/(bound_hi-bound_low));
  vwc3 = 1-((val3-bound_low)/(bound_hi-bound_low));
  //Serial.println(vwc*100); //print the value to serial port
  Serial.print(val1); Serial.print(",");
  Serial.print(val2); Serial.print(",");
  Serial.print(val3); Serial.print(",");
  Serial.print(vwc1); Serial.print(",");
  Serial.print(vwc2); Serial.print(",");
  Serial.print(vwc3); Serial.println("");
  delay(1000);
  count++;
}
