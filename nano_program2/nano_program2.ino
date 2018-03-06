#include <SoftwareSerial.h>
SoftwareSerial blueSerial(4, 2); // RX, TX
//SoftwareSerial barcoSerial(6, 5); // RX, TX

struct Button {
  int pin;
  bool state, oldState;
  int counter;

  Button(int _pin)
  {
    pin = _pin;
    state = oldState = 0;
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

  bool check()
  {
    oldState = state;
    return state = digitalRead(pin);
  }
};

String command = ""; // Stores response of the HC-06 Bluetooth App

int potPin = 2;    // select the input pin for the potentiometer
int pollRate = 500;

const int ledPin =  13;      // the number of the LED pin


bool toggled = 0;

Button button(7);

int powerWait = 300;

int btButtonWait = 200;
bool btDiscover = 0;
bool btConnected = 0;
int btCheckCounter = 0;
int btCheckCount = 10;

int ledCounter = 0;
int ledCount = 50;

int loadVal = 0;

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
  digitalWrite(ledPin, HIGH);
  while (1);
}

void reset()
{
  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
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
  if (button.state == HIGH)
  {
    button.tick();
    if (button.counter > powerWait)
    {
      powerOff();
    }
    else if (button.counter > btButtonWait)
    {
      if (!toggled)
        btDiscover = !btDiscover;
      toggled = 1;
    }
  }
  else
  {
    if (button.counter <= btButtonWait
        && !button.state
        && button.oldState)
      reset();
    button.reset();
    toggled = 0;
  }
}

String intToChars(int i)
{
  String s = "xx";
  s[0] = (char) (i / 256);
  s[1] = (char) (i % 256);
  return s;
}

void writeString(String s)
{
  for (int i = 0; i < s.length(); ++i)
  {
    delay(500);
    blueSerial.write(s.charAt(i));
  }
}
void writeStringln(String s)
{
  writeString(s);
  blueSerial.write('\n');
}

void sendLoadVal()
{
  String s  = "xw";
  writeStringln(s);// + intToChars(loadVal));
}

bool handleCommand(String c)
{
  bool commandState = 1;

  if (c[0] == 'A' && c[1] == 'T')
  {
    Serial.print("[SYS] ");
    writeStringln(c);
  }
  else
  { switch (c[0])
    {
      case 'x':
        Serial.print("[SCL] ");
        break;
      case 'y':
        Serial.print("[APP] ");
        break;
      default:
        commandState = 0;
    }

    if (commandState)
    { switch (c[1])
      {
        case 'w':
          Serial.print(" load = ");
          Serial.print(loadVal);
          break;
        case 'r':
          Serial.print(" reseting weight");
          break;
        case 'p':
          Serial.print(" powering off...");
          powerOff();
          break;
        default:
          commandState = 0;
      }
    }
  }

  if (!commandState)
    Serial.print("[ERR] Not a command: ");

  Serial.println(c);
}

void handleBluetoothState()
{
  command = "";
  // Read device output if available.
  if (blueSerial.available()) {
    while (blueSerial.available()) { // While there is more to be read, keep reading.
      delay(500);
      command += (char)blueSerial.read();

      handleCommand(command);
      command = ""; // No repeats
    }
  }
  // Read user input if available.
  bool commandExists = 0;
  while (Serial.available()) { // While there is more to be read, keep reading.
    delay(500);
    command += (char)Serial.read();
    commandExists = 1;
  }
  if (commandExists)
    handleCommand(command);

}

void loop() {
  delay(100);
  handleBluetoothState();
  handleButtonState();
  handleLEDState();
  //sendLoadVal();
}



