#include "arduino_secrets.h"
#include "thingProperties.h"
#include <Servo.h>

int redLightPin = 5; //D5
int greenLightPin = 4; //D4
int led = LED_BUILTIN;
int servoPin = 3; //D3

#define GATE_OPEN 0
#define GATE_CLOSED 90

Servo myServo;

void setServo(int angle);
void greenLightOn(bool value);

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Pin configuration
  pinMode(redLightPin, OUTPUT);
  pinMode(greenLightPin, OUTPUT);
  pinMode(led, OUTPUT);

  pinMode(servoPin, OUTPUT);
  myServo.attach(servoPin);

  // Initial state: gate closed and red light on
  setServo(GATE_CLOSED);
  greenLightOn(false);
}

void loop() {
  ArduinoCloud.update();
}

/*
  Since Region is READ_WRITE variable, onRegionChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onRegionChange()  {
  if (region == "EU" || region == "SWISS") {
    Serial.println("Opening barrier");
    greenLightOn(true);
    setServo(GATE_OPEN);
  } else {
    Serial.println("Closing barrier");
    greenLightOn(false);
    setServo(GATE_CLOSED);
  }
}

void setServo(int angle) {
  angle = angle + 60;
  int duty_cycle = map(angle, 0, 180, 544, 2400);
  myServo.writeMicroseconds(duty_cycle);
}

void greenLightOn(bool value) {
  if (value) {
    digitalWrite(greenLightPin, HIGH);
    digitalWrite(led, HIGH);
    digitalWrite(redLightPin, LOW);
  } else {
    digitalWrite(greenLightPin, LOW);
    digitalWrite(led, LOW);
    digitalWrite(redLightPin, HIGH);
  }
}