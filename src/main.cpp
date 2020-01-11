#include <Arduino.h>
#include "SoftwareSerial.h"
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "PotMonitor.h"
#include "SoundPlayer.h"
#include <SPI.h>

#define POT_PIN 18

PotMonitor volumePot(POT_PIN, 12);

SoundPlayer soundPlayer(POT_PIN, 15);

Adafruit_7segment matrix = Adafruit_7segment();

Adafruit_7segment matrix01 = Adafruit_7segment();
Adafruit_7segment matrix02 = Adafruit_7segment();

Adafruit_7segment matrix03 = Adafruit_7segment();
Adafruit_7segment matrix04 = Adafruit_7segment();

//DFRobotDFPlayerMini myDFPlayer;

long map2(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
}

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  //#ifndef __AVR_ATtiny85__
  // Serial.println("7 Segment Backpack Test");
  //#endif
  matrix01.begin(0x70);

  pinMode(LED_BUILTIN, OUTPUT);

  //Serial.begin(115200);

  soundPlayer.initialize();

  delay(500);
  //debouncer.attach(BUTTON_PIN, INPUT);

  //debouncer.interval(200); // interval in ms

  // pinMode(BUTTON_PIN, INPUT);
}

void loop()
{
  if (volumePot.hasUpdated())
  {
    //Serial.println(speedPotentiometer.getValue());
    long volume = map2(volumePot.getValue(), 0, 880, 0, 30);
    Serial.println(volume);
    //soundPlayer.volume(10);
  }
  /*
  if (myDFPlayer.available() && soundPlaying == true)
  {

    if (myDFPlayer.readType() == DFPlayerPlayFinished && playCount < fileCount)
    {

      delay(500);
      myDFPlayer.next();
      soundIdx++;

      if (soundIdx > fileCount)
      {
        soundIdx = 1;
      }
      Serial.print("count = ");
      Serial.println(playCount);

      playCount += 1;
    }
    else
    {
    }
    */
  //printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
}
