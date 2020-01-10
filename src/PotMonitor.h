
#ifndef POTMONITOR_H_
#define POTMONITOR_H_
/*
 * PotMonitor.h
 *
 *  Created on: Jan 19, 2016
 *      Author: carbi
 */

class PotMonitor
{
public:
	PotMonitor(int pin, int tollerance);
	bool hasUpdated(void);
	int getValue(void);
	virtual ~PotMonitor();

private:
	int pin;
	int old_value;
	int tolerance;
};

#endif /* POTMONITOR_H_ */
