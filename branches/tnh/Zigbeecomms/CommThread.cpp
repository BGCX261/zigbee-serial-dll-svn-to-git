#include "stdafx.h"
#include "CommThread.h"
#include ".\commthread.h"

#define PACKET_LOG

//--- 클래스 생성자
CCommThread::CCommThread()
{

	//--> 초기는 당연히..포트가 열리지 않은 상태여야겠죠?
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
// 포트 sPortName을 dwBaud 속도로 연다.
// ThreadWatchComm 함수에서 포트에 무언가 읽혔을 때 MainWnd에 알리기
// 위해 WM_COMM_READ메시지를 보낼때 같이 보낼 wPortID값을 전달 받는다.
BOOL CCommThread::OpenPort(CString strPortName, 
					   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity )
{



	// Local 변수.
	COMMTIMEOUTS	timeouts;
	DCB				dcb;
	DWORD			dwThreadID;

	// overlapped structure 변수 초기화.
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	//--> Read 이벤트 생성에 실패..
	if ( !(m_osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) ) 	
	{
		return FALSE;
	}


	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	//--> Write 이벤트 생성에 실패..
	if (! (m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
	{
		return FALSE;
	}

	//--> 포트명 저장..
	m_sPortName = strPortName;

	//--> 실제적인...RS 232 포트 열기..
	m_hComm = CreateFile( m_sPortName, 
						  GENERIC_READ | GENERIC_WRITE, 0, 0,
						  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
						  NULL);


	//--> 포트 열기에 실해하면..
	if (m_hComm == (HANDLE) -1)
	{
		//AfxMessageBox("fail Port ofen");
		return FALSE;
	}
	

	//===== 포트 상태 설정. =====

	// EV_RXCHAR event 설정...데이터가 들어오면.. 수신 이벤트가 발생하게끔..
	SetCommMask( m_hComm, EV_RXCHAR);	

	// InQueue, OutQueue 크기 설정.
	SetupComm( m_hComm, BUFF_SIZE, BUFF_SIZE);	

	// 포트 비우기.
	PurgeComm( m_hComm,					
			   PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// timeout 설정.
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600 / dwBaud;;
	timeouts.WriteTotalTimeoutConstant = 0;

	SetCommTimeouts( m_hComm, &timeouts);

	
	// dcb 설정.... 포트의 실제적인..제어를 담당하는 DCB 구조체값 셋팅..
	dcb.DCBlength = sizeof(DCB);

	//--> 현재 설정된 값 중에서..
	GetCommState( m_hComm, &dcb);	
	
	//--> 보드레이트를 바꾸고..
	dcb.BaudRate = dwBaud;
	
	//--> Data 8 Bit
	dcb.ByteSize = byData;

	//--> Noparity
	dcb.Parity = byParity;

	//--> 1 Stop Bit
	dcb.StopBits = byStop;




	//--> 포트를 재..설정값으로.. 설정해보고..
	if( !SetCommState( m_hComm, &dcb) )	
	{
		return FALSE;
	}
	
	//Sleep(60);

	// 포트 감시 쓰레드 생성.
	m_bConnected = TRUE;

	::SendMessage(hStartWnd, WM_COMM_OPEN, 1, 0);
	//--> 포트 감시 쓰레드 생성.
	m_hThreadWatchComm = CreateThread( NULL, 0, 
									   (LPTHREAD_START_ROUTINE)ThreadWatchComm, 
									   this, 0, &dwThreadID);

	//--> 쓰레드 생성에 실패하면..
	if (! m_hThreadWatchComm)
	{
		//--> 열린 포트를 닫고..
		ClosePort();
		return FALSE;
	}
	check = FALSE;
	

	return TRUE;
}
	
// 포트를 닫는다.
void CCommThread::ClosePort()
{
	//--> 연결되지 않았음.
//	sflag = FALSE;
	m_bConnected = FALSE;

	::SendMessage(hStartWnd, WM_COMM_OPEN, 0, 0);
	//DCB dcb;
	//SetCommState( m_hComm, &dcb);

	//--> 마스크 해제..
	SetCommMask( m_hComm, 0);
	
	Sleep(60);
	//--> 포트 비우기.
	PurgeComm( m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	
	//Sleep(60);
	
	CloseHandle(m_hComm);
	//--> 핸들 닫기
	
}

// 포트에 pBuff의 내용을 nToWrite만큼 쓴다.
// 실제로 쓰여진 Byte수를 리턴한다.
DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	//--> 포트가 연결되지 않은 상태이면..
	if( !m_bConnected )		
	{
		return 0;
	}


	//--> 인자로 들어온 버퍼의 내용을 nToWrite 만큼 쓰고.. 쓴 갯수를.,dwWrite 에 넘김.

	if( !WriteFile( m_hComm, pBuff, nToWrite, &dwWritten, &m_osWrite))
	{
		//--> 아직 전송할 문자가 남았을 경우..
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// 읽을 문자가 남아 있거나 전송할 문자가 남아 있을 경우 Overapped IO의
			// 특성에 따라 ERROR_IO_PENDING 에러 메시지가 전달된다.
			//timeouts에 정해준 시간만큼 기다려준다.
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



	//--> 실제 포트로 쓰여진 갯수를 리턴..
	return dwWritten;
}


// 포트로부터 pBuff에 nToWrite만큼 읽는다.
// 실제로 읽혀진 Byte수를 리턴한다.
DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead,dwError, dwErrorFlags;
	COMSTAT comstat;

	//--- system queue에 도착한 byte수만 미리 읽는다.
	ClearCommError( m_hComm, &dwErrorFlags, &comstat);

	//--> 시스템 큐에서 읽을 거리가 있으면..
	dwRead = comstat.cbInQue;
	if(dwRead > 0)
	{
		//--> 버퍼에 일단 읽어들이는데.. 만일..읽어들인값이 없다면..

		if( !ReadFile( m_hComm, pBuff, nToRead, &dwRead, &m_osRead) )
		{
			//--> 읽을 거리가 남았으면..
			if (GetLastError() == ERROR_IO_PENDING)
			{
				//--------- timeouts에 정해준 시간만큼 기다려준다.
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


	//--> 실제 읽어들인 갯수를 리턴.
	return dwRead;

}


// 쓰기 데이터 10초 대기 쓰레드 생성함수

BOOL CCommThread::createtimeoutThread(){
	//--> 포트 감시 쓰레드 생성.
	DWORD dwThreadID;
	BOOL bResult = FALSE;
	m_hThreadRcvTimeout = CreateThread( NULL, 0, 
		(LPTHREAD_START_ROUTINE)ThreadRcvTimeout, 
		this, 0, &dwThreadID);

	if(m_hThreadRcvTimeout)
		bResult = TRUE;

	return bResult;

}

// 쓰기 데이터 10초 대기 쓰레드
DWORD ThreadRcvTimeout(CCommThread* pComm){
	int cnt = 0;

	if(pComm->mCommand == WRITEDATA_CMD || pComm->mCommand == READDATA_CMD)
	{
		while(cnt < 10){
			Sleep(1000);
			cnt++;
		}

		//10초가 지나서 종료가 되면 fail		
		::SendMessage(pComm->getHandle(), WM_COMM_READ, pComm->mCommand, FALSE);
		pComm->m_hThreadRcvTimeout = NULL;
	}

	else
	{
		while(cnt < 2){
			Sleep(1000);
			cnt++;
		}

		//2초가 지나서 종료가 되면 fail
		::SendMessage(pComm->getHandle(), WM_COMM_READ, pComm->mCommand, FALSE);
		pComm->m_hThreadRcvTimeout = NULL;
	}

	return TRUE;
}


// 포트를 감시하고, 읽힌 내용이 있으면
// m_ReadData에 저장한 뒤에 MainWnd에 메시지를 보내어 Buffer의 내용을
// 읽어가라고 신고한다.

DWORD	ThreadWatchComm(CCommThread* pComm)
{
   DWORD           dwEvent;
   OVERLAPPED      os;
   BOOL            bOk = TRUE;
   BYTE            buff[2048];      // 읽기 버퍼
   DWORD           dwRead;  // 읽은 바이트수.
 

   // Event, OS 설정.
   memset( &os, 0, sizeof(OVERLAPPED));
   
   //--> 이벤트 설정..
   if( !(os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL)) )
   {
		bOk = FALSE;
		
   }

   //--> 이벤트 마스크..
   if( !SetCommMask( pComm->m_hComm, EV_RXCHAR) )
   {
	   bOk = FALSE;
	   	
   }

   //--> 이벤트나..마스크 설정에 실패함..
   if( !bOk )
   {
		//AfxMessageBox("Error while creating ThreadWatchComm, " + pComm->m_sPortName);
		return FALSE;
   }
  
   while (pComm ->m_bConnected)
   {
 		dwEvent = 0;
	
        // 포트에 읽을 거리가 올때까지 기다린다.
        WaitCommEvent( pComm->m_hComm, &dwEvent, NULL);
	
	
		//--> 데이터가 수신되었다는 메세지가 발생하면..
        if( (dwEvent & EV_RXFLAG || dwEvent & EV_RXCHAR) == (EV_RXFLAG || EV_RXCHAR) )
		//if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
        {
            // 포트에서 읽을 수 있는 만큼 읽는다.
				//--> buff 에 받아놓고..
			memset(buff, 0, sizeof(BYTE)*2048);			
			dwRead = pComm->ReadComm( buff, 512);
			//if(dwRead == 66)
			{
				pComm->SetReadData(buff, dwRead);				
			}
		
		}	
   }
   
  CloseHandle( os.hEvent);

   //--> 쓰레드 종료가 되겠죠?
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
