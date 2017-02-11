#ifndef CANCONFUNC_H
#define CANCONFUNC_H

#include <stdio.h>
#include <conio.h>
//#include <process.h>
#include <vcinpl.h>
#include <vciguid.h>

//void    ReceiveThread( void* Param );

HRESULT SelectDevice ( BOOL fUserSelect );
HRESULT InitSocket   ( UINT32 dwCanNo );
void    FinalizeApp  ( void );
void    DisplayError ( HRESULT hResult );

void    ShowData(HRESULT hResult, PCANMSG pCanMsg );
HRESULT TransmitData( PCANMSG pCanMsg );
HRESULT RecieveData( PCANMSG pCanMsg );


extern HANDLE hDevice;       // device handle
extern LONG   lCtrlNo;       // controller number
extern HANDLE hCanCtl;       // controller handle 
extern HANDLE hCanChn;       // channel handle
extern LONG   lMustQuit;	 // quit flag for the receive thread
extern int RecieveTimeOutCount;

extern UINT32		 TxWaitTime;
extern UINT32 TxMaxTimeOutTimes;
extern UINT32		 RxWaitTime;
extern UINT32 RxMaxTimeOutTimes; 

//extern HRESULT hResult;
extern CANMSG tCanMsg;
extern CANMSG rCanMsg;

#endif