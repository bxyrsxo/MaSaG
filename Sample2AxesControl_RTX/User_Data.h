#pragma once

#include "MaSaG_RTSS.h"

#define ARM_DOF		7
#define LEFT_ARM	0
#define RIGHT_ARM	1

typedef	struct
{
	//Master
	U16_T		masterId;
	U16_T		MasterState;
	// Motors for both arms, representing with 2-dim arrays 
	// where 0 indicates left arm, and 1 indicates right arm
	CANAxis_T	mAxis[2][ARM_DOF + 1];	// dof+1 for f/t sensors
	I32_T		actualPos[2][ARM_DOF];
	I32_T		targetPos[2][ARM_DOF];
	I16_T		actualTorque[2][ARM_DOF];
	I16_T		targetTorque[2][ARM_DOF];
	U16_T		slaveState[2][ARM_DOF];
	U8_T		controlMode[2][ARM_DOF];
	U32_T		ACC_DEC_J0[2];
	U32_T		Velocity_J0[2];
	//Other parameters
	I32_T		Mode;
	I8_T		currentState[20];
	U32_T		runTime;
	BOOL_T		startFlag;

}USER_DAT;
