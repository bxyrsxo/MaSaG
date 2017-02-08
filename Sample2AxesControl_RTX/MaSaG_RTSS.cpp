//////////////////////////////////////////////////////////////////
//
// Sample2AxesControl_RTX.c - c file
//
// This file was generated using the RTX 2016 SDK 12.0.0.1324
// RTX Application Template.
//
// Created: 5/20/2016 2:30:58 PM 
// User: NET3640
//
//////////////////////////////////////////////////////////////////

#include "RTX.h"
#include "User_Data.h"

using namespace Eigen;
using namespace std;


// Joint 0 command for testing purpose
int J0_Command(USER_DAT* pData)
{
	RTN_ERR		ret;
	// J0 Position Command Here at the beginning
	ret = NEC_CoE402Halt(pData->mAxis[0][0], 0);
	ret = NEC_CoE402Halt(pData->mAxis[1][0], 0);
	Sleep(2000);
	ret = NEC_CoE402PtpA(pData->mAxis[0][0], OPT_ABS | OPT_WMC, pData->targetPos[0][0], pData->Velocity_J0[0], pData->ACC_DEC_J0[0], pData->ACC_DEC_J0[0]);
	if (ret != 0) { RtPrintf("NEC_CoE402PtpA failed!%d L0 \n", ret); return 1; }
	ret = NEC_CoE402PtpA(pData->mAxis[1][0], OPT_ABS | OPT_WMC, pData->targetPos[1][0], pData->Velocity_J0[1], pData->ACC_DEC_J0[1], pData->ACC_DEC_J0[1]);
	if (ret != 0) { RtPrintf("NEC_CoE402PtpA failed!%d R0 \n", ret); return 1; }
	Sleep(15000);

	return 0;
}

int main(int  argc,	char **argv)
{
	RTX			_RTX;
	RTN_ERR     ret;

	while (1)
	{
		_RTX.Info();
		ret = _RTX.WaitForSingleObject(-3);
		if (ret == 0)
		{
			ret = _RTX.Init();		if (ret) return 1;
			Sleep(2000);

			//rflag = J0_Command(pData);								if (rflag) return 1;
						
			//MASSAGE PROGRAM!!!
			//while ()
			//{
			//switch()
			//}

			_RTX.Close_Process();
		}
	}

	return 0;
}

