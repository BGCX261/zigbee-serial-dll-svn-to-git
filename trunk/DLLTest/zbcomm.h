#define WM_COMM_READ (WM_USER +1)		// ��Ŷ ���Ž� �޽���

#define GENERAL_DATA			100	
#define READDATA_CMD			101
#define WRITEDATA_CMD			102
#define COMMOVER_CMD			103

/* ���ŵ����� ��Ŷ ���� */
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

/* ��� ��Ʈ ����
 ��� ��Ʈ�� ������ �Է��Ͽ� ��� ��Ʈ�� ����.
*/
__declspec(dllimport) BOOL	zbOpenPort(char* strPortName, DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity);

/* �ø�����Ʈ ���� ���� 
 �������� ����� �����Ѵ�.
*/
__declspec(dllimport) void	zbClosePort();

/* ���ŵ� ��Ŷ ��ȯ �Լ� 
 ���ŵ� ��Ŷ �����͸� ��ȯ ���ش�.
*/
__declspec(dllimport) BYTE* zbGetReadData();

/* �ڵ鷯 ��Ī �Լ�
 ����ϴ� �������� �ڵ鷯�� ��� ����� �ڵ鷯�� ��Ī����.
 ��� ����� ����ϱ��� ������� ����Ǿ�� �Ѵ�.
*/
__declspec(dllimport) BOOL zbcreateComms(HWND hwnd);

/* ������ ���� Ŀ�ǵ� �Լ� 
 Ŀ�ǵ� 2 byte �� ������ 30 byte�� �Է¹޾� �����͸� �۽��Ѵ�.
 ������ ���� ������ TRUE return
*/
__declspec(dllimport) BOOL zbDataWrite(BYTE PKTcommand[2], BYTE PKTdata[30]);

/* ������ �о���� Ŀ�ǵ� �Լ� 
 Ŀ�ǵ� 1 byte �� ��ġID 2 byte�� �Է¹޾� �ش�Ǵ� ��ġ�� �����͸� �о���� Ŀ�ǵ带 �۽��Ѵ�.
 ������ �б� ������ TRUE return
*/
__declspec(dllimport) BOOL zbDataRead(BYTE command, BYTE devid[2]);

/* ��� ���� Ŀ�ǵ� �Լ� 
 Ŀ�ǵ� 1 byte �� ��ġID 2 byte�� �Է¹޾� �ش�Ǵ� ��ġ�� ����� �����ϴ� Ŀ�ǵ带 �۽��Ѵ�.
*/
__declspec(dllimport) BOOL zbCommOver(BYTE command, BYTE devid[2]);

/* �ø�����Ʈ �˻� �Լ�
 PC�� ����� ���ű��� ��Ʈ ��ȣ�� �˻��Ͽ� ��ȯ �Ѵ�.
*/
__declspec(dllimport) int ComPortScan(void);


__declspec(dllimport) RECEIVEDATA zvGetRcvData(void);


/* ��Ÿ�� ���̺귯�� ���� */
#pragma comment(lib,"Zigbeecomms")

/* ���ŵ����� ó����� */
/* ���׺񵿱ۿ� ���ŵ� �����Ͱ� ������� WM_COMM_READ ������ �޽����� ������ �˴ϴ�.*/
/* �������α׷������� WM_COMM_READ �޽����� �����Ͽ� ó�����ָ� �˴ϴ�.*/
/* �޽��� ó�� ����*/
/*	

	//�޽��� �ʿ� ������ �޽����� �޽��� ó���Լ��� �����մϴ�.
	BEGIN_MESSAGE_MAP(CDLLTestDlg, CDialogEx)
	...
	ON_MESSAGE(WM_COMM_READ,receivePacket)
	END_MESSAGE_MAP()

	//������ �޽��� ó���Լ� �Դϴ�.
	LRESULT CDLLTestDlg::receivePacket(WPARAM wParam, LPARAM lParam)
	{
		//�ʿ��� ������ �����մϴ�.
		BYTE* inData;
		int nType = wParam;
		int nLength = lParam;
		inData = zbGetReadData();   --> ���ŵ� �޽����� �о� �ɴϴ�.
		...

	}
*/
