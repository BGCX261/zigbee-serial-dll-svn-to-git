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

//	��� Ŭ����	CCommThread 

// ��Ʈ���� �б� :
//   ��Ʈ�� �� �Ŀ� ��Ʈ�� �ڷᰡ �����ϸ� WM_COMM_READ �޽����� ���� 
//   �����쿡 ���޵ȴ�. ON_MESSAGE ��ũ�θ� �̿�, �Լ��� �����ϰ�
//   m_ReadData String�� ����� �����͸� �̿� �б�
// ��Ʈ�� ���� : WriteComm(buff, 30)�� ���� ���ۿ� �� ũ�⸦ �ǳ׸� �ȴ�.

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
	//--------- ȯ�� ���� -----------------------------------------//
	BOOL        check;
	HANDLE		m_hComm;				// ��� ��Ʈ ���� �ڵ�
	CString		m_sPortName;			// ��Ʈ �̸� (COM1 ..)
	BOOL		m_bConnected;			// ��Ʈ�� ���ȴ��� ������ ��Ÿ��.
	OVERLAPPED	m_osRead, m_osWrite;	// ��Ʈ ���� Overlapped structure
	HANDLE		m_hThreadWatchComm;		// Watch�Լ� Thread �ڵ�.
	HANDLE		m_hThreadRcvTimeout;
	WORD		m_wPortID;				// WM_COMM_READ�� �Բ� ������ �μ�.
	
	//--------- �ܺ� ��� �Լ� ------------------------------------//
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
	//--------- ���� ��� �Լ� ------------------------------------//
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

// Thread�� ����� �Լ� 
DWORD	ThreadWatchComm(CCommThread* pComm);
DWORD	ThreadRcvTimeout(CCommThread* pComm);

