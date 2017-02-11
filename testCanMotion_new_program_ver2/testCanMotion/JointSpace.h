#ifndef _JOINT_SPACE_H
#define _JOINT_SPACE_H

#include "SystemParams.h"
#include "Joint.h"

class jointSpace
{
public:
	Joint* jointLArm[7];
	Joint* jointRArm[7];
	Joint* jointHead[2];
	Joint* jointTorso[3];
	Joint* jointWheel[4];

	jointSpace() {
		jointLArm[0] = new Joint(resEncoder_ARM[0], gearRatio_ARM[0], TYPE_MAXPOS_PP);
		jointLArm[1] = new Joint(resEncoder_ARM[1], gearRatio_ARM[1], TYPE_EPOS3_CST);
		jointLArm[2] = new Joint(resEncoder_ARM[2], gearRatio_ARM[2], TYPE_MAXPOS_CST);
		jointLArm[3] = new Joint(resEncoder_ARM[3], gearRatio_ARM[3], TYPE_MAXPOS_CST);
		jointLArm[4] = new Joint(resEncoder_ARM[4], gearRatio_ARM[4], TYPE_MAXPOS_CST);
		jointLArm[5] = new Joint(resEncoder_ARM[5], gearRatio_ARM[5], TYPE_mcDSA_E65_PT);
		jointLArm[6] = new Joint(resEncoder_ARM[6], gearRatio_ARM[6], TYPE_mcDSA_E65_PT);

		jointRArm[0] = new Joint(resEncoder_ARM[0], gearRatio_ARM[0], TYPE_MAXPOS_PP);
		jointRArm[1] = new Joint(resEncoder_ARM[1], gearRatio_ARM[1], TYPE_EPOS3_CST);
		jointRArm[2] = new Joint(resEncoder_ARM[2], gearRatio_ARM[2], TYPE_MAXPOS_CST);
		jointRArm[3] = new Joint(resEncoder_ARM[3], gearRatio_ARM[3], TYPE_MAXPOS_CST);
		jointRArm[4] = new Joint(resEncoder_ARM[4], gearRatio_ARM[4], TYPE_MAXPOS_CST);
		jointRArm[5] = new Joint(resEncoder_ARM[5], gearRatio_ARM[5], TYPE_mcDSA_E65_PT);
		jointRArm[6] = new Joint(resEncoder_ARM[6], gearRatio_ARM[6], TYPE_mcDSA_E65_PT);

		jointHead[0] = new Joint(resEncoder_Head[0], gearRatio_Head[0], TYPE_MCDC3006_Pcmd);
		jointHead[1] = new Joint(resEncoder_Head[1], gearRatio_Head[1], TYPE_MCDC3006_Pcmd);

		jointTorso[0] = new Joint(resEncoder_Torso[0], gearRatio_Torso[0], TYPE_MCDC3006_Pcmd);
		jointTorso[1] = new Joint(resEncoder_Torso[1], gearRatio_Torso[1], TYPE_MCDC3006_Pcmd);
		jointTorso[2] = new Joint(resEncoder_Torso[2], gearRatio_Torso[2], TYPE_MCDC3006_Pcmd);

		jointWheel[0] = new Joint(resEncoder_Wheel[0], gearRatio_Wheel[0], TYPE_MCDC3006_Vcmd);
		jointWheel[1] = new Joint(resEncoder_Wheel[1], gearRatio_Wheel[1], TYPE_MCDC3006_Vcmd);
		jointWheel[2] = new Joint(resEncoder_Wheel[2], gearRatio_Wheel[2], TYPE_MCDC3006_Vcmd);
		jointWheel[3] = new Joint(resEncoder_Wheel[3], gearRatio_Wheel[3], TYPE_MCDC3006_Vcmd);
	}

	~jointSpace() {
		for (int i = 0; i < 7; i++)
		{
			delete jointLArm[i];
			delete jointRArm[i];
		}

		delete jointHead[0];
		delete jointHead[1];
		delete jointTorso[0];
		delete jointTorso[1];
		delete jointTorso[2];

		for (int i = 0; i < 4; i++)
			delete jointWheel[i];
	}

private:


};


#endif