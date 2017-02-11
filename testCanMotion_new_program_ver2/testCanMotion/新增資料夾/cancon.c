//////////////////////////////////////////////////////////////////////////
// IXXAT Automation GmbH
//////////////////////////////////////////////////////////////////////////
/**
  Demo application for the IXXAT VCI C-API.

  @file "cancon.c"

  @note 
    This demo demonstrates the following VCI features
    - adapter selection
    - controller initialization 
    - creation of a message channel
    - transmission / reception of CAN messages
*/
//////////////////////////////////////////////////////////////////////////
// (C) 2002-2011 IXXAT Automation GmbH, all rights reserved
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// compiler directives
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////////
#include <vcinpl.h>

#include <process.h>
#include <stdio.h>
#include <conio.h>
#include "select.hpp"

#include "vciguid.h"

//////////////////////////////////////////////////////////////////////////
// static constants, types, macros, variables
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// global variables
//////////////////////////////////////////////////////////////////////////
static HANDLE hDevice;       // device handle
static LONG   lCtrlNo;       // controller number
static HANDLE hCanCtl;       // controller handle 
static HANDLE hCanChn;       // channel handle
static LONG   lMustQuit = 0; // quit flag for the receive thread

//////////////////////////////////////////////////////////////////////////
// static function prototypes
//////////////////////////////////////////////////////////////////////////
HRESULT SelectDevice ( BOOL fUserSelect );
HRESULT InitSocket   ( UINT32 dwCanNo );
void    FinalizeApp  ( void );
void    DisplayError ( HRESULT hResult );

void    TransmitData ( void );
void    ReceiveThread( void* Param );

//////////////////////////////////////////////////////////////////////////
// global functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static functions
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/**
  Main entry point of the application.
*/
//////////////////////////////////////////////////////////////////////////
int main ( )
{
  HRESULT hResult;
  // a variable to get the last pressed key
  BYTE bChar;

  printf(" >>>> VCI - C-API Example V2.0 <<<<\n");
  printf("\n Initializes the CAN with 125 kBaud");
  printf("\n Shows all received messages");
  printf("\n 't'-key send a message with ID 100H");
  printf("\n 'Esc'-key quit the application\n\n");

  printf(" Select Adapter...\r");  
  hResult = SelectDevice( TRUE );

  if ( VCI_OK == hResult )
  {
    printf(" Select Adapter.......... OK !\n\n");

    printf(" Initialize CAN...\r");
    hResult = InitSocket( lCtrlNo );

    if ( VCI_OK == hResult )
    {
      printf(" Initialize CAN............ OK !\n\n");
      
      //
      // start the receive thread
      //
      _beginthread( ReceiveThread, 0, NULL);

      //
      // wait for keyboard hit transmit  CAN-Messages cyclically
      //
      while( 1 )
      { 
        // wait for the user to press a key
        bChar = _getch();

        // when the key is 't' ort 'T' the send a CAN message
        if ( (bChar == 't') || (bChar == 'T') )
          TransmitData();  

        // when the user press the ESC key then end the program
        if (bChar == VK_ESCAPE)
          break;

        Sleep(1);
      } 

      //
      // tell receive thread to quit
      //
      InterlockedExchange(&lMustQuit, 1);
    }
  }

  printf(" Free VCI - Resources...\r");
  FinalizeApp();
  printf(" Free VCI - Resources........ OK !\n\n");

  system("pause");
  return 0;
}

//////////////////////////////////////////////////////////////////////////
/**
  Selects the first CAN adapter.

  @param fUserSelect
    If this parameter is set to TRUE the functions display a dialog box which 
    allows the user to select the device.

  @return
    VCI_OK on success, otherwise an Error code
*/
//////////////////////////////////////////////////////////////////////////
HRESULT SelectDevice( BOOL fUserSelect )
{
  HRESULT hResult; // error code

  if (fUserSelect == FALSE)
  {
    HANDLE        hEnum;   // enumerator handle
    VCIDEVICEINFO sInfo;   // device info

    //
    // open the device list
    //
    hResult = vciEnumDeviceOpen(&hEnum);

    //
    // retrieve information about the first
    // device within the device list
    //
    if (hResult == VCI_OK)
    {
      hResult = vciEnumDeviceNext(hEnum, &sInfo);
    }

    //
    // close the device list (no longer needed)
    //
    vciEnumDeviceClose(hEnum);

    //
    // open the device
    //
    if (hResult == VCI_OK)
    {
      hResult = vciDeviceOpen(&sInfo.VciObjectId, &hDevice);
    }

    //
    // always select controller 0
    //
    lCtrlNo = 0;
  }
  else
  {
    //
    // open a device selected by the user
    //
    hResult = SocketSelectDlg(NULL, VCI_BUS_CAN, &hDevice, &lCtrlNo);
  }

  DisplayError(hResult);
  return hResult;
}

//////////////////////////////////////////////////////////////////////////
/**
  Opens the specified socket, creates a message channel, initializes
  and starts the CAN controller.

  @param dwCanNo
    Number of the CAN controller to open.

  @return
    VCI_OK on success, otherwise an Error code

  @note
    If <dwCanNo> is set to 0xFFFFFFFF, the function shows a dialog box
    which allows the user to select the VCI device and CAN controller.
*/
//////////////////////////////////////////////////////////////////////////
HRESULT InitSocket(UINT32 dwCanNo)
{
  HRESULT hResult;

  //
  // create a message channel
  //
  if (hDevice != NULL)
  {
    //
    // create and initialize a message channel
    //
    hResult = canChannelOpen(hDevice, dwCanNo, FALSE, &hCanChn);

    //
    // initialize the message channel
    //
    if (hResult == VCI_OK)
    {
      UINT16 wRxFifoSize  = 1024;
      UINT16 wRxThreshold = 1;
      UINT16 wTxFifoSize  = 128;
      UINT16 wTxThreshold = 1;

      hResult = canChannelInitialize( hCanChn,
                                      wRxFifoSize, wRxThreshold,
                                      wTxFifoSize, wTxThreshold);
    }

    //
    // activate the CAN channel
    //
    if (hResult == VCI_OK)
    {
      hResult = canChannelActivate(hCanChn, TRUE);
    }

    //
    // open the CAN controller
    //
    if (hResult == VCI_OK)
    {
      hResult = canControlOpen(hDevice, dwCanNo, &hCanCtl);
      // this function fails if the controller is in use
      // by another application.
    }

    //
    // initialize the CAN controller
    //
    if (hResult == VCI_OK)
    { 
      hResult = canControlInitialize(hCanCtl, CAN_OPMODE_STANDARD | CAN_OPMODE_ERRFRAME,
                                     CAN_BT0_125KB, CAN_BT1_125KB);
    }

    //
    // set the acceptance filter
    //
    if (hResult == VCI_OK)
    { 
       hResult = canControlSetAccFilter( hCanCtl, FALSE,
                                         CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);
    }

    //
    // start the CAN controller
    //
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

//////////////////////////////////////////////////////////////////////////
/**
  Transmits a CAN message with ID 0x100.
*/
//////////////////////////////////////////////////////////////////////////
void TransmitData( )
{
  HRESULT hResult;
  CANMSG  sCanMsg;
  UINT8   i;

  sCanMsg.dwTime   = 0;
  sCanMsg.dwMsgId  = 0x100;    // CAN message identifier

  sCanMsg.uMsgInfo.Bytes.bType  = CAN_MSGTYPE_DATA;
  sCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(8,0,0,0,0);
  sCanMsg.uMsgInfo.Bits.srr     = 1;

  for (i = 0; i < sCanMsg.uMsgInfo.Bits.dlc; i++ )
  {
    sCanMsg.abData[i] = i;
  }

  // write the CAN message into the transmit FIFO
  hResult = canChannelSendMessage(hCanChn, INFINITE, &sCanMsg);
}

//////////////////////////////////////////////////////////////////////////
/**
  Receive thread.

  @param Param
    ptr on a user defined information
*/
//////////////////////////////////////////////////////////////////////////
void ReceiveThread( void* Param )
{
  HRESULT hResult;
  CANMSG  sCanMsg;
  
  while ( lMustQuit == 0 )
  {
    // read a CAN message from the receive FIFO

    hResult = canChannelReadMessage(hCanChn, 100, &sCanMsg);

    if (hResult == VCI_OK)
    {
      if (sCanMsg.uMsgInfo.Bytes.bType == CAN_MSGTYPE_DATA)
      {
        //
        // show data frames
        //
        if (sCanMsg.uMsgInfo.Bits.rtr == 0)
        {
          UINT8 j;

          printf("\nTime: %10u  ID: %3X  DLC: %1u  Data:",
                  sCanMsg.dwTime,
                  sCanMsg.dwMsgId,
                  sCanMsg.uMsgInfo.Bits.dlc);

          for (j = 0; j < sCanMsg.uMsgInfo.Bits.dlc; j++)
          {
            printf(" %.2X", sCanMsg.abData[j]);
          }
        }
        else
        {
          printf("\nTime: %10u ID: %3X  DLC: %1u  Remote Frame",
                 sCanMsg.dwTime,
                 sCanMsg.dwMsgId,
                 sCanMsg.uMsgInfo.Bits.dlc);
        }
      }
      else if (sCanMsg.uMsgInfo.Bytes.bType == CAN_MSGTYPE_INFO)
      {
        //
        // show informational frames
        //
        switch (sCanMsg.abData[0])
        {
          case CAN_INFO_START: printf("\nCAN started..."); break;
          case CAN_INFO_STOP : printf("\nCAN stoped...");  break;
          case CAN_INFO_RESET: printf("\nCAN reseted..."); break;
        }
      }
      else if (sCanMsg.uMsgInfo.Bytes.bType == CAN_MSGTYPE_ERROR)
      {
        //
        // show error frames
        //
        switch (sCanMsg.abData[0])
        {
          case CAN_ERROR_STUFF: printf("\nstuff error...");          break; 
          case CAN_ERROR_FORM : printf("\nform error...");           break; 
          case CAN_ERROR_ACK  : printf("\nacknowledgment error..."); break;
          case CAN_ERROR_BIT  : printf("\nbit error...");            break; 
          case CAN_ERROR_CRC  : printf("\nCRC error...");            break; 
          case CAN_ERROR_OTHER:
          default             : printf("\nother error...");          break;
        }
      }
    }
  }

  _endthread();
}

//////////////////////////////////////////////////////////////////////////
/**
  Finalizes the application
*/
//////////////////////////////////////////////////////////////////////////
void FinalizeApp()
{
  //
  // close all open handles

  canControlReset(hCanCtl);
  canChannelClose(hCanChn);
  canControlClose(hCanCtl);

  vciDeviceClose(hDevice);
}

//////////////////////////////////////////////////////////////////////////
/**
  This function displays a message box for the specified error code.

  @param hResult
    Error code or -1 to display the error code returned by GetLastError().
*/
//////////////////////////////////////////////////////////////////////////
void DisplayError(HRESULT hResult)
{
  char szError[VCI_MAX_ERRSTRLEN];

  if (hResult != NO_ERROR)
  {
    if (hResult == -1)
      hResult = GetLastError();

    szError[0] = 0;
    vciFormatError(hResult, szError, sizeof(szError));
    MessageBox(NULL, szError, "VCI Demo", MB_OK | MB_ICONSTOP);
  }
}
