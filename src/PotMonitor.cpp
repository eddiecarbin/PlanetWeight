/*
 * PotMonitor.cpp
 *
 *  Created on: Jan 19, 2016
 *      Author: carbi
 */

#include <Arduino.h>
#include "PotMonitor.h"

#define TOLERANCE 12

int tolerance = 12;

PotMonitor::PotMonitor(int pin, int value = 12)
{
	this->pin = pin;
	this->tolerance = value;
	old_value = 0;

	pinMode(pin, INPUT);
}

int PotMonitor::getValue()
{
	return old_value;
}

bool PotMonitor::hasUpdated()
{
	int pot_value = analogRead(pin);
	//pot_value = map(pot_value, 0, 1023, 0, 179);


	//delay(15);

	int diff = abs(pot_value - old_value);

	if (diff > tolerance)
	{
			// Serial.println( pot_value );

		old_value = pot_value; // only save if the val has changed enough to avoid slowly drifting
		// and so on
		return true;
	}

	return false;

}

PotMonitor::~PotMonitor()
{
}


