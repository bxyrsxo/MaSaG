#pragma once

#include "User_Data.h"

void __RtCyclicCallback(void *UserDataPtr);
void __RtEventCallback(void *UserDataPtr, U32_T EventCode);
void __RtErrorCallback(void *UserDataPtr, I32_T ErrorCode);

class RTX
{
public:
	int			rflag;
	
	RTX();
	int			WaitForSingleObject(int msec);
	int			Get_Drivers_ID();
	int			MotorParams_Init();
	int			Init();
	void		Info();
	void		Close_Process();

private:

	HANDLE		shHandle;
	HANDLE		obHandle;
	USER_DAT	*pData;
	RTN_ERR     ret;
	U16_T       masterId;
	TClintParam clientParam;

	void		SharedMemory_Init();
	void		RTX_Event_Init();
	

};
