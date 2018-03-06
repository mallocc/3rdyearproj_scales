#include <SoftwareSerial.h>
SoftwareSerial BTSerial(4, 2); // RX, TX
#include "LowPower.h"

#define POWER_OFF 0
#define POWER_ON 1

char myChar;

int load = 0;
const int ledPin =  13;
const int button_pin =  6;
int button_pressed = 0;
int button_pressed_old = 0;

int power_state = POWER_OFF;
int power_time_ticks = 10;
int power_time_counter = 0;

int bt_discover_button_counter = 0;
int bt_discover_button_ticks = 5;

void setup() {
  pinMode(button_pin, INPUT);
    pinMode(ledPin, OUTPUT);
  // Open serial communications:
  Serial.begin(9600);
  // The HC-06 defaults to 9600 according to the datasheet.
  BTSerial.begin(9600);
  Serial.print("BT serial started...");
}

void loop() {
    digitalWrite(ledPin, HIGH);
    
  while (BTSerial.available()) {
    myChar = BTSerial.read();
    Serial.print(myChar);
  }

  while (Serial.available()) {
    myChar = Serial.read();
    Serial.print(myChar); //echo
    BTSerial.print(myChar);
  }

  load++;
  BTSerial.print(String(load));

  button_pressed = digitalRead(button_pin);

  if (button_pressed)
  {
    bt_discover_button_counter++;
    if (!button_pressed_old) // inital press
    {
      power_state = POWER_ON;
      power_time_counter = 0;
      //bt_discover_button_counter = 0;
    }
  }        

  if (power_state == POWER_ON)
    ++power_time_counter;

  if (power_time_counter > power_time_ticks)
    power_state = POWER_OFF;

  button_pressed_old = button_pressed;
  
  LowPower.powerDown(power_state ? SLEEP_250MS : SLEEP_2S, ADC_OFF, BOD_OFF);
}
