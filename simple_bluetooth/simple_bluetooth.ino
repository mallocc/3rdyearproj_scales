#include <SoftwareSerial.h>
SoftwareSerial BTSerial(4, 2); // RX, TX

String command = ""; // Stores response of the HC-06 Bluetooth device
char myChar;

void setup() {
  // Open serial communications:
  Serial.begin(9600);
  Serial.println("Type AT commands!");

  // The HC-06 defaults to 9600 according to the datasheet.
  BTSerial.begin(9600);
}

void loop() {
  while (BTSerial.available()) {
    myChar = BTSerial.read();
    Serial.print(myChar);
  }

  while (Serial.available()) {
    myChar = Serial.read();
    Serial.print(myChar); //echo
    BTSerial.print(myChar);
  }
}
