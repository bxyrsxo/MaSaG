#include "canconfunc.h"

HANDLE hDevice;       // device handle
LONG   lCtrlNo;       // controller number
HANDLE hCanCtl;       // controller handle 
HANDLE hCanChn;       // channel handle
//LONG   lMustQuit = 0; // quit flag for the receive thread
//int RecieveTimeOutCount = 0;

UINT32		  TxWaitTime = 4;
UINT32 TxMaxTimeOutTimes = 4;
UINT32		  RxWaitTime = 8;
UINT32 RxMaxTimeOutTimes = 20;

//HRESULT hResult;
CANMSG tCanMsg;
CANMSG rCanMsg;




HRESULT SelectDevice ( BOOL fUserSelect )
{
	HRESULT hResult; // error code

	if (fUserSelect == FALSE)
	{
		HANDLE        hEnum;   // enumerator handle
		VCIDEVICEINFO sInfo;   // device info

		// open the device list
		hResult = vciEnumDeviceOpen(&hEnum);

		// retrieve information about the first
		// device within the device list
		if (hResult == VCI_OK)
		{
		  hResult = vciEnumDeviceNext(hEnum, &sInfo);
		}

		// close the device list (no longer needed)
		vciEnumDeviceClose(hEnum);

		// open the device
		if (hResult == VCI_OK)
		{
		  hResult = vciDeviceOpen(sInfo.VciObjectId, &hDevice);
		}

		// always select controller 0
		lCtrlNo = 0;
	}
	else
	{
		// open a device selected by the user
		//hResult = SocketSelectDlg(NULL, VCI_BUS_CAN, &hDevice, &lCtrlNo);
	}

	DisplayError(hResult);
	return hResult;
}


HRESULT InitSocket   ( UINT32 dwCanNo )
{
	HRESULT hResult;

	// create a message channel
	if (hDevice != NULL)
	{
		// create and initialize a message channel
		hResult = canChannelOpen(hDevice, dwCanNo, FALSE, &hCanChn);

		// initialize the message channel
		if (hResult == VCI_OK)
		{
			UINT16 wRxFifoSize  = 128;//1024
			UINT16 wRxThreshold = 1;
			UINT16 wTxFifoSize  = 16;//128
			UINT16 wTxThreshold = 1;

			hResult = canChannelInitialize( hCanChn,
											wRxFifoSize, wRxThreshold,
											wTxFifoSize, wTxThreshold);
		}

		// activate the CAN channel
		if (hResult == VCI_OK)
		{
			hResult = canChannelActivate(hCanChn, TRUE);
		}

		// open the CAN controller
		if (hResult == VCI_OK)
		{
			hResult = canControlOpen(hDevice, dwCanNo, &hCanCtl);
			// this function fails if the controller is in use
			// by another application.
		}

		// initialize the CAN controller
		if (hResult == VCI_OK)
		{ 
			hResult = canControlInitialize( hCanCtl, CAN_OPMODE_STANDARD | CAN_OPMODE_ERRFRAME,
											CAN_BT0_1000KB, CAN_BT1_1000KB);
		}

		// set the acceptance filter
		if (hResult == VCI_OK)
		{ 
			hResult = canControlSetAccFilter( hCanCtl, FALSE,
											CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);
		}

		// start the CAN controller
		if (hResult == VCI_OK)
		{
			hResult = canControlStart(hCanCtl, TRUE);
		}
	}
	else
	{
		hResult = VCI_E_INVHANDLE;
	}

	DisplayError(hResult);
	return hResult;
}


void FinalizeApp( void )
{
	// close all open handles

	canControlReset(hCanCtl);
	canChannelClose(hCanChn);
	canControlClose(hCanCtl);

	vciDeviceClose(hDevice);
}


void DisplayError(HRESULT hResult)
{
	char szError[VCI_MAX_ERRSTRLEN];

	if (hResult != NO_ERROR)
	{
		if (hResult == -1)
			hResult = GetLastError();

		szError[0] = 0;
		vciFormatError(hResult, szError, sizeof(szError));
		MessageBoxA(NULL, szError, "VCI Demo", MB_OK | MB_ICONSTOP);
	}
}


void ShowData( HRESULT hResult, PCANMSG pCanMsg )
{
	if (hResult == VCI_OK)
	{
		if (pCanMsg->uMsgInfo.Bytes.bType == CAN_MSGTYPE_DATA)
		{
			// show data frames
			if (pCanMsg->uMsgInfo.Bits.rtr == 0)
			{
				UINT8 j;

				/*printf("Time: %10u  ID: %3X  Data:",
						pCanMsg->dwTime, pCanMsg->dwMsgId);*/

				for (j = 0; j < pCanMsg->uMsgInfo.Bits.dlc; j++)
				{
					//printf(" %.2X", pCanMsg->abData[j]);
				}

				/*printf("  DLC:%1u OVR:%1u SRR:%1u RTR:%1u\n",
						pCanMsg->uMsgInfo.Bits.dlc, pCanMsg->uMsgInfo.Bits.ovr,
						pCanMsg->uMsgInfo.Bits.srr, pCanMsg->uMsgInfo.Bits.rtr);*/
			}
			else
			{
				/*printf("Time: %10u ID: %3X  Remote Frame  DLC:%1u OVR:%1u SRR:%1u RTR:%1u\n",
						pCanMsg->dwTime, pCanMsg->dwMsgId,
						pCanMsg->uMsgInfo.Bits.dlc, pCanMsg->uMsgInfo.Bits.ovr,
						pCanMsg->uMsgInfo.Bits.srr, pCanMsg->uMsgInfo.Bits.rtr);*/
			}
		}
		else if (pCanMsg->uMsgInfo.Bytes.bType == CAN_MSGTYPE_INFO)
		{
			// show informational frames
			switch (pCanMsg->abData[0])
			{
				case CAN_INFO_START: printf("CAN started...\n"); break;
				case CAN_INFO_STOP : printf("CAN stoped...\n");  break;
				case CAN_INFO_RESET: printf("CAN reseted...\n"); break;
			}
		}
		else if (pCanMsg->uMsgInfo.Bytes.bType == CAN_MSGTYPE_ERROR)
		{
			// show error frames
			switch (pCanMsg->abData[0])
			{
				case CAN_ERROR_STUFF: printf("stuff error...\n");          break; 
				case CAN_ERROR_FORM : printf("form error...\n");           break; 
				case CAN_ERROR_ACK  : printf("acknowledgment error...\n"); break;
				case CAN_ERROR_BIT  : printf("bit error...\n");            break; 
				case CAN_ERROR_CRC  : printf("CRC error...\n");            break; 
				case CAN_ERROR_OTHER:
				default             : printf("other error...\n");          break;
			}
		}
	}
	else
	{
		//DisplayError( hResult );
		//printf("hResult: %d\n", hResult);
	}
}


HRESULT TransmitData( PCANMSG pCanMsg )
{
	HRESULT hResult;
	BOOL QuitFlag = 0;
	int i = 1;

	while ( QuitFlag == 0 )
	{
		//hResult = canChannelSendMessage(hCanChn, 10, pCanMsg);
		hResult = canChannelWaitTxEvent(hCanChn, TxWaitTime);//
		if( hResult == VCI_OK )
		{
			hResult = canChannelPostMessage(hCanChn, pCanMsg);//
			//printf("\nTransmitData: (%ums TimeOut Times: %d)\n", TxWaitTime, i );
			ShowData( hResult, pCanMsg);
			QuitFlag = 1;
		}
		else
		{
			i++;
			if( i >= TxMaxTimeOutTimes )
			{
				//printf("\nTransmitData TimeOut!! (%ums TimeOut Times: %d)\n", TxWaitTime, i );
				QuitFlag = 1;
			}
		}
	}
	return hResult;
}


HRESULT RecieveData( PCANMSG pCanMsg )
{
	HRESULT hResult;
	BOOL QuitFlag = 0;
	int i = 0;

	while ( QuitFlag == 0 )
	{
		//hResult = canChannelReadMessage(hCanChn, 10, pCanMsg);
		hResult = canChannelWaitRxEvent(hCanChn, RxWaitTime);//
		if( hResult == VCI_OK )
		{
			hResult = canChannelPeekMessage(hCanChn, pCanMsg);//
			//printf("\nRecieveData: (%ums TimeOut Times: %d)\n", RxWaitTime, i );
			ShowData( hResult, pCanMsg);
			QuitFlag = 1;
		}
		else
		{
			i++;
			if( i >= RxMaxTimeOutTimes )
			{
				//printf("\nRecieveData TimeOut!! (%ums TimeOut Times: %d)\n", RxWaitTime, i );
				QuitFlag = 1;
			}
		}
		
	}
	return hResult;
}



