#include <Arduino.h>
#include "SoftwareSerial.h"
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "PotMonitor.h"
#include "SoundPlayer.h"
#include <SPI.h>
#include "Fsm.h"
#include <HX711.h>

#define POT_PIN 18

// #define GO_TO_DISPLAY_WEIGHT 1
#define GO_TO_WAIT_FOR_USER 2
#define GO_TO_CALCULATE_WEIGHT 3
// #define GO_USER_LEFT 4
#define FRAME_PER_SECOND 60

SoundPlayer soundPlayer(1, 0);

// WeightManager scaleManager(A1, A0);

float calibration_factor = -9570; //-7050 worked for my 440lb max scale setup

Adafruit_7segment matrix01 = Adafruit_7segment();
Adafruit_7segment matrix02 = Adafruit_7segment();

Adafruit_7segment matrix03 = Adafruit_7segment();
Adafruit_7segment matrix04 = Adafruit_7segment();

// const int LOADCELL_DOUT_PIN = A1;
// const int LOADCELL_SCK_PIN = A0;

const long minWeight = 30;

const double MOON_F = 0.17;
const double MARS_F = 0.38;
const double JUPITER_F = 2.32;

int readIndex = 0;
const int numReadings = 5;
unsigned long _lastCheck = 0;
float readings[numReadings]; // the readings from the analog
float total = 0;             // the running total
float average = 0;           // the average
float readValue = 0;

HX711 scale;

State StateDoNothing(NULL, NULL, NULL);
Fsm fsm(&StateDoNothing);
//

void DisplayWeight(int value)
{
  matrix01.print(int(value));
  matrix01.writeDisplay();
  matrix02.print(int(value * MOON_F));
  matrix02.writeDisplay();
  matrix03.print(int(value * MARS_F));
  matrix03.writeDisplay();
  matrix04.print(int(value * JUPITER_F));
  matrix04.writeDisplay();
}

void OnIdleWaitForUserEnter()
{
  scale.power_up();
  DisplayWeight(0);
  readValue = 0;
}

void OnIdleWaitForUserUpdate()
{
  if (scale.is_ready())
  {
    // scale.set_scale(calibration_factor);
    readValue = abs(scale.get_units());

    if (readValue > minWeight)
      fsm.trigger(GO_TO_CALCULATE_WEIGHT);
    else
      delay(500);
  }
  else
  {
    //Serial.println("HX711 not found.");
  }
}

/*------------------------------------------------
/  Display User State
*/

void OnCalculateWeightEnter()
{
  readIndex = 0;
  soundPlayer.PlaySound(1);
  _lastCheck = 0;
}

void OnCalculateWeightUpdate()
{
  scale.set_scale(calibration_factor);

  total -= readings[readIndex];
  // read from the sensor:
  readValue = readings[readIndex] = abs(scale.get_units());
  // add the reading to the total:
  total += readings[readIndex];
  // advance to the next position in the array:
  readIndex += 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings)
  {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  // Serial.println("reading: " + String(abs(scale.get_units())));

  // calculate average over a set count
  // if (readIndex < numReadings)
  // {
  //   sum += readValue;
  //   readIndex += 1;
  // }
  // else
  // {
  //   userWeight = sum / numReadings;
  // }
  unsigned long t = millis();
  if (t - _lastCheck > 50)
  {
    _lastCheck = t;
  }

  DisplayWeight(average);

  if (readValue < minWeight)
  {
    fsm.trigger(GO_TO_WAIT_FOR_USER);
    soundPlayer.StopSound();
  }
}

State StateIdleWaitForUser(&OnIdleWaitForUserEnter, &OnIdleWaitForUserUpdate, NULL);
State StateCalculateWeight(&OnCalculateWeightEnter, &OnCalculateWeightUpdate, NULL);

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);

  delay(3000);

  fsm.add_transition(&StateIdleWaitForUser, &StateCalculateWeight,
                     GO_TO_CALCULATE_WEIGHT, NULL);
  fsm.add_transition(&StateCalculateWeight, &StateIdleWaitForUser,
                     GO_TO_WAIT_FOR_USER, NULL);
  scale.begin(A1, A0);

  scale.tare();

  scale.set_scale(calibration_factor);

  matrix01.begin(0x70);
  matrix02.begin(0x71);
  matrix03.begin(0x73);
  matrix04.begin(0x74);

  soundPlayer.initialize();

  fsm.goToState(&StateIdleWaitForUser);
}

void loop()
{

  soundPlayer.update();
  fsm.run_machine();

  delay(1000 / FRAME_PER_SECOND);
}
