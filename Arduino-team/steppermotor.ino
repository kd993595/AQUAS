/* Example sketch to control a stepper motor with L298N motor driver, Arduino UNO and Stepper.h library. More info: https://www.makerguides.com */

// Include the Stepper library:
#include <Stepper.h>

// Define number of steps per revolution:
const int stepsPerRevolution = 200;

// Initialize the stepper library on pins 8 through 11:
Stepper myStepper = Stepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  // Set the motor speed (RPMs):
  myStepper.setSpeed(100);
}

void loop() {
  // Step one revolution in one direction:
  myStepper.step(200);

  delay(2000);

  // Step on revolution in the other direction:
  myStepper.step(-200);

  delay(2000);
}
