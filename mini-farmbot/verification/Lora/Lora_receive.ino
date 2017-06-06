#include <SoftwareSerial.h>

SoftwareSerial lora(4, 5);  // UNO (RX, TX) --> (Arduino Pin 4 ->Lora TX)
                                        // --> (Arduino Pin 5 ->Lora RX) 
                                        // P1,P2,GND -->GND
                                        // BZ --> N/A
                                        // +V --> 3.3V
void setup() {
    Serial.begin(9600);
    lora.begin(9600);

    pinMode(13, OUTPUT);
    pinMode(11, OUTPUT);
    digitalWrite(13, HIGH);
    delay(3000);
    digitalWrite(13, LOW);
}

void loop() {
  if (lora.available()) {
    String read_string = lora.readString();
  //  Serial.println(read_string.length());
    Serial.println(read_string);
    digitalWrite(11, HIGH);
    delay(1000);
    digitalWrite(11, LOW);
  }
}
