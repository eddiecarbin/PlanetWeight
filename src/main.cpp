#include <Arduino.h>
#include "SoftwareSerial.h"
#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "PotMonitor.h"
#include "DFRobotDFPlayerMini.h"

#define POT_PIN 18

PotMonitor volumePot(POT_PIN, 12);
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
Adafruit_7segment matrix = Adafruit_7segment();
DFRobotDFPlayerMini myDFPlayer;

long map2(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
}

void setup()
{
  // put your setup code here, to run once:

#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("7 Segment Backpack Test");
#endif
  matrix.begin(0x70);

  pinMode(LED_BUILTIN, OUTPUT);

  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  debouncer.attach(BUTTON_PIN, INPUT);

  debouncer.interval(200); // interval in ms

  // pinMode(BUTTON_PIN, INPUT);

  if (!myDFPlayer.begin(mySoftwareSerial))
  { //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
      ;
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(15); //Set volume value. From 0 to 30

  fileCount = myDFPlayer.readFileCounts();

  Serial.print("file count = ");
  Serial.println(fileCount);
}

int fileCount = 0;
int playCount = 0;
bool soundPlaying = false;
int soundIdx = 1;

void StartSoundPlayer()
{
  soundPlaying = true;
  playCount = 1;
  delay(500);
  myDFPlayer.play(soundIdx); //Play the first mp3
  soundIdx++;
  if (soundIdx > fileCount)
  {
    soundIdx = 1;
  }
}

void StopSoundPlayer()
{
  soundPlaying = false;
  myDFPlayer.stop();
}

void loop()
{
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000);                     // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(1000);                     // wait for a second

  if (volumePot.hasUpdated())
  {
    //Serial.println(speedPotentiometer.getValue());
    long volume = map2(volumePot.getValue(), 0, 880, 0, 30);
    Serial.println(volume);
    myDFPlayer.volume(volume);
  }

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

    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
  void printDetail(uint8_t type, int value)
  {
    switch (type)
    {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value)
      {
      case Busy:
        Serial.println(F("Card not found"));
        break;
      case Sleeping:
        Serial.println(F("Sleeping"));
        break;
      case SerialWrongStack:
        Serial.println(F("Get Wrong Stack"));
        break;
      case CheckSumNotMatch:
        Serial.println(F("Check Sum Not Match"));
        break;
      case FileIndexOut:
        Serial.println(F("File Index Out of Bound"));
        break;
      case FileMismatch:
        Serial.println(F("Cannot Find File"));
        break;
      case Advertise:
        Serial.println(F("In Advertise"));
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }