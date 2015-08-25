// Zigbeecomms.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Zigbeecomms.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CZigbeecommsApp

BEGIN_MESSAGE_MAP(CZigbeecommsApp, CWinApp)
END_MESSAGE_MAP()


// CZigbeecommsApp construction

CZigbeecommsApp::CZigbeecommsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	
}


// The one and only CZigbeecommsApp object

CZigbeecommsApp theApp;


// CZigbeecommsApp initialization

BOOL CZigbeecommsApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

BOOL zbcreateComms(HWND hwnd){
	
	//theApp.mHwnd = hwnd;
	theApp.mComm.SetreturnHandle(hwnd);

	return TRUE;
}

BOOL zbOpenPort(char* strPortName, DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity )
{
	if(theApp.mComm.m_bConnected == TRUE)
	{
		theApp.mComm.ClosePort();
	}

	char szData[100];
	ZeroMemory(szData, sizeof(szData));
	sprintf_s(szData,100,strPortName);
	return theApp.mComm.OpenPort(szData,dwBaud,byData,byStop,byParity);
}
void zbClosePort()
{
	if(theApp.mComm.m_bConnected == TRUE)
		theApp.mComm.ClosePort();
}

DWORD zbWriteComm(BYTE *pBuff, DWORD nToWrite)
{	
	return theApp.mComm.WriteComm(pBuff,nToWrite);
}

RECEIVEDATA zvGetRcvData(){

	return theApp.mComm.GetRCVdata();
}


BYTE* zbGetReadData()
{
	return theApp.mComm.GetReadData();
	//return theApp.mComm.GetRealDataArray();
}

DWORD PKTtoBYTEARRAY(SERIAL_PKT pkt, BYTE* buff){

	BYTE* arr = new BYTE[pkt.LEN+4];

	arr[0] = pkt.STX;
	arr[1] = pkt.LEN;

	BYTE chksum=0;
	for(int i=0;i<pkt.LEN;i++){
		arr[2+i] = pkt.BUF[i];
		chksum += pkt.BUF[i];
	}

	arr[pkt.LEN+2] = pkt.SUM;
	arr[pkt.LEN+3] = pkt.ETX;

	DWORD length = pkt.LEN+4;

	memcpy(buff,arr,length);
	
	delete[] arr;

	return length;
}

//기록데이터전송
BOOL zbDataWrite(BYTE devid[2], BYTE lineid[2], BYTE PKTdata[30]){ 
	//2byte command, 30byte data
	//  [2/9/2012 sbhwang] devid 추가
	//2byte devid, 2byte command, 30byte data
	
	SERIAL_PKT sendpkt;		
	BYTE PKTtype[5] = {0x80,0x80,0x10,0x23,0x05};
	
	memset(&sendpkt,NULL,sizeof(sendpkt));

	memcpy(&sendpkt.BUF[0],PKTtype,5);
	memcpy(&sendpkt.BUF[5],devid,2);
	memcpy(&sendpkt.BUF[7],lineid,2);
	memcpy(&sendpkt.BUF[9],PKTdata,30);

	BYTE cnt=0x00; 
	BYTE chksum=0x00;
	//while(sendpkt.BUF[cnt] != NULL || cnt > 128){
	while(cnt < 0x27){
		chksum += sendpkt.BUF[cnt];
		cnt++;				
	}

	sendpkt.STX = 0xAA;
	sendpkt.LEN = cnt;
	sendpkt.SUM = chksum;
	sendpkt.ETX = 0x55;
	
	BYTE* temp = new BYTE[sendpkt.LEN+4];
	DWORD len = PKTtoBYTEARRAY(sendpkt,temp);

	memcpy(theApp.mComm.TxPktBuff.PKT,temp,len);
	theApp.mComm.TxPktBuff.LENGTH = len;
	
	//memcpy(temp, &sendpkt,sendpkt.LEN+4);

	BOOL bResult = FALSE;
	if(zbWriteComm(temp,len) == len){
		theApp.mComm.mCommand = WRITEDATA_CMD;
		bResult = TRUE;
	}
	if(bResult)
		theApp.mComm.createtimeoutThread();

	delete[] temp;

	return bResult;
}

//데이터 읽어오기
BOOL zbDataRead(BYTE devid[2],BYTE lineid[2]){
					/*											  dID[1]dID[2]  command	  lID[2]lID[2] chk			*/	   
	BYTE sendpkt[15] = {0xAA, 0x0B, 0x80, 0x80, 0x10, 0x07, 0x05, 0x00, 0x00, 0x88, 0x10, 0x00, 0x00, 0x00, 0x55};

	sendpkt[7] = devid[0];
	sendpkt[8] = devid[1];
	sendpkt[11] = lineid[0];
	sendpkt[12] = lineid[1];

	BYTE chksum = 0x00;
	for(int i=0; i<11; i++){
		chksum += sendpkt[2+i];
	}
	sendpkt[13] = chksum;

	//  [2/16/2012 sbhwang]
	int len = sizeof(sendpkt);

	BOOL bResult = FALSE;
	if(theApp.mComm.m_bCmdMode)
	{
		if(zbWriteComm(sendpkt,15) == 15){
			theApp.mComm.mCommand = READDATA_CMD;
			bResult = TRUE;
		}
	}
	else
	{
		memcpy(theApp.mComm.TxPktBuff.PKT,sendpkt,len);
		theApp.mComm.TxPktBuff.LENGTH = len;
		bResult = TRUE;
	}

	return bResult;
}


//통신 종료하기
BOOL zbCommOver(BYTE devid[2],BYTE lineid[2]){
					/*											  dID[1]dID[2]  command	  lID[2]lID[2] chk			*/
	BYTE sendpkt[15] = {0xAA, 0x0B, 0x80, 0x80, 0x10, 0x07, 0x05, 0x00, 0x00, 0xAA, 0x10, 0x00, 0x00, 0x00, 0x55};

	sendpkt[7] = devid[0];
	sendpkt[8] = devid[1];
	sendpkt[11] = lineid[0];
	sendpkt[12] = lineid[1];

	BYTE chksum = 0x00;
	for(int i=0; i<11; i++){
		chksum += sendpkt[2+i];
	}
	sendpkt[13] = chksum;
	BOOL bResult = FALSE;
	if(zbWriteComm(sendpkt,15) == 15){
		theApp.mComm.mCommand = COMMOVER_CMD;
		bResult = TRUE;
	}
	if(bResult)
		theApp.mComm.createtimeoutThread();

	return bResult;
}

int ComPortScan(void){
	int nPort = 0;

	bool bRtn = false;

	CString strReg="";
	DWORD dwType = REG_SZ;
	DWORD dwSize = 128;
	HKEY hKey;
	TCHAR szBuffer[128] = {'\0' ,};

	CString strPort;
	LONG  regRead = -1;

	int i;
	int count = 0;
	LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE , "HARDWARE\\DEVICEMAP\\SERIALCOMM" , 0 , KEY_READ , &hKey);
	if(ERROR_SUCCESS == lResult)
	{

		for(i=0; i<10; i++)
		{
			strPort.Format("\\Device\\cyg_ser%d", i);

			regRead = RegQueryValueEx(hKey , (LPCTSTR)strPort , NULL , &dwType , (LPBYTE)szBuffer , &dwSize);
			if(regRead == 0)
			{
				strReg.Format("%s", szBuffer);
				bRtn = true;
				break;

			}
		}
		if(!bRtn)
		{

			for(i=0; i<10; i++)
			{
				strPort.Format("\\Device\\slabser%d", i);

				regRead = RegQueryValueEx(hKey , (LPCTSTR)strPort , NULL , &dwType , (LPBYTE)szBuffer , &dwSize);
				if(regRead == 0)
				{
					strReg.Format("%s", szBuffer);
					bRtn = true;
					break;

				}
			}
		}
		if(!bRtn)
		{

			for(i=0; i<10; i++)
			{
				strPort.Format("\\Device\\Silabser%d", i);

				regRead = RegQueryValueEx(hKey , (LPCTSTR)strPort , NULL , &dwType , (LPBYTE)szBuffer , &dwSize);
				if(regRead == 0)
				{
					strReg.Format("%s", szBuffer);
					bRtn = true;
					break;
				}
			}
		}
	}

	if(bRtn)
	{
		CString strPortMsg;

		int length = strReg.GetLength();
		strPortMsg = strReg.Mid(3, length);

		nPort = atoi(strPortMsg);
	}

	return nPort;

}

//기록데이터전송
BOOL zbDataWriteForCSharp(BYTE *devid, BYTE *lineid, BYTE *PKTdata){ 
	//2byte command, 30byte data
	//  [2/9/2012 sbhwang] devid 추가
	//2byte devid, 2byte command, 30byte data

	SERIAL_PKT sendpkt;		
	BYTE PKTtype[5] = {0x80,0x80,0x10,0x23,0x05};

	memset(&sendpkt,NULL,sizeof(sendpkt));

	memcpy(&sendpkt.BUF[0],PKTtype,5);
	memcpy(&sendpkt.BUF[5],devid,2);
	memcpy(&sendpkt.BUF[7],lineid,2);
	memcpy(&sendpkt.BUF[9],PKTdata,30);

	BYTE cnt=0x00;
	BYTE chksum=0x00;
	//while(sendpkt.BUF[cnt] != NULL || cnt > 128){
	while(cnt < 0x27){
		chksum += sendpkt.BUF[cnt];
		cnt++;				
	}

	sendpkt.STX = 0xAA;
	sendpkt.LEN = cnt;
	sendpkt.SUM = chksum;
	sendpkt.ETX = 0x55;

	BYTE* temp = new BYTE[sendpkt.LEN+4];
	DWORD len = PKTtoBYTEARRAY(sendpkt,temp);

	memcpy(theApp.mComm.TxPktBuff.PKT,temp,len);
	theApp.mComm.TxPktBuff.LENGTH = len;


	BOOL bResult = FALSE;
	if(zbWriteComm(temp,len) == len){
		theApp.mComm.mCommand = WRITEDATA_CMD;
		bResult = TRUE;
	}
	if(bResult)
		theApp.mComm.createtimeoutThread();

	delete[] temp;

	return bResult;
}

//데이터 읽어오기
BOOL zbDataReadForCSharp(BYTE *devid, BYTE *lineid){

	/*											  dID[1]dID[2]  command	  lID[2]lID[2] chk			*/	   
	BYTE sendpkt[15] = {0xAA, 0x0B, 0x80, 0x80, 0x10, 0x07, 0x05, 0x00, 0x00, 0x88, 0x10, 0x00, 0x00, 0x00, 0x55};


	//sendpkt[8] = command;
	sendpkt[7] = devid[0];
	sendpkt[8] = devid[1];
	sendpkt[11] = lineid[0];
	sendpkt[12] = lineid[1];

	BYTE chksum = 0x00;
	for(int i=0; i<11; i++){
		chksum += sendpkt[2+i];
	}
	sendpkt[13] = chksum;

	//  [2/16/2012 sbhwang]
	int len = sizeof(sendpkt);

	memcpy(theApp.mComm.TxPktBuff.PKT,sendpkt,len);
	theApp.mComm.TxPktBuff.LENGTH = len;
	//////////////////////////////////////////////////////////////////////////

	BOOL bResult = FALSE;
	if(zbWriteComm(sendpkt,15) == 15){
		theApp.mComm.mCommand = READDATA_CMD;
		bResult = TRUE;
	}
	if(bResult)
		theApp.mComm.createtimeoutThread();

	return bResult;
}

//통신 종료하기
BOOL zbCommOverForCSharp(BYTE *devid, BYTE *lineid){
	/*											  dID[1]dID[2]  command	  lID[2]lID[2] chk			*/
	BYTE sendpkt[15] = {0xAA, 0x0B, 0x80, 0x80, 0x10, 0x07, 0x05, 0x00, 0x00, 0xAA, 0x10, 0x00, 0x00, 0x00, 0x55};

	sendpkt[7] = devid[0];
	sendpkt[8] = devid[1];
	sendpkt[11] = lineid[0];
	sendpkt[12] = lineid[1];

	BYTE chksum = 0x00;
	for(int i=0; i<11; i++){
		chksum += sendpkt[2+i];
	}
	sendpkt[13] = chksum;
	BOOL bResult = FALSE;
	if(zbWriteComm(sendpkt,15) == 15){
		theApp.mComm.mCommand = COMMOVER_CMD;
		bResult = TRUE;
	}
	if(bResult)
		theApp.mComm.createtimeoutThread();

	return bResult;
}

