#ifndef _MCDC3006_CF_H
#define _MCDC3006_CF_H
#include "absDriver.h"
#include "canconfunc.h"
#include <process.h>




#define NUMBER_OF_DOFS			9
#define pi 3.1415926
class MCDC3006_CF : public absDriver
{
	friend long roundd(double x);
	friend int CanHexToDec(UINT8 *Data, int *Num);
	friend int DecToCanHex(int *Num, UINT8 *Data);
public:
	//// inheritance from absDriver
	void initialization();
	int  readEncoder();
	void cmdPosition(double cmd_p_val);
	void cmdVelocity(double cmd_v_val);
	void cmdTorque();
	
	//// constructor
	MCDC3006_CF() = default;
	MCDC3006_CF(int ID) {
		JointNodeID = ID;
	}

	
	//void InitPara();
	//void ResetPara();
	void InitMC();
	void EnableMC();

	void DisableMC();
	void SetOperationMode(int mode);

	void SetSpeed(int Speed);
	void GetSpeed();
	void LoadAbsPos(int targetPos);
	void LoadRelPos(int distance);
	void InitiateMotion();
	void SetHomZero();

	void PosRequest();
	//void begin_thread()
	//{
	//	_beginthread(&MCDC3006_CF::ReceiveTread_wrapper, 0, static_cast<void*>(this));
	//	//_beginthread(&motion::ReceiveTread_wrapper, 0, NULL);
	//}

	//void ReceiveThread();

	/*static void __cdecl ReceiveTread_wrapper(void* o)
	{
		static_cast<MCDC3006_CF*>(o)->ReceiveThread();
	}
*/
	//void PDO2DataProcess(UINT8 *Data);
	~MCDC3006_CF() = default;
private:
	int velocity = 0;
	int cmd_value = 0;
	
};
long roundd(double x);
int CanHexToDec(UINT8 *Data, int *Num);
int DecToCanHex(int *Num, UINT8 *Data);
extern LONG rtMustQuit;
#endif