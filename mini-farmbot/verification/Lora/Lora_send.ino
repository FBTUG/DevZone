#include <SoftwareSerial.h>

SoftwareSerial lora(4, 5); // UNO (RX, TX) --> (Arduino Pin 4 ->Lora TX)
                                         // --> (Arduino Pin 5 ->Lora RX) 
                                        // P1,P2,GND -->GND
                                        // BZ --> N/A
                                        // +V --> 3.3V
#define INCOMING_CMD_BUF_SIZE 50 
char incomingChar = 0;
char incomingCommandArray[INCOMING_CMD_BUF_SIZE];
int incomingCommandPointer = 0;

void setup() {
    Serial.begin(9600);
    lora.begin(9600);
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(3000);
    digitalWrite(13, LOW);
}

void loop() {
  if (Serial.available()) {
    incomingChar = Serial.read();
    if (incomingChar >= '0' && incomingChar <= 'z')
    {
      incomingCommandArray[incomingCommandPointer] = incomingChar;
      incomingCommandPointer++;
    } 

    // If the string is getting to long, cap it off with a new line and let it process anyway
    if (incomingCommandPointer >= INCOMING_CMD_BUF_SIZE - 1)
    {
      incomingChar = '\n';
      incomingCommandArray[incomingCommandPointer] = incomingChar;
      incomingCommandPointer++;
    }
   if (incomingChar == '\n') {
     lora.println(incomingCommandArray);
   }
  }
}
