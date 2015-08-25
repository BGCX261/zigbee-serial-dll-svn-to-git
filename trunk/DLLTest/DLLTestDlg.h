
// DLLTestDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CDLLTestDlg dialog
class CDLLTestDlg : public CDialogEx
{


// Construction
public:
	CDLLTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DLLTEST_DIALOG };

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
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg LRESULT receivePacket(WPARAM wParam, LPARAM lParam);
	CRichEditCtrl m_cLogbox;
	CString m_strLogbox;
	void addLog(CString szMsg, int nCondition);
	void AddLogString( const TCHAR* str );
	afx_msg void OnBnClickedButton4();
	CString m_command_1;
	CString m_command_2;
	CString m_devid_1;
	CString m_devid_2;
	char* GetPortName(int index);
	afx_msg void OnBnClickedButton5();
	CString m_strCommconnect;
	CString m_strColumnbar;
	CString m_strData;
	byte StringToByte(CString str);
};
