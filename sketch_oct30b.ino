
#include <SoftwareSerial.h>

SoftwareSerial mySerial(6, 5); // RX, TX

void setup() {
  Serial.begin(9600);
}

void loop() {
  // run over and over
  if (mySerial.available()) {
    byte x = mySerial.read();
    Serial.println(x);
  }
}
