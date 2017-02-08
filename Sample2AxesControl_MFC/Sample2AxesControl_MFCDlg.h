
// Sample2AxesControl_MFCDlg.h : header file
//

#pragma once

#include"nex_type.h"
#include "Nexecm.h"
#include "NexCoeMotion.h"


// CSample2AxesControl_MFCDlg dialog
class CSample2AxesControl_MFCDlg : public CDialogEx
{
// Construction
public:
	CSample2AxesControl_MFCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SAMPLE2AXESCONTROL_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	UINT    TimerHandle;
	U16_T   misUpdate;
	afx_msg void OnBnClickedCancel();
};
