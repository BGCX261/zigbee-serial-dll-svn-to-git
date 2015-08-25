//#include "Log.h"
//#include "resource.h"
#pragma once
#include <afxtempl.h>

#define WM_COMM_READ		(WM_USER +1)
#define WM_COMM_OPEN		(WM_USER +2)

#define GENERAL_DATA			100	
#define READDATA_CMD			101
#define WRITEDATA_CMD			102
#define COMMOVER_CMD			103

#define BUFF_SIZE			4192

//	통신 클래스	CCommThread 

// 포트에서 읽기 :
//   포트를 연 후에 포트에 자료가 도착하면 WM_COMM_READ 메시지가 메인 
//   윈도우에 전달된다. ON_MESSAGE 매크로를 이용, 함수를 연결하고
//   m_ReadData String에 저장된 데이터를 이용 읽기
// 포트에 쓰기 : WriteComm(buff, 30)과 같이 버퍼와 그 크기를 건네면 된다.

typedef struct _SERIAL_PKT
{
	BYTE	STX;
	BYTE	LEN;
	BYTE	BUF[128];
	BYTE	SUM;
	BYTE	ETX;
} SERIAL_PKT;

typedef struct _RECEIVEDATA
{
	BYTE	ID[2];
	BYTE	SECTION[30];
	BYTE	FACILITY[30];
	BYTE	PROVIDER[30];
	BYTE	CUSTOMER[30];
	BYTE	READINGDATA[30];
	float	TEMPERATURE;
	float	HUMIDITY;
	BOOL	MOISTURE;

} RECEIVEDATA;

typedef struct _TXPKTBUFF{
	BYTE	PKT[128];
	DWORD	LENGTH;
} TXPKTBUFF;


class	CCommThread
{
public:
	

	RECEIVEDATA GetRCVdata();
	BYTE* GetRealDataArray();
	BYTE* GetReadData();
	void SetReadData (BYTE *pBuff, DWORD nData);
	
	CCommThread();
	~CCommThread();
	//--------- 환경 변수 -----------------------------------------//
	BOOL        check;
	HANDLE		m_hComm;				// 통신 포트 파일 핸들
	CString		m_sPortName;			// 포트 이름 (COM1 ..)
	BOOL		m_bConnected;			// 포트가 열렸는지 유무를 나타냄.
	OVERLAPPED	m_osRead, m_osWrite;	// 포트 파일 Overlapped structure
	HANDLE		m_hThreadWatchComm;		// Watch함수 Thread 핸들.
	HANDLE		m_hThreadRcvTimeout;
	WORD		m_wPortID;				// WM_COMM_READ와 함께 보내는 인수.
	
	//--------- 외부 사용 함수 ------------------------------------//
	BOOL	OpenPort(CString strPortName, 
					   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity );
	void	ClosePort();
	DWORD	WriteComm(BYTE *pBuff, DWORD nToWrite);
	BOOL	SetreturnHandle(HWND hwnd);
	TXPKTBUFF TxPktBuff[100];	
	int mIndex;
	int mTotalcnt;
	int mReadcnt;
	int	mCommand;
	BOOL	createtimeoutThread();
	//--------- 내부 사용 함수 ------------------------------------//
	DWORD	ReadComm(BYTE *pBuff, DWORD nToRead);
	void	makeRcvData(BYTE command);
	HWND	getHandle(){ return hStartWnd;};
	void	SendTxBuff();

	
private:
	SERIAL_PKT	RxPacket;
	RECEIVEDATA mRcvData;	
	BYTE	m_usData[128];
	BYTE	m_rcvData[33];
	BYTE	mTempData[BUFF_SIZE];
	HWND	hStartWnd;
	BOOL	bGeneralDataFlag;
	

public:
	bool CmdParser(SERIAL_PKT* pSerial, BYTE data);
};

// Thread로 사용할 함수 
DWORD	ThreadWatchComm(CCommThread* pComm);
DWORD	ThreadRcvTimeout(CCommThread* pComm);

