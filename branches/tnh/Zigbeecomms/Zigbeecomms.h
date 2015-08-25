// Zigbeecomms.h : main header file for the Zigbeecomms DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "CommThread.h"

// CZigbeecommsApp
// See Zigbeecomms.cpp for the implementation of this class
//

class CZigbeecommsApp : public CWinApp
{
public:
	CZigbeecommsApp();

// Overrides
public:
	virtual BOOL InitInstance();

	//HWND mHwnd;
	CCommThread mComm;
	//extern function
	//BOOL	OpenPort(CString strPortName, DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity );
	//void	ClosePort();
	//DWORD	WriteComm(BYTE *pBuff, DWORD nToWrite);
	//BYTE* GetRealDataArray();
	//BOOL createComms(HWND hwnd);
	/////////////////////////////////////////
	DECLARE_MESSAGE_MAP()
};

