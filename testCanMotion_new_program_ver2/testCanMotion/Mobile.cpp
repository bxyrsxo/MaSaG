#include "Mobile.h"
#include <iostream>
using namespace std;

void Mobile::ReceiveThread() {
	

	while (rtMustQuit == 0)
	{
		// read a CAN message from the receive FIFO
		//hResult = canChannelReadMessage(hCanChn, 10, &rCanMsg);
		//if (canChannelReadMessage(hCanChn, 10, &rCanMsg) == VCI_OK) {
			if (canChannelWaitRxEvent(hCanChn, RxWaitTime) == VCI_OK) //if( hResult == VCI_OK )
			{
				hResult = canChannelPeekMessage(hCanChn, &rCanMsg);

				//printf("\nRecieveData: (Last RxEvent: %dx%ums before)\n",
				//	RecieveTimeOutCount, RxWaitTime);
				//RecieveTimeOutCount = 0;
				//ShowData(hResult, &rCanMsg);
				if (rCanMsg.dwMsgId > 0x280 && rCanMsg.dwMsgId < 0x300)
				{
					//std::cout << "Update your encoders..." << std::endl;
					Update_encoder(rCanMsg.dwMsgId);

				}

			}
			else
			{
				RecieveTimeOutCount++;
			}
		//}
	}
	_endthread();
}
void Mobile::Update_encoder(int NodeID) {
	/*for (size_t i = 0; i < 2; ++i) {
		if (jointHead[i]->driver->NodeID() == NodeID) {
			jointHead[i]->driver->encoder_value = PDO2DataProcess(rCanMsg.abData);
		}
	}*/
	for (size_t i = 0; i < 3; ++i) {
		if (0x280+jointTorso[i]->driver->JointNodeID == NodeID) {
			//std::cout << "encoder_value updates..." << std::endl;
			jointTorso[i]->driver->encoder_value = PDO2DataProcess(rCanMsg.abData);
			std::cout << std::endl;
			std::cout << "Encoder " << "Torso" << i  << ": "<< jointTorso[i]->driver->encoder_value << std::endl;
		}
	}
	/*for (size_t i = 0; i < 4; ++i) {
		if (jointWheel[i]->driver->NodeID() == NodeID) {
			jointWheel[i]->driver->encoder_value = PDO2DataProcess(rCanMsg.abData);
		}
	}*/
}
int Mobile::PDO2DataProcess(UINT8 *Data)
{
	int num;
	UINT8 b[4];
	b[0] = Data[1]; b[1] = Data[2]; b[2] = Data[3]; b[3] = Data[4];
	//printf("\n Data: %.2X %.2X %.2X %.2X %.2X \n", Data[0], b[0], b[1], b[2], b[3]);//debug
	Sleep(10);
	switch (Data[0])
	{
	case 0x40: //POS
		if (Data[5] == 1) // error byte is ok
		{
			CanHexToDec(b, &num);
			//printf("Encoder: %d", num);

		}
		return num;
		break;
	case 0x2B: //GN, get actual speed
		if (Data[5] == 1)
		{
			CanHexToDec(b, &num);
			//printf("\n Velocity: %d", num);
		}
		return num;
		break;
	default:
		//printf("\n POS default \n");//debug
		break;
	}

}
void Mobile::initialize() {

	printf("\n Initializes the CAN with 1000 kBaud");
	printf(" Select Adapter...\r");
	hResult = SelectDevice(FALSE);
	//open a thread for RecieveThread
	begin_thread();
	
	if (VCI_OK == hResult)
	{
		printf(" Select Adapter.......... OK !\n\n");

		printf(" Initialize CAN...\r");
		hResult = InitSocket(lCtrlNo);

		if (VCI_OK == hResult)
		{
			printf(" Initialize CAN............ OK !\n\n");
		}
	}
	for (size_t i = 0; i < 2; ++i) {
		jointHead[i]->driver->initialization();
	}

	for (size_t i = 0; i < 3; ++i) {
		jointTorso[i]->driver->initialization();
	}
	for (size_t i = 0; i < 4; ++i) {
		jointWheel[i]->driver->initialization();
	}

}

void Mobile::keyboard_remote() {
	cout << "Initialize CANopen..." << endl;
	initialize();
	BYTE bChar = {};
	//// wait for keyboard hit transmit  CAN-Messages cyclically
	while (1)
	{

		
		//UIinterface();
		
		cout << endl;
		
		/*for (size_t i = 0; i < 3; ++i) {
			std::cout << "Torso " << i << " " << jointTorso[i]->driver->readEncoder() << std::endl;
		}*/
		cout << "Command... ";
		cin >> bChar;
		////Profile velocity for Mobile
		if ((bChar == 'W') || (bChar == 'w')) {
			move_forward();
		}

		else if ((bChar == 'S') || (bChar == 's')) {
			move_backward();
		}

		else if ((bChar == 'R') || (bChar == 'r')) {
			rotate_clockwise();
		}

		else if ((bChar == 'C') || (bChar == 'c')) {
			rotate_counclockwise();
		}
		////Profile Position for Torso and Head
		else if ((bChar == 'T') || (bChar == 't')) {
			torso_pitch_1(position_cmd);
		}
		if ((bChar == 'Y') || (bChar == 'y')) {
			torso_pitch_2(position_cmd);
		}
		if ((bChar == 'U') || (bChar == 'u')) {
			torso_rotate(position_cmd);
		}
		if ((bChar == 'H') || (bChar == 'h')) {
			head_row(position_cmd);
		}
		if ((bChar == 'J') || (bChar == 'j')) {
			head_pitch(position_cmd);
		}

		//// get position
		else if ((bChar == 'p') || (bChar == 'P'))
		{

		/*	for (size_t i = 0; i < 2; ++i) {
				cout << "Encoder Head: " << i << " : " << jointHead[i]->driver->readEncoder() << endl;
			}
*/
			for (size_t i = 0; i < 3; ++i) {
				jointTorso[i]->driver->readEncoder();
				
				
			}
			
			/*for (size_t i = 0; i < 4; ++i) {
				cout << "Encoder Wheel: " << i << " : " << jointWheel[i]->driver->readEncoder() << endl;
			}*/
			
		}
		//// Set every joint's Velocity to zero
		else if (bChar == 'z' || (bChar == 'Z')) {
			int cmd = 0;
			for (size_t i = 0; i < 2; ++i) {
				jointHead[i]->driver->cmdVelocity(cmd);
			}

			for (size_t i = 0; i < 3; ++i) {
				jointTorso[i]->driver->cmdVelocity(cmd);
			}
			for (size_t i = 0; i < 4; ++i) {
				jointWheel[i]->driver->cmdVelocity(cmd);
			}
		}
		
		////Quit
		else if (bChar == 'q' || bChar == 'Q')
		{
			
			break;
		}
		
		
	}
	InterlockedExchange(&rtMustQuit, 1);
	Sleep(20);
}

void Mobile::move_forward() {
	cout << "Set the mobile velocity... ";
	cin >> mobile_velocity;
	jointWheel[0]->driver->cmdVelocity(mobile_velocity);
	jointWheel[1]->driver->cmdVelocity(-mobile_velocity);
	jointWheel[2]->driver->cmdVelocity(-mobile_velocity);
	jointWheel[3]->driver->cmdVelocity(mobile_velocity);
}

void Mobile::move_backward() {
	cout << "Set the mobile velocity... ";
	cin >> mobile_velocity;
	jointWheel[0]->driver->cmdVelocity(-mobile_velocity);
	jointWheel[1]->driver->cmdVelocity(mobile_velocity);
	jointWheel[2]->driver->cmdVelocity(mobile_velocity);
	jointWheel[3]->driver->cmdVelocity(-mobile_velocity);
}

void Mobile::rotate_clockwise() {
	cout << "Set the mobile velocity... ";
	cin >> mobile_velocity;
	jointWheel[0]->driver->cmdVelocity(mobile_velocity);
	jointWheel[1]->driver->cmdVelocity(mobile_velocity);
	jointWheel[2]->driver->cmdVelocity(mobile_velocity);
	jointWheel[3]->driver->cmdVelocity(mobile_velocity);
}

void Mobile::rotate_counclockwise() {
	cout << "Set the mobile velocity... ";
	cin >> mobile_velocity;
	jointWheel[0]->driver->cmdVelocity(-mobile_velocity);
	jointWheel[1]->driver->cmdVelocity(-mobile_velocity);
	jointWheel[2]->driver->cmdVelocity(-mobile_velocity);
	jointWheel[3]->driver->cmdVelocity(-mobile_velocity);
}

void Mobile::head_row(int cmd) {
	jointHead[0]->driver->cmdPosition(cmd*jointHead[0]->ENC2Rad);
}

void Mobile::head_pitch(int cmd) {
	jointHead[1]->driver->cmdPosition(cmd);
}

void Mobile::torso_pitch_1(int cmd) {
	jointTorso[0]->driver->cmdPosition(cmd);
	//jointTorso[0]->driver->cmdVelocity(cmd);
}

void Mobile::torso_pitch_2(int cmd) {
	jointTorso[1]->driver->cmdPosition(cmd);
	//jointTorso[1]->driver->cmdVelocity(cmd);
}

void Mobile::torso_rotate(int cmd) {
	jointTorso[2]->driver->cmdPosition(cmd);
	//jointTorso[2]->driver->cmdVelocity(cmd);
}

void Mobile::UIinterface() {
	cout << "***************************************" << endl;
	cout << "Press P: get encoder data of each axis" << endl;
	cout << "Press W: Mobile go forward" << endl;
	cout << "Press S: Mobile go backward" << endl;
	cout << "Press R: Mobile rotate clockwise" << endl;
	cout << "Press C: Mobile rotate counterclockwise" << endl;
	cout << "Press Z: all Joint Velocity to zero" << endl;
	cout << "Press T: Torso pitch_1 move" << endl;
	cout << "Press Y: Torso pitch_2 move" << endl;
	cout << "Press U: Torso rotate move" << endl;
	cout << "Press H: Head pitch move" << endl;
	cout << "Press J: Head row move" << endl;
	cout << "Press Q: Quit" << endl;
	cout << "***************************************" << endl;

}