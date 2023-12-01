//code for both sensors and stepper motor to work at same time
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Stepper.h>

// Temperature sensor definitions
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);   
DallasTemperature sensors(&oneWire);

// pH sensor definitions
#define pH_SensorPin 0
#define Offset 0.00
unsigned long int avgValue;

// TDS sensor definitions
#define TdsSensorPin A1
#define VREF 5.0
#define SCOUNT 20
int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0;

// Initialize the stepper library on pins 8 through 11:
const int stepsPerRevolution = 200;
const float maxRotation = 0.6; // Max Threshold is 13/17 teeth, or 0.76 of a full rotation. 
Stepper myStepper = Stepper(stepsPerRevolution, 8, 9, 10, 11);

// RC Control toggle inputs: activate sampling and sensors via RC input. 
const int masterTogglePin = 3;
const int sensorTogglePin = 4; 
const int stepperTogglePin = 5; 

// Control variable for data collection
const int ALL_OFF = 0;
const int SENSORS_ON = 1;
const int MOTORS_ON = 2;
int state = MOTORS_ON;

bool clockwise = true;
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0; 

void setup()
{
  // Initialize sensors
  sensors.begin();
  pinMode(pH_SensorPin, INPUT);
  pinMode(TdsSensorPin, INPUT);
  pinMode(masterTogglePin, INPUT);
  pinMode(sensorTogglePin, INPUT);
  pinMode(stepperTogglePin, INPUT);

  // Set the motor speed (RPMs):
  myStepper.setSpeed(100);

  // Initialize Serial communication
  Serial.begin(115200);
  Serial.println("Ready");

}

void loop()
{
  decideState();

  currentMillis = millis();
  
  //replaces delay function so loop always running despite use of delay(). 
  if (currentMillis - previousMillis >= 2000) { 
    previousMillis = currentMillis;
    
    // THIS IS THE MAIN FUNCTIONALITY
    if (state == ALL_OFF) 
    {
      Serial.println("ALL OFF");
    }
    else if (state == SENSORS_ON) 
    {
      activateSensors(); 
    } 
    else if (state == MOTORS_ON) // WATER COLLECTION
    {
      activateStepper(maxRotation);
    } 
    else 
    {
      Serial.println("default case");
    }
    Serial.println("end loop action");
  }
  
}

/**
Modify "state", or what should be running, based on toggle inputs. 
0 is kill. 1 = activate sensors. 2 = activate stepper. 
*/
void decideState() {
    if (digitalRead(masterTogglePin) == HIGH) 
    {
      state = ALL_OFF;
    } 
    else if (digitalRead(sensorTogglePin)) 
    {
      state = SENSORS_ON; 
    } 
    else if (digitalRead(stepperTogglePin)) 
    {
      state = MOTORS_ON; 
    }
} 

/**
Activates all 3 sensors. Understand and document this better, especially TDS! 
*/
void activateSensors() {
  Serial.println("case 1");
      // pH Measurement
      int buf[10];
      for (int i = 0; i < 10; i++)
      {
        buf[i] = analogRead(pH_SensorPin);
        delay(10);
      }
      for (int i = 0; i < 9; i++)
      {
        for (int j = i + 1; j < 10; j++)
        {
          if (buf[i] > buf[j])
          {
            int temp = buf[i];
            buf[i] = buf[j];
            buf[j] = temp;
          }
        }
      }
      avgValue = 0;
      for (int i = 2; i < 8; i++)
        avgValue += buf[i];

      float phValue = (float)avgValue * 5.0 / 1024 / 6;
      phValue = 3.5 * phValue + Offset;

      // Temperature Measurement
      sensors.requestTemperatures();
      float tempC = sensors.getTempCByIndex(0);
      float tempF = (tempC * 9.0) / 5.0 + 32.0;

      // TDS Measurement      
      analogBufferIndex = 0;
      for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
      {
        analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);
        analogBufferIndex++;
        delay(5);
      }
      for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
        analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * VREF / 1024.0;
      float compensationCoefficient = 1.0 + 0.02 * (tempC - 25.0); // factoring in temperature dependence of TDS? 
      float compensationVoltage = averageVoltage / compensationCoefficient;
      tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5; //Conversion from voltage to TDS? 

      // Print results to Serial
      Serial.print("Temperature: ");
      Serial.print(tempC);
      Serial.print("C | ");
      Serial.print(tempF);
      Serial.print("F | pH: ");
      //Serial.print(phValue, 2);
      Serial.print("NO PH RN");
      Serial.print(" | TDS Value: ");
      Serial.print(tdsValue, 0);
      Serial.println("ppm");
      //state = 2;
}

/**
Rotates the stepper back and forth by a desired number of rotations. 
@param: number of rotations. Could be fractional! E.g. 0.75 of a full revolution. 
*/
void activateStepper(float numRotations) {
  Serial.println("case 2");
      int desiredRotation = int(numRotations * stepsPerRevolution); 
      if (clockwise) 
      {
        myStepper.step(desiredRotation);
        clockwise = false;
      } 
      else // anticlockwise
      {
        myStepper.step(-desiredRotation);
        clockwise = true;
      }
      state = SENSORS_ON;
}

/**
A filter that smoothens out the ultimate TDS reading. 
@param: bArray[], Array full of TDSsensorPin readings. Voltages
@param: iFilterLen, ??? 
@return: Median voltage reading. 
*/
int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    return bTab[(iFilterLen - 1) / 2];
  else
    return (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
}
