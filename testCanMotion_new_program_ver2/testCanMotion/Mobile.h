#ifndef MOBILE_H
#define MOBILE_H
#include "Joint.h"
#include "SystemParams.h"
class Mobile {

public:

	Joint* jointHead[2];
	Joint* jointTorso[3];
	Joint* jointWheel[4];
	HRESULT hResult;
	void ReceiveThread();
	void Update_encoder(int NodeID);
	int PDO2DataProcess(UINT8 *Data);
	void begin_thread()
	{
		_beginthread(&Mobile::ReceiveThread_wrapper, 0, static_cast<void*>(this));
		/*_beginthread(&Mobile::ReceiveThread_wrapper, 0, NULL);*/
	}
	static void __cdecl ReceiveThread_wrapper(void* o)
	{
		static_cast<Mobile*>(o)->ReceiveThread();
	}
	Mobile() {
		jointHead[0] = new Joint(resEncoder_Head[0], gearRatio_Head[0], TYPE_MCDC3006_CF,0); //row
		jointHead[1] = new Joint(resEncoder_Head[1], gearRatio_Head[1], TYPE_MCDC3006_CF,1); //pitch

		jointTorso[0] = new Joint(resEncoder_Torso[0], gearRatio_Torso[0], TYPE_MCDC3006_CF,9); //pitch_1
		jointTorso[1] = new Joint(resEncoder_Torso[1], gearRatio_Torso[1], TYPE_MCDC3006_CF,7); //pitch_2
		jointTorso[2] = new Joint(resEncoder_Torso[2], gearRatio_Torso[2], TYPE_MCDC3006_CF,16); //rotate

		jointWheel[0] = new Joint(resEncoder_Wheel[0], gearRatio_Wheel[0], TYPE_MCDC3006_CF,2); // 玡近
		jointWheel[1] = new Joint(resEncoder_Wheel[1], gearRatio_Wheel[1], TYPE_MCDC3006_CF,3); // オ玡近
		jointWheel[2] = new Joint(resEncoder_Wheel[2], gearRatio_Wheel[2], TYPE_MCDC3006_CF,4); //オ近
		jointWheel[3] = new Joint(resEncoder_Wheel[3], gearRatio_Wheel[3], TYPE_MCDC3006_CF,5); //近
	}

	~Mobile() {
		delete jointHead[0];
		delete jointHead[1];
		delete jointTorso[0];
		delete jointTorso[1];
		delete jointTorso[2];
		for (int i = 0; i < 4; i++)
			delete jointWheel[i];
	}

	void initialize();

	//// For the Mobile part
	void move_forward();

	void move_backward();

	void rotate_clockwise();

	void rotate_counclockwise();

	void keyboard_remote();

	//// Profile Position for Head and Torso
	void head_row(int position_cmd);

	void head_pitch(int position_cmd);

	void torso_pitch_1(int position_cmd);

	void torso_pitch_2(int position_cmd);

	void torso_rotate(int position_cmd);

	void UIinterface();
private:
	int mobile_velocity = 0;
	int position_cmd = 5000;

};
#endif // !MOBILE_H