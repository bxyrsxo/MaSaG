#ifndef _JOINT_H
#define _JOINT_H

#include "absDriver.h"
#include "MCDC3006_CF.h"

class Joint

{

public:

	absDriver* driver;
	int ENC2Rad = 2*pi/ _resEncoder / 4 / _gearRatio;
	
	Joint(int encoder, int gear, int driver_type, int Node) : _resEncoder(encoder), _gearRatio(gear)
	{
		driver = new MCDC3006_CF(Node);
	}

private:

	int _resEncoder;
	int _gearRatio;
	
};


#endif