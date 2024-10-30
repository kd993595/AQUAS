/*
  Arduino FS-I6X Demo
  fsi6x-arduino-uno.ino
  Read output ports from FS-IA6B receiver module
  Display values on Serial Monitor
  
  Some code derived from Ricardo Paiva - https://gist.github.com/werneckpaiva/
*/

#include <Servo.h>

// Define Input Connections
//#define CH1 3
//#define CH2 5
//#define CH3 6
//#define CH4 9
#define CH5 10
#define CH6 11

  // Integers to represent values from sticks and pots
  //int ch1Value;
  //int ch2Value;
  //int ch3Value;
  //int ch4Value;

  // Boolean to represent switch value
bool ch5Value;
bool ch6Value;

// Pump variables
const int intakePumpRelayPin = 5;
const int outtakePumpRelayPin = 6;
const int pumpVoltageScale = 100;  // 0-255 analog signal for pwm control of pump motors.
// Keep track if intake or outtake mode is on.
int intakeState = LOW;
int outtakeState = LOW;

// Powder resevoir and mixer components
const int mixerPin = 7;
const int agitatorPin = 8;
const int servoPin = 9;
Servo myServo;
int servoAngle = 90;
int servoState = 0; // 0 = closed, 1 = open. 

// Track time elapsed for synchronous sensing and actuation of components
unsigned long currentMillis = 0;
unsigned long servoPreviousMillis = 0;
unsigned long intakePreviousMillis = 0;
unsigned long outtakePreviousMillis = 0;



// Read the number of a specified channel and convert to the range provided.
// If the channel is off, return the default value
// Read an analog channel (e.g. joystick) and map it onto a usable range.
int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue) {
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Read the switch channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}

void setup() {
  // Set up serial monitor
  Serial.begin(115200);
  
  // Controller pin setup:
  //  pinMode(CH1, INPUT);
  //  pinMode(CH2, INPUT);
  //  pinMode(CH3, INPUT);
  //  pinMode(CH4, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);

  // Pump pin setup
  pinMode(intakePumpRelayPin, OUTPUT);
  pinMode(outtakePumpRelayPin, OUTPUT);

  // Servo setup
  myServo.attach(servoPin);
  myServo.write(0); 
  
}

void loop() {
  // Get current millis elapsed. 
  unsigned long currentMillis = millis();


  // Get values for each channel
  //  ch1Value = readChannel(CH1, -100, 100, 0);
  //  ch2Value = readChannel(CH2, -100, 100, 0);
  //  ch3Value = readChannel(CH3, -100, 100, -100);
  //  ch4Value = readChannel(CH4, -100, 100, 0);
  //  ch5Value = readChannel(CH5, -100, 100, 0);
  ch5Value = readSwitch(CH5, false);
  ch6Value = readSwitch(CH6, false);

  // RUN INTAKE COMPONENTS
  if (ch5Value == HIGH) {
    // Run just the servo first for 1 second pulses. 
    if (servoState == LOW) {
      myServo.write(servoAngle);
      servoPreviousMillis = currentMillis;
      servoState = HIGH; 
    }

    if (servoState == HIGH && currentMillis - servoPreviousMillis >= 1000) {
      myServo.write(0);
      servoState = LOW; 
    }

    if (intakeState == LOW) {
      // Run rest of intake system
      analogWrite(intakePumpRelayPin, pumpVoltageScale);
      digitalWrite(agitatorPin, HIGH);
      digitalWrite(mixerPin, HIGH);
      intakePreviousMillis = currentMillis;
      intakeState = HIGH;
      Serial.println("intake runnning");
    }

    if (intakeState == HIGH && currentMillis - intakePreviousMillis >= 3000) {
      analogWrite(intakePumpRelayPin, LOW);
      intakeState = LOW;
    }

  } else {
    analogWrite(intakePumpRelayPin, LOW);
    digitalWrite(mixerPin, LOW);
    digitalWrite(agitatorPin, LOW);
    myServo.write(0); 
  }

  // RUN OUTTAKE COMPONENTS
  if (ch6Value == HIGH) {
      if (outtakeState == LOW) {
        analogWrite(outtakePumpRelayPin, pumpVoltageScale);
        outtakePreviousMillis = currentMillis;
        outtakeState = HIGH;
        Serial.println("outtake runnning");
      }
      if (outtakeState == HIGH && currentMillis - outtakePreviousMillis >= 3000) {
        analogWrite(outtakePumpRelayPin, LOW);
        outtakeState = LOW;
      }

  } else { 
      analogWrite(outtakePumpRelayPin, LOW);
  }


  // Print to Serial Monitor
  //  Serial.print("Ch1: ");
  //  Serial.print(ch1Value);
  //  Serial.print(" | Ch2: ");
  //  Serial.print(ch2Value);
  //  Serial.print(" | Ch3: ");
  //  Serial.print(ch3Value);
  //  Serial.print(" | Ch4: ");
  //  Serial.print(ch4Value);
  Serial.print(" | Ch5: ");
  Serial.print(ch5Value);
  Serial.print(" | Ch6: ");
  Serial.println(ch6Value);
}
