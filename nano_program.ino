#include <SoftwareSerial.h>
SoftwareSerial blueSerial(4, 2); // RX, TX
//SoftwareSerial barcoSerial(6, 5); // RX, TX

struct Button {
  int pin;
  bool state;
  int counter;

  Button(int _pin)
  {
    pin = _pin;
    state = 0;
    counter = 0;
    pinMode(pin, INPUT);
  }

  void tick()
  {
    counter++;
  }

  void reset()
  {
    counter = 0;
  }

  void check()
  {
    state = digitalRead(pin);
  }
};

String message = "";

char command = 0; // Stores response of the HC-06 Bluetooth App
int potPin = 2;    // select the input pin for the potentiometer
int pollRate = 500;
bool getReading = 0;

const int ledPin =  13;      // the number of the LED pin

int btButtonWait = 200;
int powerWait = 300;

Button button(7);

bool btDiscover = 0;
bool btConnected = 0;
int btCheckCounter = 0;
int btCheckCount = 10;

int ledCounter = 0;
int ledCount = 50;

void setup() {
  // Open serial communications:
  Serial.begin(9600);
  Serial.println("Serial open...");
  // The HC-06 defaults to 9600 according to the datasheet.
  blueSerial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
}

void powerOff()
{
    
}

void writeString(String s)
{
  for (int i = 0; i < s.length(); ++i)
  {
    delay(10);
    blueSerial.write(s.charAt(i));
  }
}
void writeStringln(String s)
{
  writeString(s);
  blueSerial.write('\n');
}

void toApp(String s)
{
  writeStringln(s);
  Serial.print("[NANO]: " + s + '\n');
}
void fromApp(String s)
{
  Serial.print("[APP]: " + s + '\n');
}

String getLine()
{
  String s = "";
  char c = 0;
  while ((c = (char)blueSerial.read()) != '\n')
    if (c >= 'A' && c <= 'z')
      s += c;
  return s;
}

void checkConnection()
{
  if (btCheckCounter > btCheckCount)
    writeStringln("AT");

  btCheckCounter++;
  btCheckCounter %= btCheckCount;
}

void handleBluetooth()
{
  // Read App output if available.
  if (blueSerial.available()) {
    command = (char)blueSerial.read();

    if (command >= 'A' && command <= 'Z')
    {
      fromApp(">" + String(command));
      switch (command)
      {
        case 'R':
          getReading = !getReading;
          break;
        case 'M':
          fromApp(getLine());
          break;
        case 'X':
          toApp("I am replying.");
          break;
        case 'O':
          btConnected = 1;
        default :

          break;
      }
    }
    else
      toApp("Unknown command.");
  }

  if (getReading)
  {
    toApp(String(analogRead(potPin)));
    delay(pollRate);
  }

  checkConnection();

}

void handleLEDState()
{
  if (btDiscover)
  {
    if (ledCounter < ledCount / 2)
      digitalWrite(ledPin, HIGH);
    else
      digitalWrite(ledPin, LOW);
  }
  else
  {
    if (btConnected)
      digitalWrite(ledPin, HIGH);
    else
      digitalWrite(ledPin, LOW);
  }

  ledCounter++;
  ledCounter %= ledCount;
}

void handleButtonState()
{
  button.check();
  if (button.state  == HIGH)
  {
    button.tick();
    if (button.counter > btButtonWait)
    {
      btDiscover = !btDiscover;
      button.reset();
    }
  }
  else
  {
    button.reset();
  }
}

void loop() {
  delay(10);
  //handleBluetooth();
  handleButtonState();
  handleLEDState();

}



