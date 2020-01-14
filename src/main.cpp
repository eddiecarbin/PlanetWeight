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

#define GO_TO_DISPLAY_WEIGHT 1
#define GO_TO_WAIT_FOR_USER 2

PotMonitor volumePot(POT_PIN, 12);

SoundPlayer soundPlayer(POT_PIN, 15);

// WeightManager scaleManager(A1, A0);

float calibration_factor = -9570; //-7050 worked for my 440lb max scale setup

Adafruit_7segment matrix01 = Adafruit_7segment();
Adafruit_7segment matrix02 = Adafruit_7segment();

Adafruit_7segment matrix03 = Adafruit_7segment();
Adafruit_7segment matrix04 = Adafruit_7segment();

const int LOADCELL_DOUT_PIN = A1;
const int LOADCELL_SCK_PIN = A0;

const long minWeight = 30;
int earth, moon, mars, jupiter;

HX711 scale;

State StateDoNothing(NULL, NULL, NULL);
Fsm fsm(&StateDoNothing);
//
long map2(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
}

void OnIdleWaitForUserEnter()
{
  scale.power_up();
  matrix01.print(0);
  matrix01.writeDisplay();
  matrix02.print(0);
  matrix02.writeDisplay();
  matrix03.print(0);
  matrix03.writeDisplay();
  matrix04.print(0);
  matrix04.writeDisplay();

  earth = 0;
}

void OnIdleWaitForUserUpdate()
{
  if (scale.is_ready())
  {
    scale.set_scale(calibration_factor);
    earth = abs(scale.get_units());
    if (earth > minWeight)
      fsm.trigger(GO_TO_DISPLAY_WEIGHT);
    else
      delay(500);
  }
  else
  {
    Serial.println("HX711 not found.");
  }
}

void OnIdleWaitForUserExit()
{
  // scale.power_down();
}

/*------------------------------------------------
/  Display User State
*/

void OnDisplayUserEnter()
{
  soundPlayer.PlaySound(1);
}

void OnDisplayUserUpdate()
{
  scale.set_scale(calibration_factor);
  earth = abs(scale.get_units());
  // Serial.println(earth);
  moon = earth * 0.17;
  mars = earth * 0.38;
  jupiter = earth * 2.32;

  matrix01.print(earth);
  matrix01.writeDisplay();
  matrix02.print(moon);
  matrix02.writeDisplay();
  matrix03.print(mars);
  matrix03.writeDisplay();
  matrix04.print(jupiter);
  matrix04.writeDisplay();

  if (earth < minWeight)
    fsm.trigger(GO_TO_WAIT_FOR_USER);
}

State StateIdleWaitForUser(&OnIdleWaitForUserEnter, &OnIdleWaitForUserUpdate, &OnIdleWaitForUserExit);
State StateOnDisplayUser(&OnDisplayUserEnter, &OnDisplayUserUpdate, NULL);

//DFRobotDFPlayerMini myDFPlayer;
// Fsm fsm(&StateIdleWaitForUser);

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  fsm.add_transition(&StateIdleWaitForUser, &StateOnDisplayUser,
                     GO_TO_DISPLAY_WEIGHT, NULL);
  fsm.add_transition(&StateOnDisplayUser, &StateIdleWaitForUser,
                     GO_TO_WAIT_FOR_USER, NULL);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println(scale.read_average(20));

  scale.tare();

  matrix01.begin(0x70);
  matrix02.begin(0x71);
  matrix03.begin(0x73);
  matrix04.begin(0x74);

  soundPlayer.initialize();

  fsm.goToState(&StateIdleWaitForUser);
  delay(500);
}

void loop()
{
  if (volumePot.hasUpdated())
  {
    //Serial.println(speedPotentiometer.getValue());
    long volume = map2(volumePot.getValue(), 0, 880, 0, 30);
    Serial.println(volume);
    //soundPlayer.volume(volume);
  }

  soundPlayer.update();
  fsm.run_machine();
}
