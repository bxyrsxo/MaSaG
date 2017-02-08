
// Sample2AxesControl_MFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sample2AxesControl_MFC.h"
#include "Sample2AxesControl_MFCDlg.h"
#include "afxdialogex.h"
#include <windows.h>
#include <rtapi.h>

static  PVOID  location;

#define EVN_NAME "Event"
#define SHM_NAME "Memory"

#define TOTAL_AXES  (2)

#define joint_number_R 7
#define joint_number_L 7

typedef	struct
{
	//Master
	U16_T masterId;
	U16_T MasterState;
	//Motors for RIGHT arm
	CANAxis_T mAxis_R[joint_number_R];
	I32_T ActualPos_R[joint_number_R];
	I32_T TargetPos_R[joint_number_R];
	I16_T ActualTorque_R[joint_number_R];
	I16_T TargetTorque_R[joint_number_R];
	U16_T SlaveState_R[joint_number_R];
	U8_T  Move_Type_R[joint_number_R];
	U32_T ACC_DEC_R1;
	U32_T Velocity_R1;
	//Motors for LEFT arm
	CANAxis_T mAxis_L[joint_number_L];
	I32_T ActualPos_L[joint_number_L];
	I32_T TargetPos_L[joint_number_L];
	I16_T ActualTorque_L[joint_number_L];
	I16_T TargetTorque_L[joint_number_L];
	U16_T SlaveState_L[joint_number_L];
	U8_T  Move_Type_L[joint_number_L];
	U32_T ACC_DEC_L1;
	U32_T Velocity_L1;
	//Other parameters
	I32_T Mode;
	I8_T State_ing[20];
	U32_T  runTime;
	BOOL_T  startFlag;

}USER_DAT;

U16_T masterId ;

RTN_ERR ret;
I32_T   timeOut = 5000;
U8_T StartCheck = 0;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSample2AxesControl_MFCDlg dialog




CSample2AxesControl_MFCDlg::CSample2AxesControl_MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSample2AxesControl_MFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSample2AxesControl_MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSample2AxesControl_MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Start, &CSample2AxesControl_MFCDlg::OnBnClickedButtonStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CSample2AxesControl_MFCDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSample2AxesControl_MFCDlg message handlers

BOOL CSample2AxesControl_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	ret = NEC_LoadRtxApp( "C:\\Program Files\\NEXCOM\\NexECMRtx\\Lib\\NexECMRtx\\x32\\NexECMRtx.rtss" );
	if( ret != 0 ) { MessageBox("NEC_LoadRtxApp NexECMRtx.rtss failed!");}

	Sleep(2000);

	this->misUpdate = 0;
	this->TimerHandle = SetTimer( NULL, 100, NULL );

	SetDlgItemText( IDC_EDIT_POS, "50000" ); 
	SetDlgItemText( IDC_EDIT_V, "100000" );
	SetDlgItemText( IDC_EDIT_A, "100000" );

	SetDlgItemText( IDC_EDIT_POS2, "50000" ); 
	SetDlgItemText( IDC_EDIT_V2, "100000" );
	SetDlgItemText( IDC_EDIT_A2, "100000" );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSample2AxesControl_MFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSample2AxesControl_MFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSample2AxesControl_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSample2AxesControl_MFCDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here
	I32_T   timeOut = 5000;
	I32_T   i = 0;
	I32_T POS = 0 , VEL = 0 , ACC = 0 , POS2 = 0 , VEL2 = 0 , ACC2 = 0;
	CString str , str_V , str_A , str2 , str_V2 , str_A2  ;
	
	HANDLE sHServo_on = NULL;
	HANDLE oBStart_Master = NULL;

	ret = NEC_StartDriver();
	if (ret != 0) { MessageBox("NEC_StarDriver failed!"); NEC_StopDriver(); return; }

	ret = NEC_GetRtMasterId(&masterId);
	if (ret != 0) { MessageBox("NEC_GetRtMasterId failed!"); NEC_StopDriver(); return; }

	ret = NEC_ResetEcMaster(masterId);
	if (ret != 0) { MessageBox("NEC_ResetEcMaster failed!"); NEC_StopDriver(); return; }

	ret = NEC_LoadNetworkConfig(masterId, "C:\\ENI_NexCAT_Export.xml", START_NETWORK_OPT_MASK_NIC_PORT);
	if (ret != 0) { MessageBox("NEC_StartNetworkEx failed!"); NEC_StopDriver(); return; }

	NEC_StopDriver();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////RtOpenSharedMemory///////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ret = NEC_LoadRtxApp("C:\\Program Files\\NEXCOM\\NexECMRtx\\Samples\\RTX2016\\Sample_2Layer\\VS2015\\Sample2AxesControl_MFC\\RtssDebug\\Sample2AxesControl_RTX.rtss");
	if (ret != 0) { MessageBox("NEC_LoadRtxApp RTX_app failed!"); NEC_StopDriver(); return; }

	sHServo_on = RtOpenSharedMemory( SHM_MAP_ALL_ACCESS , 0 , SHM_NAME , &location );
	//if( oBStart_Master == NULL ) { return ; }

	USER_DAT *p;
	p = (USER_DAT *) location;
	
	GetDlgItemText( IDC_EDIT_POS, str );
	GetDlgItemText( IDC_EDIT_V, str_V );
	GetDlgItemText( IDC_EDIT_A, str_A );

	POS = strtol( str, 0, 10 );
	VEL = strtol( str_V, 0, 10 );
	ACC = strtol( str_A, 0, 10 );

/*	p -> TargetPos[0] = POS;
	p ->Velocity[0] = VEL;
	p ->ACC_DEC[0] = ACC*/;

	GetDlgItemText( IDC_EDIT_POS2, str2 );
	GetDlgItemText( IDC_EDIT_V2, str_V2 );
	GetDlgItemText( IDC_EDIT_A2, str_A2 );

	POS2 = strtol( str2, 0, 10 );
	VEL2 = strtol( str_V2, 0, 10 );
	ACC2 = strtol( str_A2, 0, 10 );

	//p -> TargetPos[1] = POS2;
	//p ->Velocity[1] = VEL2;
	//p ->ACC_DEC[1] = ACC2;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////RtOpenEvent//////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	oBStart_Master = RtOpenEvent( NULL, 0, EVN_NAME);
	if( oBStart_Master == NULL ) { return ; }
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////RtSetEvent//////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	RtSetEvent(oBStart_Master);

	this->misUpdate = 1;

}


void CSample2AxesControl_MFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CString strText;
	if( this->misUpdate )
	{
		USER_DAT *p;
		p = (USER_DAT *) location;

		//strText.Format(_T("%d"), p -> ActualPos[0] );
		//GetDlgItem(IDC_EDIT_REAL_POS)->SetWindowText(strText);

		//strText.Format(_T("%d"), p ->SlaveState[0]);
		//GetDlgItem(IDC_EDIT_STATE)->SetWindowText(strText);

		//strText.Format(_T("%d"), p -> ActualPos[1] );
		//GetDlgItem(IDC_EDIT_REAL_POS2)->SetWindowText(strText);

		//strText.Format(_T("%d"), p ->SlaveState[1]);
		//GetDlgItem(IDC_EDIT_STATE2)->SetWindowText(strText);

		//strText.Format(_T("%d"), p ->MasterState);
		//GetDlgItem(IDC_EDIT_STATE_MASTER_STATE)->SetWindowText(strText);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CSample2AxesControl_MFCDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}
