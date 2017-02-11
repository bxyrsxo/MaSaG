#include "MCDC3006_CF.h"
#include <iostream>
using namespace std;
////useless initialization
void MCDC3006_CF::initialization()
{
	std::cout << "MCDC3006_CF initialization()" << std::endl;
	InitMC();
	SetOperationMode(-1);
	SetHomZero();
	DisableMC();
	EnableMC();
	std::cout << "MCDC3006_CF initialization.....................OK !" << std::endl;
}
int MCDC3006_CF::readEncoder() {
	PosRequest();
	return encoder_value;
}
void  MCDC3006_CF::cmdPosition(double cmd_p_val)
{
	std::cout << "MCDC3006_CF cmdPosition()" << std::endl;
	LoadRelPos(cmd_p_val);
	InitiateMotion();
	cout << "Rotate 5000 counts... " << endl;
}

void  MCDC3006_CF::cmdVelocity(double cmd_v_val)
{
	std::cout << "MCDC3006_CF cmdVelocity()" << std::endl;
	Sleep(10);
	SetSpeed(cmd_v_val);
	Sleep(10);
	PosRequest();
}

////useless cmd
void  MCDC3006_CF::cmdTorque()
{
	std::cout << "MCDC3006_CF invalid" << std::endl;
}

//int MaxMotorSpeed[NUMBER_OF_DOFS]; //rpm 4000rpm 2000rpm
//int MotorSpeedLimit[NUMBER_OF_DOFS];
//
//double dq_MaxSpeed[NUMBER_OF_DOFS]; //rad/s
//double dq_SpeedLimit[NUMBER_OF_DOFS];
//
//double Vtip_MaxSpeed; // 200 mm/s
//double Vtip_SpeedLimit;
//double Atip_MaxAcc;
//double Atip_AccLimit;

extern LONG rtMustQuit = 0; // quit flag for the receive thread
int RecieveTimeOutCount = 0;
//
int UpdataFlag[NUMBER_OF_DOFS] = { 0,0 };
long roundd(double x) // 四捨五入用
{
	if (x >= 0.0)
		return (long)(x + 0.5);
	else
		return (long)(x - 0.5);
}

int CanHexToDec(UINT8 *Data, int *Num) // 十六進位轉十進位
{

	if (Data[3] < 128) //>0
	{
		if (Data[3] > 0)
		{
			*Num = 16777215;
			return 0;
		}
		else
		{
			*Num = Data[2] * 65536 + Data[1] * 256 + Data[0];
			return 1;
		}
	}
	else //<0
	{
		if (Data[3] < 255)
		{
			*Num = -16777216;
			return 0;
		}
		else
		{
			int p;
			p = Data[2] * 65536 + Data[1] * 256 + Data[0];
			*Num = p - 16777216;
			return 1;
		}
	}
}

int DecToCanHex(int *Num, UINT8 *Data)
{
	int p = *Num;

	if (p < 0)
	{
		if (p < -16777216)
		{
			return 0;
		}
		else
		{
			p = 16777216 + p;
			Data[0] = p % 256;
			Data[1] = p / 256;
			Data[2] = p / 65536;
			Data[3] = 0xFF;
			return 1;
		}
	}
	else
	{
		if (p > 16777215)
		{
			return 0;
		}
		else
		{
			Data[0] = p % 256;
			Data[1] = p / 256;
			Data[2] = p / 65536;
			Data[3] = 0x0;
			return 1;
		}
	}
}
//
//void MCDC3006_CF::InitPara()
//{
//	rad2ENC[0] = -66.0*512.0*4.0 / 2.0 / pi;
//	rad2ENC[1] = -14.0*512.0*4.0 / 2.0 / pi;
//	
//	ENC2rad[0] = 1.0 / rad2ENC[0];
//	ENC2rad[1] = 1.0 / rad2ENC[1];
//	rads2rpm[0] = -66.0*30.0 / pi;
//	rads2rpm[1] = -14.0*30.0 / pi;
//	rpm2rads[0] = 1.0 / rads2rpm[0];
//	rpm2rads[1] = 1.0 / rads2rpm[1];
//
//	MaxMotorSpeed[0] = 4000;
//	MaxMotorSpeed[1] = 4000;
//
//	dq_MaxSpeed[0] = abs(rpm2rads[0] * MaxMotorSpeed[0]);
//	dq_MaxSpeed[1] = abs(rpm2rads[1] * MaxMotorSpeed[1]);
//
//	Vtip_MaxSpeed = 200.0; // mm/s
//	Atip_MaxAcc = 2000.0; //mm/s/s
//
//	ResetPara();
//}
//
//void MCDC3006_CF::ResetPara()
//{
//	MotorSpeedLimit[0] = abs(roundd(0.2*(double)MaxMotorSpeed[0]));
//	MotorSpeedLimit[1] = abs(roundd(0.2*(double)MaxMotorSpeed[1]));
//	cout << "MotorSpeedLimit[0]:" << MotorSpeedLimit[0] << endl;
//	cout << "MotorSpeedLimit[1]:" << MotorSpeedLimit[1] << endl;
//
//	dq_SpeedLimit[0] = abs(0.5*rads2rpm[0] * dq_MaxSpeed[0]);
//	dq_SpeedLimit[1] = abs(0.5*rads2rpm[1] * dq_MaxSpeed[1]);
//
//	Vtip_SpeedLimit = 0.5*Vtip_MaxSpeed;
//	Atip_AccLimit = 0.5*Atip_MaxAcc;
//
//}

//start remote mode of entire network 
void MCDC3006_CF::InitMC()
{
	HRESULT hResult;

	tCanMsg.uMsgInfo.Bytes.bType = CAN_MSGTYPE_DATA;
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(2, 0, 0, 0, 0);
	tCanMsg.uMsgInfo.Bits.srr = 0;

	tCanMsg.dwMsgId = 0x000;
	tCanMsg.abData[0] = 1;
	tCanMsg.abData[1] = 0;
	hResult = TransmitData(&tCanMsg);

}

//PDO1
void MCDC3006_CF::EnableMC()
{
	HRESULT hResult;
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(2, 0, 0, 0, 0);

	tCanMsg.dwMsgId = 0x200 + JointNodeID;
	tCanMsg.abData[0] = 0x07;//7
	tCanMsg.abData[1] = 0;
	hResult = TransmitData(&tCanMsg);

	tCanMsg.dwMsgId = 0x200 + JointNodeID;
	tCanMsg.abData[0] = 0x0F;
	tCanMsg.abData[1] = 0;
	hResult = TransmitData(&tCanMsg);
}
//PDO1
void MCDC3006_CF::DisableMC()
{
	HRESULT hResult;

	//tCanMsg.uMsgInfo.Bytes.bType  = CAN_MSGTYPE_DATA;
	//tCanMsg.uMsgInfo.Bits.srr     = 0;
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(2, 0, 0, 0, 0);

	tCanMsg.dwMsgId = 0x200 + JointNodeID;
	tCanMsg.abData[0] = 0x07;
	tCanMsg.abData[1] = 0;
	hResult = TransmitData(&tCanMsg);
}
//SDO write

void MCDC3006_CF::SetOperationMode(int mode = -1)
{
	//-1: faulhaber mode
	// 1: position mode
	// 3: velocity mode
	// 6: homing mode
	HRESULT hResult;
	UINT8 Data[4];
	DecToCanHex(&mode, Data);
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(5, 0, 0, 0, 0);
	tCanMsg.dwMsgId = 0x600 + JointNodeID;
	tCanMsg.abData[0] = 0x2F;
	tCanMsg.abData[1] = 0x60; tCanMsg.abData[2] = 0x60;
	tCanMsg.abData[3] = 0x00;
	tCanMsg.abData[4] = Data[0];
	hResult = TransmitData(&tCanMsg);
}

void MCDC3006_CF::SetSpeed(int Speed) {
	HRESULT hResult;

	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(5, 0, 0, 0, 0);
	//tCanMsg.uMsgInfo.Bits.srr     = 0;
	UINT8 Data[4];
	DecToCanHex(&Speed, Data);
	tCanMsg.dwMsgId = 0x300 + JointNodeID;
	tCanMsg.abData[0] = 0x93; // V
	tCanMsg.abData[1] = Data[0]; tCanMsg.abData[2] = Data[1]; tCanMsg.abData[3] = Data[2]; tCanMsg.abData[4] = Data[3];
	hResult = TransmitData(&tCanMsg);

	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(0, 0, 0, 0, 0);
	tCanMsg.dwMsgId = 0x080;
	hResult = TransmitData(&tCanMsg);
	/*
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(0, 0, 0, 0, 0); // request
	tCanMsg.uMsgInfo.Bits.rtr = 1;

	tCanMsg.dwMsgId = 0x280 + NodeID;
	hResult = TransmitData(&tCanMsg);*/
}

void MCDC3006_CF::GetSpeed() {
	HRESULT hResult;
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(5, 0, 0, 0, 0);
	//tCanMsg.uMsgInfo.Bits.srr     = 0;

	tCanMsg.dwMsgId = 0x300 + JointNodeID;
	tCanMsg.abData[0] = 0x2B; // GN get actual speed
	tCanMsg.abData[1] = 0; tCanMsg.abData[2] = 0; tCanMsg.abData[3] = 0; tCanMsg.abData[4] = 0;
	hResult = TransmitData(&tCanMsg);

	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(0, 0, 0, 0, 0); // request
	tCanMsg.uMsgInfo.Bits.rtr = 1;

	tCanMsg.dwMsgId = 0x280 + JointNodeID;
	hResult = TransmitData(&tCanMsg);

}
//Load Abs Positon, LA
void MCDC3006_CF::LoadAbsPos(int targetPos) {
	HRESULT hResult;
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(5, 0, 0, 0, 0);
	//tCanMsg.uMsgInfo.Bits.srr     = 0;
	int Pos = 10000;
	UINT8 Data[4];
	DecToCanHex(&Pos, Data);
	tCanMsg.dwMsgId = 0x300 + JointNodeID;
	tCanMsg.abData[0] = 0xB4; // 
	tCanMsg.abData[1] = Data[0]; tCanMsg.abData[2] = Data[1]; tCanMsg.abData[3] = Data[2]; tCanMsg.abData[4] = Data[3];
	hResult = TransmitData(&tCanMsg);

	//tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(0, 0, 0, 0, 0); // request
	//tCanMsg.uMsgInfo.Bits.rtr = 1;

	//tCanMsg.dwMsgId = 0x280 + JointNodeID;
	//hResult = TransmitData(&tCanMsg);
}
//Load rel Position, LR
void MCDC3006_CF::LoadRelPos(int distance) {
	HRESULT hresult;
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(5, 0, 0, 0, 0);
	//tCanMsg.uMsgInfo.Bits.srr     = 0;
	UINT8 Data[4];

	DecToCanHex(&distance, Data);
	tCanMsg.dwMsgId = 0x300 + JointNodeID;
	tCanMsg.abData[0] = 0xB6;
	tCanMsg.abData[1] = Data[0]; tCanMsg.abData[2] = Data[1]; tCanMsg.abData[3] = Data[2]; tCanMsg.abData[4] = Data[3];
	hresult = TransmitData(&tCanMsg);


	//tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(0, 0, 0, 0, 0); // request
	//tCanMsg.uMsgInfo.Bits.rtr = 1;

	//tCanMsg.dwMsgId = 0x280 + JointNodeID;
	//hresult = TransmitData(&tCanMsg);
}
//Initiate Motion, M
void MCDC3006_CF::InitiateMotion() {
	HRESULT hresult;
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(5, 0, 0, 0, 0);
	//tCanMsg.uMsgInfo.Bits.srr     = 0;


	tCanMsg.dwMsgId = 0x300 + JointNodeID;
	tCanMsg.abData[0] = 0x3C;
	tCanMsg.abData[1] = 0; tCanMsg.abData[2] = 0; tCanMsg.abData[3] = 0; tCanMsg.abData[4] = 0;
	hresult = TransmitData(&tCanMsg);


	//tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(0, 0, 0, 0, 0); // request
	//tCanMsg.uMsgInfo.Bits.rtr = 1;

	//tCanMsg.dwMsgId = 0x280 + JointNodeID;
	//hresult = TransmitData(&tCanMsg);
}
//HO
void MCDC3006_CF::SetHomZero()
{
	HRESULT hResult;
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(5, 0, 0, 0, 0);
	//tCanMsg.uMsgInfo.Bits.srr     = 0;
	tCanMsg.dwMsgId = 0x300 + JointNodeID;
	tCanMsg.abData[0] = 0xB8; //
	tCanMsg.abData[1] = 0; tCanMsg.abData[2] = 0; tCanMsg.abData[3] = 0; tCanMsg.abData[4] = 0;
	hResult = TransmitData(&tCanMsg);
}
//Get Actual position, POS
void MCDC3006_CF::PosRequest()
{
	HRESULT hResult;
	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(5, 0, 0, 0, 0);
	//tCanMsg.uMsgInfo.Bits.srr = 0;

	tCanMsg.dwMsgId = 0x300 + JointNodeID;
	tCanMsg.abData[0] = 0x40; //get actual position
	tCanMsg.abData[1] = 0; tCanMsg.abData[2] = 0; tCanMsg.abData[3] = 0; tCanMsg.abData[4] = 0;
	hResult = TransmitData(&tCanMsg);

	tCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(0, 0, 0, 0, 0); // request
	tCanMsg.uMsgInfo.Bits.rtr = 1;

	tCanMsg.dwMsgId = 0x280 + JointNodeID;
	hResult = TransmitData(&tCanMsg);
}

//void MCDC3006_CF::ReceiveThread()
//{
//	HRESULT hResult;
//	UINT32 NodeID;
//
//	while (rtMustQuit == 0)
//	{
//		// read a CAN message from the receive FIFO
//		hResult = canChannelReadMessage(hCanChn, 10, &rCanMsg);
//		if (canChannelWaitRxEvent(hCanChn, RxWaitTime) == VCI_OK) //if( hResult == VCI_OK )
//		{
//			hResult = canChannelPeekMessage(hCanChn, &rCanMsg);
//			printf("\nRecieveData: (Last RxEvent: %dx%ums before)\n",
//				RecieveTimeOutCount, RxWaitTime);
//			RecieveTimeOutCount = 0;
//			ShowData(hResult, &rCanMsg);
//
//
//			if (rCanMsg.dwMsgId > 0x180 && rCanMsg.dwMsgId < 0x200)
//			{
//			}
//			else if (rCanMsg.dwMsgId > 0x280 && rCanMsg.dwMsgId < 0x300)
//			{
//				NodeID = rCanMsg.dwMsgId - 0x280;
//				printf("\n NodeID: %3X \n", NodeID);//debug
//				if (NodeID == JointNodeID)
//				{
//					PDO2DataProcess(rCanMsg.abData);
//					UpdataFlag[0] = 1;
//				}
//			}
//			else if (rCanMsg.dwMsgId > 0x580 && rCanMsg.dwMsgId < 0x600)
//			{
//			}
//		}
//		else
//		{
//			RecieveTimeOutCount++;
//		}
//	}
//	//_endthread();
//	/*void* x;
//	return (_beginthread_proc_type)x;*/
//}

//void MCDC3006_CF::PDO2DataProcess(UINT8 *Data)
//{
//	int num;
//	UINT8 b[4];
//	b[0] = Data[1]; b[1] = Data[2]; b[2] = Data[3]; b[3] = Data[4];
//	printf("\n Data: %.2X %.2X %.2X %.2X %.2X \n", Data[0], b[0], b[1], b[2], b[3]);//debug
//	switch (Data[0])
//	{
//	case 0x40: //POS
//		if (Data[5] == 1) // error byte is ok
//		{
//			CanHexToDec(b, &num);
//			encoder = num;
//			printf("\n %d", encoder);
//
//		}
//		break;
//	case 0x2B: //GN, get actual speed
//		if (Data[5] == 1)
//		{
//			CanHexToDec(b, &num);
//			velocity = num;
//			printf("\n %d", velocity);
//		}
//		break;
//	default:
//		printf("\n POS default \n");//debug
//		break;
//	}
//
//}
