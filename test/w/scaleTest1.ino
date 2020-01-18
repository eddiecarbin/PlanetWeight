#include "HX711.h"
#include <math.h>
#define DOUT  A0
#define CLK  A1
HX711 scale(DOUT, CLK);
float calibration_factor = -9570; //-7050 worked for my 440lb max scale setup


#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix01 = Adafruit_7segment();
Adafruit_7segment matrix02 = Adafruit_7segment();

Adafruit_7segment matrix03 = Adafruit_7segment();
Adafruit_7segment matrix04 = Adafruit_7segment();

float earth, moon, mars, jupiter;
float val = 0;
float i = 0;

const int numReadings = 7;


float readings[numReadings];      // the readings from the analog
int readIndex = 0;              // the index of the current reading
float total = 0;                  // the running total
float average = 0;                // the average

void setup() {
  Serial.begin(9600);

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);



#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("7 Segment Backpack Test");
#endif
  matrix01.begin(0x70);
  matrix02.begin(0x71);

  matrix03.begin(0x73);
  matrix04.begin(0x74);

  matrix01.print(0);
  matrix02.print(0);
  matrix03.print(0);
  matrix04.print(0);
  //delay(500);

}

void loop() {
  scale.set_scale(calibration_factor); //Adjust to this calibration factor

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = abs(scale.get_units());
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  Serial.println("reading: " + String(abs(scale.get_units())));



  earth = average;

  //  if (i<5){
  //    val += earth;
  //  }
  //  else if (i==5){
  //    val = val/5;
  //    earth = val;
  //  }
  //  else if (i>5 && abs(val - abs(scale.get_units()))>1){
  //    i = 0;
  //  }
  //  i++;



  if (earth < 0.5) {
    earth = 0.0;
    moon = 0.0;
    mars = 0.0;
    jupiter = 0.0;
  } else {
    moon = earth * 0.17;
    mars = earth * 0.38;
    jupiter = earth * 2.32;

    earth = roundf(earth * 10.0);//  / 10.0;
    moon = roundf(moon * 10.0);//  / 10.0;
    mars = roundf(mars * 10.0);//  / 10.0;
    jupiter = roundf(jupiter * 10.0);// / 10.0;

    


  }


  Serial.print("Earth: ");
  Serial.print(earth);
  Serial.print(" lbs   "); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person

  Serial.print("Moon: ");
  Serial.print(moon);
  Serial.print(" lbs   "); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person

  Serial.print("Mars: ");
  Serial.print(mars);
  Serial.print(" lbs   "); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person

  Serial.print("Jupiter: ");
  Serial.print(jupiter);
  Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  //  Serial.print(" calibration_factor: ");
  //  Serial.print(calibration_factor);
  Serial.println();
  //  if (Serial.available())
  //  {
  //    char temp = Serial.read();
  //    if (temp == '+' || temp == 'a') {
  //      calibration_factor += 100;
  //    }
  //    else if (temp == '-' || temp == 'z') {
  //      calibration_factor -= 100;
  //    }
  //    else if (temp == 't' || temp == 'T') {
  //      scale.tare();
  //    }
  //    Serial.println("calibration_factor: " + String(calibration_factor));
  //
  //  }


  if (abs(val - earth) > 0.05 || earth < 0.5) {
    matrix01.print(int(earth));
    //    segDisp(earth);
//    matrix01.writeDigitNum('', earth, true);
        matrix01.writeDisplay();

    matrix02.print(int(moon));
    matrix02.writeDisplay();

    matrix03.print(int(mars));
    matrix03.writeDisplay();

    matrix04.print(int(jupiter));
    matrix04.writeDisplay();
  }
  //  delay(100);
  val = earth;


}

//void segDisp(float n) {
//  n = n * 100;
//  String sn = String(n);
//  sn = sn.remove(sn.length()-1);
//
//  for (int i = 0; i < sn.length(); i++){
//
//    }
//
//
//
//
//
//
//
//
//  for (int i = 0; i < sn.length() - 1; i++) {
//    //Serial.println("i: " + String(i) + "str: " + String(sn.charAt(i)));
//
//
//
//  }
//
//
//
//}
