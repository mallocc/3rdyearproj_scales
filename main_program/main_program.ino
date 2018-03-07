#include <SoftwareSerial.h>
SoftwareSerial BTSerial(4, 2); // RX, TX
#include "LowPower.h"

#define POWER_OFF 0
#define POWER_ON 1

char myChar;

int load = 0;
const int ledPin =  13;
const int button_pin =  9;
int power_state = POWER_OFF;
int power_time_ticks = 3;
int power_time_counter = 0;

void setup() {
  pinMode(button_pin, INPUT);
  pinMode(ledPin, OUTPUT);
  // Open serial communications:
  Serial.begin(9600);
  // The HC-06 defaults to 9600 according to the datasheet.
  BTSerial.begin(9600);
  Serial.print("BT serial started...");
  delay(1000);
}

void loop() {

int state = digitalRead(button_pin);
  if (state)
  {
    Serial.println("button pressed");
    power_state = POWER_ON;
    power_time_counter = 0;
  }

  if (power_state == POWER_ON)
  {
    Serial.println("tick power on");
    //    digitalWrite(ledPin, HIGH);
    //    delay(100);
    ++power_time_counter;
    BTSerial.println(String(load++));
  }
  else
  {
       Serial.println("tick power off");
  }

  if (power_time_counter > power_time_ticks)
    power_state = POWER_OFF;

  LowPower.powerDown(power_state ? SLEEP_250MS : SLEEP_1S, ADC_OFF, BOD_OFF);
}
