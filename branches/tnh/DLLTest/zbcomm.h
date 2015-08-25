#define WM_COMM_READ (WM_USER +1)		// 패킷 수신시 메시지

#define GENERAL_DATA			100	
#define READDATA_CMD			101
#define WRITEDATA_CMD			102
#define COMMOVER_CMD			103

/* 수신데이터 패킷 구조 */
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

/* 통신 포트 연결
 통신 포트의 설정을 입력하여 통신 포트를 연다.
*/
__declspec(dllimport) BOOL	zbOpenPort(char* strPortName, DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity);

/* 시리얼포트 연결 종료 
 연결중인 통신을 종료한다.
*/
__declspec(dllimport) void	zbClosePort();

/* 수신된 패킷 반환 함수 
 수신된 패킷 데이터를 반환 해준다.
*/
__declspec(dllimport) RECEIVEDATA zvGetRcvData(void);

__declspec(dllimport) BYTE* zbGetReadData(); //Byte 배열의 형태로 데이터를 반환해준다.

/* 핸들러 매칭 함수
 사용하는 윈도우의 핸들러와 통신 모듈의 핸들러를 매칭힌다.
 통신 모듈을 사용하기전 가장먼저 실행되어야 한다.
*/
__declspec(dllimport) BOOL zbcreateComms(HWND hwnd);

/* 데이터 쓰기 커맨드 함수 
 장치 ID 2 byte, 선번 번호 2 byte, 데이터 30 byte를 입력받아 데이터를 송신한다.
 데이터 쓰기 성공시 TRUE return
*/
__declspec(dllimport) BOOL zbDataWrite(BYTE devid[2], BYTE lineid[2], BYTE PKTdata[30]);
__declspec(dllimport) BOOL zbDataWriteForCSharp(BYTE *devid, BYTE *lineid, BYTE *PKTdata);

/* 데이터 읽어오기 커맨드 함수 
 장치 ID 2 byte, 선번 번호 2 byte를 입력받아 해당되는 장치의 데이터를 읽어오는 커맨드를 송신한다.
 데이터 읽기 성공시 TRUE return
*/
__declspec(dllimport) BOOL zbDataRead(BYTE devid[2],BYTE lineid[2]);
__declspec(dllimport) BOOL zbDataReadForCSharp(BYTE *devid, BYTE *lineid);

/* 통신 종료 커맨드 함수 
 장치 ID 2 byte, 선번 번호 2 byte를 입력받아 해당되는 장치의 통신을 종료하는 커맨드를 송신한다.
*/
__declspec(dllimport) BOOL zbCommOver(BYTE devid[2],BYTE lineid[2]);
__declspec(dllimport) BOOL zbCommOverForCSharp(BYTE *devid, BYTE *lineid);

/* 시리얼포트 검색 함수
 PC와 연결된 수신기의 포트 번호를 검색하여 반환 한다.
*/
__declspec(dllimport) int ComPortScan(void);





/* 통신모듈 라이브러리 연결 */
#pragma comment(lib,"Zigbeecomms")

/* 수신데이터 처리방법 */
/* 지그비동글에 수신된 데이터가 있을경우 WM_COMM_READ 윈도우 메시지를 보내게 됩니다.*/
/* 수신프로그램에서는 WM_COMM_READ 메시지를 수신하여 처리해주면 됩니다.*/
/* 메시지 처리 예시*/
/*	

	//메시지 맵에 윈도우 메시지와 메시지 처리함수를 연결합니다.
	BEGIN_MESSAGE_MAP(CDLLTestDlg, CDialogEx)
	...
	ON_MESSAGE(WM_COMM_READ,receivePacket)
	END_MESSAGE_MAP()

	//윈도우 메시지 처리함수 입니다.
	LRESULT CDLLTestDlg::receivePacket(WPARAM wParam, LPARAM lParam)
	{
		//필요한 동작을 수행합니다.
		BYTE* inData;
		int nType = wParam;
		int nLength = lParam;
		inData = zbGetReadData();   --> 수신된 메시지를 읽어 옵니다.
		...

	}
*/
