#include "stdafx.h"
#include "CommThread.h"
#include ".\commthread.h"

#define PACKET_LOG

//--- Ŭ���� ������
CCommThread::CCommThread()
{

	//--> �ʱ�� �翬��..��Ʈ�� ������ ���� ���¿��߰���?
	m_bConnected = FALSE;
	bGeneralDataFlag = FALSE;
	m_hThreadRcvTimeout = NULL;
	m_bCmdMode = FALSE;
	//memset(&TxPktBuff,0x00,sizeof(TXPKTBUFF)*100);
//	sflag		 = FALSE;
}

CCommThread::~CCommThread()
{

}

BOOL CCommThread::SetreturnHandle(HWND hwnd){
	hStartWnd = hwnd;

	if(hStartWnd == (HANDLE) -1)
		return FALSE;

	return TRUE;
}
// ��Ʈ sPortName�� dwBaud �ӵ��� ����.
// ThreadWatchComm �Լ����� ��Ʈ�� ���� ������ �� MainWnd�� �˸���
// ���� WM_COMM_READ�޽����� ������ ���� ���� wPortID���� ���� �޴´�.
BOOL CCommThread::OpenPort(CString strPortName, 
					   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity )
{



	// Local ����.
	COMMTIMEOUTS	timeouts;
	DCB				dcb;
	DWORD			dwThreadID;

	// overlapped structure ���� �ʱ�ȭ.
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	//--> Read �̺�Ʈ ������ ����..
	if ( !(m_osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) ) 	
	{
		return FALSE;
	}


	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	//--> Write �̺�Ʈ ������ ����..
	if (! (m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
	{
		return FALSE;
	}

	//--> ��Ʈ�� ����..
	m_sPortName = strPortName;

	//--> ��������...RS 232 ��Ʈ ����..
	m_hComm = CreateFile( m_sPortName, 
						  GENERIC_READ | GENERIC_WRITE, 0, 0,
						  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
						  NULL);


	//--> ��Ʈ ���⿡ �����ϸ�..
	if (m_hComm == (HANDLE) -1)
	{
		//AfxMessageBox("fail Port ofen");
		return FALSE;
	}
	

	//===== ��Ʈ ���� ����. =====

	// EV_RXCHAR event ����...�����Ͱ� ������.. ���� �̺�Ʈ�� �߻��ϰԲ�..
	SetCommMask( m_hComm, EV_RXCHAR);	

	// InQueue, OutQueue ũ�� ����.
	SetupComm( m_hComm, BUFF_SIZE, BUFF_SIZE);	

	// ��Ʈ ����.
	PurgeComm( m_hComm,					
			   PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// timeout ����.
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600 / dwBaud;;
	timeouts.WriteTotalTimeoutConstant = 0;

	SetCommTimeouts( m_hComm, &timeouts);

	
	// dcb ����.... ��Ʈ�� ��������..��� ����ϴ� DCB ����ü�� ����..
	dcb.DCBlength = sizeof(DCB);

	//--> ���� ������ �� �߿���..
	GetCommState( m_hComm, &dcb);	
	
	//--> ���巹��Ʈ�� �ٲٰ�..
	dcb.BaudRate = dwBaud;
	
	//--> Data 8 Bit
	dcb.ByteSize = byData;

	//--> Noparity
	dcb.Parity = byParity;

	//--> 1 Stop Bit
	dcb.StopBits = byStop;




	//--> ��Ʈ�� ��..����������.. �����غ���..
	if( !SetCommState( m_hComm, &dcb) )	
	{
		return FALSE;
	}
	
	//Sleep(60);

	// ��Ʈ ���� ������ ����.
	m_bConnected = TRUE;

	::SendMessage(hStartWnd, WM_COMM_OPEN, 1, 0);
	//--> ��Ʈ ���� ������ ����.
	m_hThreadWatchComm = CreateThread( NULL, 0, 
									   (LPTHREAD_START_ROUTINE)ThreadWatchComm, 
									   this, 0, &dwThreadID);

	//--> ������ ������ �����ϸ�..
	if (! m_hThreadWatchComm)
	{
		//--> ���� ��Ʈ�� �ݰ�..
		ClosePort();
		return FALSE;
	}
	check = FALSE;
	

	return TRUE;
}
	
// ��Ʈ�� �ݴ´�.
void CCommThread::ClosePort()
{
	//--> ������� �ʾ���.
//	sflag = FALSE;
	m_bConnected = FALSE;

	::SendMessage(hStartWnd, WM_COMM_OPEN, 0, 0);
	//DCB dcb;
	//SetCommState( m_hComm, &dcb);

	//--> ����ũ ����..
	SetCommMask( m_hComm, 0);
	
	Sleep(60);
	//--> ��Ʈ ����.
	PurgeComm( m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	
	//Sleep(60);
	
	CloseHandle(m_hComm);
	//--> �ڵ� �ݱ�
	
}

// ��Ʈ�� pBuff�� ������ nToWrite��ŭ ����.
// ������ ������ Byte���� �����Ѵ�.
DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	//--> ��Ʈ�� ������� ���� �����̸�..
	if( !m_bConnected )		
	{
		return 0;
	}


	//--> ���ڷ� ���� ������ ������ nToWrite ��ŭ ����.. �� ������.,dwWrite �� �ѱ�.

	if( !WriteFile( m_hComm, pBuff, nToWrite, &dwWritten, &m_osWrite))
	{
		//--> ���� ������ ���ڰ� ������ ���..
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// ���� ���ڰ� ���� �ְų� ������ ���ڰ� ���� ���� ��� Overapped IO��
			// Ư���� ���� ERROR_IO_PENDING ���� �޽����� ���޵ȴ�.
			//timeouts�� ������ �ð���ŭ ��ٷ��ش�.
			while (! GetOverlappedResult( m_hComm, &m_osWrite, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError( m_hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError( m_hComm, &dwErrorFlags, &comstat);
		}
	}



	//--> ���� ��Ʈ�� ������ ������ ����..
	return dwWritten;
}


// ��Ʈ�κ��� pBuff�� nToWrite��ŭ �д´�.
// ������ ������ Byte���� �����Ѵ�.
DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead,dwError, dwErrorFlags;
	COMSTAT comstat;

	//--- system queue�� ������ byte���� �̸� �д´�.
	ClearCommError( m_hComm, &dwErrorFlags, &comstat);

	//--> �ý��� ť���� ���� �Ÿ��� ������..
	dwRead = comstat.cbInQue;
	if(dwRead > 0)
	{
		//--> ���ۿ� �ϴ� �о���̴µ�.. ����..�о���ΰ��� ���ٸ�..

		if( !ReadFile( m_hComm, pBuff, nToRead, &dwRead, &m_osRead) )
		{
			//--> ���� �Ÿ��� ��������..
			if (GetLastError() == ERROR_IO_PENDING)
			{
				//--------- timeouts�� ������ �ð���ŭ ��ٷ��ش�.
				while (! GetOverlappedResult( m_hComm, &m_osRead, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError( m_hComm, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError( m_hComm, &dwErrorFlags, &comstat);
			}
		}
	}


	//--> ���� �о���� ������ ����.
	return dwRead;

}


// ���� ������ 10�� ��� ������ �����Լ�

BOOL CCommThread::createtimeoutThread(){
	//--> ��Ʈ ���� ������ ����.
	DWORD dwThreadID;
	BOOL bResult = FALSE;
	m_hThreadRcvTimeout = CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadRcvTimeout, 
		this, 0, &dwThreadID);

	if(m_hThreadRcvTimeout)
		bResult = TRUE;

	return bResult;

}

// ���� ������ 10�� ��� ������
DWORD ThreadRcvTimeout(CCommThread* pComm){
	int cnt = 0;

	if(pComm->mCommand == WRITEDATA_CMD || pComm->mCommand == READDATA_CMD)
	{
		while(cnt < 10){
			Sleep(1000);
			cnt++;
		}

		//10�ʰ� ������ ���ᰡ �Ǹ� fail		
		::SendMessage(pComm->getHandle(), WM_COMM_READ, pComm->mCommand, FALSE);
		pComm->m_hThreadRcvTimeout = NULL;
	}

	else
	{
		while(cnt < 2){
			Sleep(1000);
			cnt++;
		}

		//2�ʰ� ������ ���ᰡ �Ǹ� fail
		::SendMessage(pComm->getHandle(), WM_COMM_READ, pComm->mCommand, FALSE);
		pComm->m_hThreadRcvTimeout = NULL;
	}

	return TRUE;
}


// ��Ʈ�� �����ϰ�, ���� ������ ������
// m_ReadData�� ������ �ڿ� MainWnd�� �޽����� ������ Buffer�� ������
// �о��� �Ű��Ѵ�.

DWORD	ThreadWatchComm(CCommThread* pComm)
{
   DWORD           dwEvent;
   OVERLAPPED      os;
   BOOL            bOk = TRUE;
   BYTE            buff[2048];      // �б� ����
   DWORD           dwRead;  // ���� ����Ʈ��.
 

   // Event, OS ����.
   memset( &os, 0, sizeof(OVERLAPPED));
   
   //--> �̺�Ʈ ����..
   if( !(os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL)) )
   {
		bOk = FALSE;
		
   }

   //--> �̺�Ʈ ����ũ..
   if( !SetCommMask( pComm->m_hComm, EV_RXCHAR) )
   {
	   bOk = FALSE;
	   	
   }

   //--> �̺�Ʈ��..����ũ ������ ������..
   if( !bOk )
   {
		//AfxMessageBox("Error while creating ThreadWatchComm, " + pComm->m_sPortName);
		return FALSE;
   }
  
   while (pComm ->m_bConnected)
   {
 		dwEvent = 0;
	
        // ��Ʈ�� ���� �Ÿ��� �ö����� ��ٸ���.
        WaitCommEvent( pComm->m_hComm, &dwEvent, NULL);
	
	
		//--> �����Ͱ� ���ŵǾ��ٴ� �޼����� �߻��ϸ�..
        if( (dwEvent & EV_RXFLAG || dwEvent & EV_RXCHAR) == (EV_RXFLAG || EV_RXCHAR) )
		//if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
        {
            // ��Ʈ���� ���� �� �ִ� ��ŭ �д´�.
				//--> buff �� �޾Ƴ���..
			memset(buff, 0, sizeof(BYTE)*2048);			
			dwRead = pComm->ReadComm( buff, 512);
			//if(dwRead == 66)
			{
				pComm->SetReadData(buff, dwRead);				
			}
		
		}	
   }
   
  CloseHandle( os.hEvent);

   //--> ������ ���ᰡ �ǰ���?
   pComm->m_hThreadWatchComm = NULL;

   return TRUE;

}

void CCommThread::SetReadData(BYTE *pBuff, DWORD nData)
{
	int length;
	int packetType;

	int i, j;	
	memset(mTempData,0,sizeof(BYTE)*BUFF_SIZE);
	memcpy(mTempData,pBuff,nData);
	for(i=0; i<nData; i++)
	{
		if(CmdParser(&RxPacket, pBuff[i]))
		{
			length = RxPacket.LEN;
			
			//broadcast data
			if(RxPacket.BUF[0] == 0x30 && RxPacket.BUF[1] == 0x0C && RxPacket.BUF[2] == 0x12)
			{
				packetType = 1;
			}

			//receive data
			else if(RxPacket.BUF[0] == 0x80 && RxPacket.BUF[1] == 0x80 && RxPacket.BUF[2] == 0x11)
			{
				packetType = 2;
			}
			else 
			{
				packetType = 256;
			}
			for(j=0; j< length; j++)
			{
				m_usData[j] = RxPacket.BUF[j];
			}

			makeRcvData(m_usData[30]);						

		}
	}	
}

void CCommThread::makeRcvData(BYTE command){

	switch(command){

		//  [10/11/2011 sbhwang] /////////////////////////////////////////////////
	case 0xff:
		memset(&mRcvData,0x00,sizeof(RECEIVEDATA));
		bGeneralDataFlag = TRUE;
		mRcvData.ID[0] = m_usData[8];
		mRcvData.ID[1] = m_usData[7];		
		for(int i=0; i<30; i++){
			mRcvData.SECTION[i] = m_usData[31+i];
		}
		break;
		//////////////////////////////////////////////////////////////////////////
	case 0x10:
		memset(&mRcvData,0x00,sizeof(RECEIVEDATA));
		bGeneralDataFlag = TRUE;
		mRcvData.ID[0] = m_usData[8];
		mRcvData.ID[1] = m_usData[7];		
		for(int i=0; i<30; i++){
			mRcvData.SECTION[i] = m_usData[31+i];
		}
		break;
	case 0x20:
		if(!bGeneralDataFlag)
		{			
			break;
		}
		for(int i=0; i<30; i++){
			mRcvData.FACILITY[i] = m_usData[31+i];
		}
		break;
	case 0x30:
		if(!bGeneralDataFlag)
		{			
			break;
		}
		for(int i=0; i<30; i++){
			mRcvData.PROVIDER[i] = m_usData[31+i];
		}
		break;
	case 0x40:
		if(!bGeneralDataFlag)
		{
			break;
		}
		for(int i=0; i<30; i++){
			mRcvData.CUSTOMER[i] = m_usData[31+i];
		}
		break;
	case 0x50:
		if(!bGeneralDataFlag)
		{
			break;
		}
		mRcvData.TEMPERATURE = (float)m_usData[14+31]+(float)m_usData[15+31]/10;
		mRcvData.HUMIDITY = (float)m_usData[16+31]+(float)m_usData[17+31]/10;
		mRcvData.MOISTURE = m_usData[18+31];
		::SendMessage(hStartWnd, WM_COMM_READ, GENERAL_DATA, TRUE);

		if(TxPktBuff.LENGTH != 0)
			SendTxBuff();

		bGeneralDataFlag = FALSE;
		break;
	
	default:
// 	case 0x05:	//return command
// 	case 0x88:	//return command
		if(m_usData[0] == 0x80 && m_usData[1] == 0x80 && m_usData[2] == 0x11);
		else if(m_usData[0] == 0x30 && m_usData[1] == 0x0c && m_usData[2] == 0x12)
		{
			mRcvData.ID[0] = m_usData[8];
			mRcvData.ID[1] = m_usData[7];

			for(int i=0; i<30; i++){
				if(command == 0x05)
					mRcvData.READINGDATA[i] = m_usData[33+i];
				else
					mRcvData.READINGDATA[i] = m_usData[34+i];
			}

			if(m_hThreadRcvTimeout){
				TerminateThread(m_hThreadRcvTimeout,1L);
				m_hThreadRcvTimeout = NULL;

				if(mCommand == WRITEDATA_CMD){
					memset(&TxPktBuff,0x00,sizeof(TXPKTBUFF));
					m_bCmdMode = TRUE;
				}
				else if(mCommand == READDATA_CMD)
				{
					memset(&TxPktBuff,0x00,sizeof(TXPKTBUFF));
					m_bCmdMode = TRUE;
				}
				else
					m_bCmdMode = FALSE;
				
			}
			::SendMessage(hStartWnd, WM_COMM_READ, mCommand, TRUE);
		}
		break;
	}
}

void CCommThread::SendTxBuff(){

	BOOL bResult = FALSE;
	
	if(TxPktBuff.LENGTH != 0)
	{
		if(WriteComm(TxPktBuff.PKT, TxPktBuff.LENGTH)){
			if(TxPktBuff.PKT[9] == 0x88)
				mCommand = READDATA_CMD;
			else if(TxPktBuff.PKT[9] == 0xAA)
				mCommand = COMMOVER_CMD;
			else
				mCommand = WRITEDATA_CMD;
			bResult = TRUE;			
		}
		if(bResult)
			createtimeoutThread();
	}
	
}

BYTE* CCommThread::GetRealDataArray( )
{
	return m_usData;	
	//return mTempData;	
}

BYTE* CCommThread::GetReadData( )
{
	memset(m_rcvData,NULL,33);

	m_rcvData[0] = m_usData[7];
	m_rcvData[1] = m_usData[8];	
	m_rcvData[2] = m_usData[30];
	//data
	for(int i=0; i<30; i++){
		m_rcvData[3+i] = m_usData[31+i];
	}

	return m_rcvData;	
}

RECEIVEDATA CCommThread::GetRCVdata(){

	return mRcvData;
}


bool CCommThread::CmdParser(SERIAL_PKT* pSerial, BYTE data)
{
	static BYTE State = 0;
	static BYTE Pos;
	static BYTE * Ptr;
	static BYTE Chk;
	
	switch(State)
	{
	case 0:
		if(data == 0xAA) State = 1;
		else if(data == 0x48) 
		{
			Ptr = (BYTE*)(pSerial->BUF);
			State = 5;
			Pos = 0;
			Ptr[Pos] = data;
		}
		break;
	case 1:
		Ptr = &(pSerial->LEN);
		*Ptr = data;
		State = 2;

		Ptr = (BYTE*)(pSerial->BUF);
		Pos = 0;
		Chk = 0;

		if(pSerial->LEN > (sizeof(pSerial->BUF)))
		{
		State = 0;
		}
		break;
	case 2:
		Ptr[Pos++] = data;
		Chk += data;
		if(Pos == pSerial->LEN)  State = 3;
		break;
	case 3:
		Ptr = &(pSerial->SUM);
		* Ptr = data;

		if((pSerial->SUM == Chk) || (pSerial->SUM == 0))
		{
		State = 4;
		}
		else
		{
		State = 0;
		}
		break;
	case 4:
		if(data == 0x55)
		{
		State=0;
		return true;
		}

	case 5:
		Ptr[++Pos] = data;
		if(Pos == 48)	{State = 6;}
		break;
	case 6:
		if(data == 0x30)
		{
			Ptr[++Pos] = data;
			State = 0;
			pSerial->LEN = Pos;
			return true;
		}
		
	
	default:
		break;
	}
 
	return false;

}
