  int count=0;
  int relay=0;
  
void setup() {
  Serial.begin(9600); // open serial port, set the baud rate as 9600 bps
  pinMode(A3,INPUT);
  pinMode(4,OUTPUT);
  digitalWrite(4,relay);
  //analogReference(EXTERNAL);
}

void loop() {
  int val;
  float vwc;
  int r_old;

  val = analogRead(A3); //connect sensor to Analog 0
  vwc = 1-((val-260.0)/(520.0-260));
  //Serial.println(vwc*100); //print the value to serial port
  Serial.print(val);
  Serial.print(",");
  Serial.print(vwc);
  Serial.print(",");
  Serial.println(relay);
  delay(1000);
  count++;
  r_old=relay;
  if(count % 10 == 0 ){ relay=1-relay;}
  if( r_old != relay){ digitalWrite(4,relay);}
}
