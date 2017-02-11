#ifndef _ABSDRIVER_h
#define _ABSDRIVER_h

class absDriver
{
public:
	//function
	virtual void initialization() = 0;
	virtual int  readEncoder() = 0;
	virtual void cmdPosition(double cmd) = 0;// count
	virtual void cmdVelocity(double cmd) = 0;// rpm
	virtual void cmdTorque() = 0;
	~absDriver() = default;
	// data
	int JointNodeID = 0;
	int encoder_value = 0;
};

#endif