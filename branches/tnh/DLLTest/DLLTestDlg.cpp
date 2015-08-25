
// DLLTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DLLTest.h"
#include "DLLTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDLLTestDlg dialog

CDLLTestDlg::CDLLTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDLLTestDlg::IDD, pParent)
	, m_command_1(_T(""))
	, m_command_2(_T(""))
	, m_lineid1(_T(""))
	, m_lineid2(_T(""))
	, m_strCommconnect(_T(""))
	, m_strColumnbar(_T(""))
	, m_strData(_T(""))
	, m_strDevid1(_T(""))
	, m_strDevid2(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDLLTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_cLogbox);
	DDX_Text(pDX, IDC_EDIT2, m_command_1);
	DDX_Text(pDX, IDC_EDIT4, m_command_2);
	DDX_Text(pDX, IDC_EDIT3, m_lineid1);
	DDX_Text(pDX, IDC_EDIT5, m_lineid2);
	DDX_Text(pDX, IDC_STATIC_CONNECT, m_strCommconnect);
	DDX_Text(pDX, IDC_EDIT6, m_strColumnbar);
	DDX_Text(pDX, IDC_EDIT1, m_strData);
	DDX_Control(pDX, IDC_COMBO_PORTNAME, m_cComboPortname);
	DDX_Text(pDX, IDC_EDIT_DEVID_1, m_strDevid1);
	DDX_Text(pDX, IDC_EDIT_DEVID_2, m_strDevid2);
}

BEGIN_MESSAGE_MAP(CDLLTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CDLLTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDLLTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDLLTestDlg::OnBnClickedButton3)
	ON_MESSAGE(WM_COMM_READ,receivePacket)
	ON_BN_CLICKED(IDC_BUTTON4, &CDLLTestDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDLLTestDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CDLLTestDlg message handlers

BOOL CDLLTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here	
	getUsefulport();
	m_strColumnbar = "  시간\t\t장치ID\t \tData";
	UpdateData(false);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDLLTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDLLTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDLLTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


byte CDLLTestDlg::StringToByte(CString str){
	str.MakeUpper();
	if(str.GetLength()<1)
		return 0x00;
	char tmp1 = str[0];
	char tmp2 = str[1];

	switch(tmp1)
	{
	case '0':
		if(tmp2 == '0') return 0x00;
		else if(tmp2 == '1') return 0x01;
		else if(tmp2 == '2') return 0x02;
		else if(tmp2 == '3') return 0x03;
		else if(tmp2 == '4') return 0x04;
		else if(tmp2 == '5') return 0x05;
		else if(tmp2 == '6') return 0x06;
		else if(tmp2 == '7') return 0x07;
		else if(tmp2 == '8') return 0x08;
		else if(tmp2 == '9') return 0x09;
		else if(tmp2 == 'A') return 0x0A;
		else if(tmp2 == 'B') return 0x0B;
		else if(tmp2 == 'C') return 0x0C;
		else if(tmp2 == 'D') return 0x0D;
		else if(tmp2 == 'E') return 0x0E;
		else if(tmp2 == 'F') return 0x0F;
		break;
	case '1':
		if(tmp2 == '0') return 0x10;
		else if(tmp2 == '1') return 0x11;
		else if(tmp2 == '2') return 0x12;
		else if(tmp2 == '3') return 0x13;
		else if(tmp2 == '4') return 0x14;
		else if(tmp2 == '5') return 0x15;
		else if(tmp2 == '6') return 0x16;
		else if(tmp2 == '7') return 0x17;
		else if(tmp2 == '8') return 0x18;
		else if(tmp2 == '9') return 0x19;
		else if(tmp2 == 'A') return 0x1A;
		else if(tmp2 == 'B') return 0x1B;
		else if(tmp2 == 'C') return 0x1C;
		else if(tmp2 == 'D') return 0x1D;
		else if(tmp2 == 'E') return 0x1E;
		else if(tmp2 == 'F') return 0x1F;
		break;
	case '2':
		if(tmp2 == '0') return 0x20;
		else if(tmp2 == '1') return 0x21;
		else if(tmp2 == '2') return 0x22;
		else if(tmp2 == '3') return 0x23;
		else if(tmp2 == '4') return 0x24;
		else if(tmp2 == '5') return 0x25;
		else if(tmp2 == '6') return 0x26;
		else if(tmp2 == '7') return 0x27;
		else if(tmp2 == '8') return 0x28;
		else if(tmp2 == '9') return 0x29;
		else if(tmp2 == 'A') return 0x2A;
		else if(tmp2 == 'B') return 0x2B;
		else if(tmp2 == 'C') return 0x2C;
		else if(tmp2 == 'D') return 0x2D;
		else if(tmp2 == 'E') return 0x2E;
		else if(tmp2 == 'F') return 0x2F;

		break;
	case '3':
		if(tmp2 == '0') return 0x30;
		else if(tmp2 == '1') return 0x31;
		else if(tmp2 == '2') return 0x32;
		else if(tmp2 == '3') return 0x33;
		else if(tmp2 == '4') return 0x34;
		else if(tmp2 == '5') return 0x35;
		else if(tmp2 == '6') return 0x36;
		else if(tmp2 == '7') return 0x37;
		else if(tmp2 == '8') return 0x38;
		else if(tmp2 == '9') return 0x39;
		else if(tmp2 == 'A') return 0x3A;
		else if(tmp2 == 'B') return 0x3B;
		else if(tmp2 == 'C') return 0x3C;
		else if(tmp2 == 'D') return 0x3D;
		else if(tmp2 == 'E') return 0x3E;
		else if(tmp2 == 'F') return 0x3F;

		break;
	case '4':
		if(tmp2 == '0') return 0x40;
		else if(tmp2 == '1') return 0x41;
		else if(tmp2 == '2') return 0x42;
		else if(tmp2 == '3') return 0x43;
		else if(tmp2 == '4') return 0x44;
		else if(tmp2 == '5') return 0x45;
		else if(tmp2 == '6') return 0x46;
		else if(tmp2 == '7') return 0x47;
		else if(tmp2 == '8') return 0x48;
		else if(tmp2 == '9') return 0x49;
		else if(tmp2 == 'A') return 0x4A;
		else if(tmp2 == 'B') return 0x4B;
		else if(tmp2 == 'C') return 0x4C;
		else if(tmp2 == 'D') return 0x4D;
		else if(tmp2 == 'E') return 0x4E;
		else if(tmp2 == 'F') return 0x4F;

		break;
	case '5':
		if(tmp2 == '0') return 0x50;
		else if(tmp2 == '1') return 0x51;
		else if(tmp2 == '2') return 0x52;
		else if(tmp2 == '3') return 0x53;
		else if(tmp2 == '4') return 0x54;
		else if(tmp2 == '5') return 0x55;
		else if(tmp2 == '6') return 0x56;
		else if(tmp2 == '7') return 0x57;
		else if(tmp2 == '8') return 0x58;
		else if(tmp2 == '9') return 0x59;
		else if(tmp2 == 'A') return 0x5A;
		else if(tmp2 == 'B') return 0x5B;
		else if(tmp2 == 'C') return 0x5C;
		else if(tmp2 == 'D') return 0x5D;
		else if(tmp2 == 'E') return 0x5E;
		else if(tmp2 == 'F') return 0x5F;

		break;
	case '6':
		if(tmp2 == '0') return 0x60;
		else if(tmp2 == '1') return 0x61;
		else if(tmp2 == '2') return 0x62;
		else if(tmp2 == '3') return 0x63;
		else if(tmp2 == '4') return 0x64;
		else if(tmp2 == '5') return 0x65;
		else if(tmp2 == '6') return 0x66;
		else if(tmp2 == '7') return 0x67;
		else if(tmp2 == '8') return 0x68;
		else if(tmp2 == '9') return 0x69;
		else if(tmp2 == 'A') return 0x6A;
		else if(tmp2 == 'B') return 0x6B;
		else if(tmp2 == 'C') return 0x6C;
		else if(tmp2 == 'D') return 0x6D;
		else if(tmp2 == 'E') return 0x6E;
		else if(tmp2 == 'F') return 0x6F;

		break;
	case '7':
		if(tmp2 == '0') return 0x70;
		else if(tmp2 == '1') return 0x71;
		else if(tmp2 == '2') return 0x72;
		else if(tmp2 == '3') return 0x73;
		else if(tmp2 == '4') return 0x74;
		else if(tmp2 == '5') return 0x75;
		else if(tmp2 == '6') return 0x76;
		else if(tmp2 == '7') return 0x77;
		else if(tmp2 == '8') return 0x78;
		else if(tmp2 == '9') return 0x79;
		else if(tmp2 == 'A') return 0x7A;
		else if(tmp2 == 'B') return 0x7B;
		else if(tmp2 == 'C') return 0x7C;
		else if(tmp2 == 'D') return 0x7D;
		else if(tmp2 == 'E') return 0x7E;
		else if(tmp2 == 'F') return 0x7F;

		break;
	case '8':
		if(tmp2 == '0') return 0x80;
		else if(tmp2 == '1') return 0x81;
		else if(tmp2 == '2') return 0x82;
		else if(tmp2 == '3') return 0x83;
		else if(tmp2 == '4') return 0x84;
		else if(tmp2 == '5') return 0x85;
		else if(tmp2 == '6') return 0x86;
		else if(tmp2 == '7') return 0x87;
		else if(tmp2 == '8') return 0x88;
		else if(tmp2 == '9') return 0x89;
		else if(tmp2 == 'A') return 0x8A;
		else if(tmp2 == 'B') return 0x8B;
		else if(tmp2 == 'C') return 0x8C;
		else if(tmp2 == 'D') return 0x8D;
		else if(tmp2 == 'E') return 0x8E;
		else if(tmp2 == 'F') return 0x8F;

		break;
	case '9':
		if(tmp2 == '0') return 0x90;
		else if(tmp2 == '1') return 0x91;
		else if(tmp2 == '2') return 0x92;
		else if(tmp2 == '3') return 0x93;
		else if(tmp2 == '4') return 0x94;
		else if(tmp2 == '5') return 0x95;
		else if(tmp2 == '6') return 0x96;
		else if(tmp2 == '7') return 0x97;
		else if(tmp2 == '8') return 0x98;
		else if(tmp2 == '9') return 0x99;
		else if(tmp2 == 'A') return 0x9A;
		else if(tmp2 == 'B') return 0x9B;
		else if(tmp2 == 'C') return 0x9C;
		else if(tmp2 == 'D') return 0x9D;
		else if(tmp2 == 'E') return 0x9E;
		else if(tmp2 == 'F') return 0x9F;

		break;
	case 'A':
		if(tmp2 == '0') return 0xA0;
		else if(tmp2 == '1') return 0xA1;
		else if(tmp2 == '2') return 0xA2;
		else if(tmp2 == '3') return 0xA3;
		else if(tmp2 == '4') return 0xA4;
		else if(tmp2 == '5') return 0xA5;
		else if(tmp2 == '6') return 0xA6;
		else if(tmp2 == '7') return 0xA7;
		else if(tmp2 == '8') return 0xA8;
		else if(tmp2 == '9') return 0xA9;
		else if(tmp2 == 'A') return 0xAA;
		else if(tmp2 == 'B') return 0xAB;
		else if(tmp2 == 'C') return 0xAC;
		else if(tmp2 == 'D') return 0xAD;
		else if(tmp2 == 'E') return 0xAE;
		else if(tmp2 == 'F') return 0xAF;

		break;
	case 'B':
		if(tmp2 == '0') return 0xB0;
		else if(tmp2 == '1') return 0xB1;
		else if(tmp2 == '2') return 0xB2;
		else if(tmp2 == '3') return 0xB3;
		else if(tmp2 == '4') return 0xB4;
		else if(tmp2 == '5') return 0xB5;
		else if(tmp2 == '6') return 0xB6;
		else if(tmp2 == '7') return 0xB7;
		else if(tmp2 == '8') return 0xB8;
		else if(tmp2 == '9') return 0xB9;
		else if(tmp2 == 'A') return 0xBA;
		else if(tmp2 == 'B') return 0xBB;
		else if(tmp2 == 'C') return 0xBC;
		else if(tmp2 == 'D') return 0xBD;
		else if(tmp2 == 'E') return 0xBE;
		else if(tmp2 == 'F') return 0xBF;

		break;		
	case 'C':
		if(tmp2 == '0') return 0xC0;
		else if(tmp2 == '1') return 0xC1;
		else if(tmp2 == '2') return 0xC2;
		else if(tmp2 == '3') return 0xC3;
		else if(tmp2 == '4') return 0xC4;
		else if(tmp2 == '5') return 0xC5;
		else if(tmp2 == '6') return 0xC6;
		else if(tmp2 == '7') return 0xC7;
		else if(tmp2 == '8') return 0xC8;
		else if(tmp2 == '9') return 0xC9;
		else if(tmp2 == 'A') return 0xCA;
		else if(tmp2 == 'B') return 0xCB;
		else if(tmp2 == 'C') return 0xCC;
		else if(tmp2 == 'D') return 0xCD;
		else if(tmp2 == 'E') return 0xCE;
		else if(tmp2 == 'F') return 0xCF;

		break;
	case 'D':
		if(tmp2 == '0') return 0xD0;
		else if(tmp2 == '1') return 0xD1;
		else if(tmp2 == '2') return 0xD2;
		else if(tmp2 == '3') return 0xD3;
		else if(tmp2 == '4') return 0xD4;
		else if(tmp2 == '5') return 0xD5;
		else if(tmp2 == '6') return 0xD6;
		else if(tmp2 == '7') return 0xD7;
		else if(tmp2 == '8') return 0xD8;
		else if(tmp2 == '9') return 0xD9;
		else if(tmp2 == 'A') return 0xDA;
		else if(tmp2 == 'B') return 0xDB;
		else if(tmp2 == 'C') return 0xDC;
		else if(tmp2 == 'D') return 0xDD;
		else if(tmp2 == 'E') return 0xDE;
		else if(tmp2 == 'F') return 0xDF;

		break;
	case 'E':
		if(tmp2 == '0') return 0xE0;
		else if(tmp2 == '1') return 0xE1;
		else if(tmp2 == '2') return 0xE2;
		else if(tmp2 == '3') return 0xE3;
		else if(tmp2 == '4') return 0xE4;
		else if(tmp2 == '5') return 0xE5;
		else if(tmp2 == '6') return 0xE6;
		else if(tmp2 == '7') return 0xE7;
		else if(tmp2 == '8') return 0xE8;
		else if(tmp2 == '9') return 0xE9;
		else if(tmp2 == 'A') return 0xEA;
		else if(tmp2 == 'B') return 0xEB;
		else if(tmp2 == 'C') return 0xEC;
		else if(tmp2 == 'D') return 0xED;
		else if(tmp2 == 'E') return 0xEE;
		else if(tmp2 == 'F') return 0xEF;

		break;
	case 'F':
		if(tmp2 == '0') return 0xF0;
		else if(tmp2 == '1') return 0xF1;
		else if(tmp2 == '2') return 0xF2;
		else if(tmp2 == '3') return 0xF3;
		else if(tmp2 == '4') return 0xF4;
		else if(tmp2 == '5') return 0xF5;
		else if(tmp2 == '6') return 0xF6;
		else if(tmp2 == '7') return 0xF7;
		else if(tmp2 == '8') return 0xF8;
		else if(tmp2 == '9') return 0xF9;
		else if(tmp2 == 'A') return 0xFA;
		else if(tmp2 == 'B') return 0xFB;
		else if(tmp2 == 'C') return 0xFC;
		else if(tmp2 == 'D') return 0xFD;
		else if(tmp2 == 'E') return 0xFE;
		else if(tmp2 == 'F') return 0xFF;

		break;

	default:
		return 0x00;
		break;
	}

	return 0x00;
	
	
}

void CDLLTestDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	

	UpdateData(TRUE);
	CString tmpChar1;
	CString tmpChar2;

	//  [2/9/2012 sbhwang]
	BYTE devid[2] = {StringToByte(m_strDevid1),StringToByte(m_strDevid2)};
	//////////////////////////////////////////////////////////////////////////

	BYTE lineid[2] = {StringToByte(m_lineid1),StringToByte(m_lineid2)};
	BYTE data[30];
	for(int i=0; i<30; i++)
	{
		data[i] = 0x00;
		//tmpChar1.Format(" %02x",data[i]);
		//tmpChar2 += tmpChar1;
	}

	int len = m_strData.GetLength();

	//무조건2자리씩 끊기
	int cnt = 0;
	int nt = 0;
	CString tmpstrtobyte="";
	for(int i=0; i<len; i++){
		tmpstrtobyte += m_strData[i];		
		nt++;
		if(i!=0 && nt == 2 ){
			data[cnt++] = StringToByte(tmpstrtobyte);
			tmpstrtobyte = "";
			nt = 0;
		}
		if(cnt == 30)
			break;
	}

	COleDateTime ct;
	ct = COleDateTime::GetCurrentTime();
	CString strLogMsg;
	CString strTmp;

	if(zbDataWrite(devid,lineid,data)){

		//strLogMsg.Format("[SEND TIME : %02d-%02d %02d:%02d:%02d]\r\n",ct.GetMonth(),ct.GetDay(),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
		//strTmp.Format("[DEVICE ID : %02x %02x] [COMMAND : %02x]\r\n[DATA : %s]\r\nData set message send\r\n",m_devid_1,m_devid_2,m_command_2,tmpChar2);
		//strLogMsg += strTmp;
		//strLogMsg += "----------------------------------------------------------------------------------------------------------------------------------\r\n";

	}
	else
	{
		//strLogMsg.Format("[SEND TIME : %02d-%02d %02d:%02d:%02d]\r\n",ct.GetMonth(),ct.GetDay(),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
		//strTmp.Format("Send fail\r\n",m_devid_1,m_devid_2,m_command_2);
		//strLogMsg += strTmp;
		//strLogMsg += "----------------------------------------------------------------------------------------------------------------------------------\r\n";
	}

	//AddLogString(strLogMsg);
}

void CDLLTestDlg::OnBnClickedButton2()
{
	BYTE lineid[2] = {0x00,0x00};
	BYTE devid[2] = {0x00,0x01};
	
	UpdateData(TRUE);

	devid[0] = StringToByte(m_strDevid1);
	devid[1] = StringToByte(m_strDevid2);

	lineid[0] = StringToByte(m_lineid1);
	lineid[1] = StringToByte(m_lineid2);
	 
	COleDateTime ct;
	ct = COleDateTime::GetCurrentTime();
	CString strLogMsg;
	CString strTmp;
	CString tmpChar;
	
	if(zbDataRead(devid,lineid)){		
		//strLogMsg.Format("[SEND TIME : %02d-%02d %02d:%02d:%02d]\r\n",ct.GetMonth(),ct.GetDay(),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
		//strTmp.Format("[DEVICE ID : %02x %02x] [COMMAND : %02x]\r\nData read message send\r\n",m_devid_1,m_devid_2,m_command_2);
		//strLogMsg += strTmp;
		//strLogMsg += "----------------------------------------------------------------------------------------------------------------------------------\r\n";

	}
	else
	{
		//strLogMsg.Format("[SEND TIME : %02d-%02d %02d:%02d:%02d]\r\n",ct.GetMonth(),ct.GetDay(),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
		//strTmp.Format("Send fail\r\n",m_devid_1,m_devid_2,m_command_2);
		//strLogMsg += strTmp;
		//strLogMsg += "----------------------------------------------------------------------------------------------------------------------------------\r\n";
	}

	//AddLogString(strLogMsg);
	

}

void CDLLTestDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here

	BYTE lineid[2] = {0x00,0x00};
	BYTE devid[2] = {0x00,0x01};

	UpdateData(TRUE);

	devid[0] = StringToByte(m_strDevid1);
	devid[1] = StringToByte(m_strDevid2);

	lineid[0] = StringToByte(m_lineid1);
	lineid[1] = StringToByte(m_lineid2);

	COleDateTime ct;
	ct = COleDateTime::GetCurrentTime();
	CString strLogMsg;
	CString strTmp;
	CString tmpChar;

	if(zbCommOver(devid,lineid)){
		//strLogMsg.Format("[SEND TIME : %02d-%02d %02d:%02d:%02d]\r\n",ct.GetMonth(),ct.GetDay(),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
		//strTmp.Format("[DEVICE ID : %02x %02x] [COMMAND : %02x]\r\n Communication over message send\r\n",m_devid_1,m_devid_2,m_command_2);
		//strLogMsg += strTmp;
		//strLogMsg += "----------------------------------------------------------------------------------------------------------------------------------\r\n";
	}
	else
	{
		//strLogMsg.Format("[SEND TIME : %02d-%02d %02d:%02d:%02d]\r\n",ct.GetMonth(),ct.GetDay(),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
		//strTmp.Format("Send fail\r\n",m_devid_1,m_devid_2,m_command_2);
		//strLogMsg += strTmp;
		//strLogMsg += "----------------------------------------------------------------------------------------------------------------------------------\r\n";
	}

	//AddLogString(strLogMsg);

	
}


void CDLLTestDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	//char* strPort;
	CString strPort="";
	int nPort = 0;
	CString strPortMsg="";

	m_cComboPortname.GetWindowTextA(strPortMsg);

	int length = strPortMsg.GetLength();	

	nPort = atoi(strPortMsg.Mid(3, length));
	
	zbcreateComms(this->m_hWnd);
	//if(zbOpenPort(GetPortName(ComPortScan()),CBR_115200,8,2,0)){
	if(zbOpenPort(GetPortName(nPort),CBR_115200,8,2,0)){
		AfxMessageBox("포트열림");
		m_strCommconnect = "연결 됨";
	}
	else{
		AfxMessageBox("포트열기 실패");
		m_strCommconnect = "연결 안 됨";
	}
	UpdateData(FALSE);
	

}

void CDLLTestDlg::getUsefulport(){
	int nPort = 0;

	bool bRtn = false;

	CString strReg="";
	DWORD dwType = REG_SZ;
	DWORD dwSize = 128;
	HKEY hKey;
	TCHAR szBuffer[128] = {'\0' ,};
	char i_str[MAX_PATH];
	DWORD Size = MAX_PATH;

	CString strPort;
	LONG  regRead = -1;

	int i=0;
	int count = 0;
	LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE , "HARDWARE\\DEVICEMAP\\SERIALCOMM" , 0 , KEY_READ | KEY_QUERY_VALUE , &hKey);
	if(lResult == ERROR_SUCCESS)
	{

		for(int i=0; lResult == ERROR_SUCCESS; i++){
			DWORD dwType, dsSize = MAX_PATH;
			lResult = RegEnumValue(hKey, i, i_str, &Size, NULL, NULL, NULL, NULL);
			if(lResult == ERROR_SUCCESS){
				
				RegQueryValueEx(hKey,i_str,0,&dwType,(LPBYTE)szBuffer,&dwSize);
				m_cComboPortname.AddString(szBuffer);
			}
			Size = MAX_PATH;
		}
		RegCloseKey(hKey);
	}
	return ;

}


void CDLLTestDlg::OnBnClickedButton5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	zbClosePort();
	m_strCommconnect = "연결 안 됨";
	UpdateData(FALSE);
}

char* CDLLTestDlg::GetPortName(int index)
{
	char* strPort;

	switch(index)
	{
	case 0: { strPort = "NULL"; } break;
	case 1: { strPort = "COM1"; } break;
	case 2: { strPort = "COM2"; } break;
	case 3: { strPort = "COM3"; } break;
	case 4: { strPort = "COM4"; } break;
	case 5: { strPort = "\\\\.\\COM5"; } break;
	case 6: { strPort = "\\\\.\\COM6"; } break;
	case 7: { strPort = "\\\\.\\COM7"; } break;
	case 8: { strPort = "\\\\.\\COM8"; } break;
	case 9: { strPort = "\\\\.\\COM9";} break;
	case 10: { strPort = "\\\\.\\COM10"; } break;
	case 11: { strPort = "\\\\.\\COM11"; } break;
	case 12: { strPort = "\\\\.\\COM12"; } break;
	case 13: { strPort = "\\\\.\\COM13"; } break;
	case 14: { strPort = "\\\\.\\COM14"; } break;
	case 15: { strPort = "\\\\.\\COM15"; } break;
	case 16: { strPort = "\\\\.\\COM16"; } break;
	case 17: { strPort = "\\\\.\\COM17"; } break;
	case 18: { strPort = "\\\\.\\COM18"; } break;
	case 19: { strPort = "\\\\.\\COM19"; } break;
	case 20: { strPort = "\\\\.\\COM20"; } break;
	case 21: { strPort = "\\\\.\\COM21"; } break;
	case 22: { strPort = "\\\\.\\COM22"; } break;
	case 23: { strPort = "\\\\.\\COM23"; } break;
	case 24: { strPort = "\\\\.\\COM24"; } break;
	case 25: { strPort = "\\\\.\\COM25"; } break;
	case 26: { strPort = "\\\\.\\COM26"; } break;
	case 27: { strPort = "\\\\.\\COM27"; } break;
	case 28: { strPort = "\\\\.\\COM28"; } break;
	case 29: { strPort = "\\\\.\\COM29"; } break;
	case 30: { strPort = "\\\\.\\COM30"; } break;
	case 31: { strPort = "\\\\.\\COM31"; } break;
	case 32: { strPort = "\\\\.\\COM32"; } break;
	case 33: { strPort = "\\\\.\\COM33"; } break;
	case 34: { strPort = "\\\\.\\COM34"; } break;
	case 35: { strPort = "\\\\.\\COM35"; } break;
	case 36: { strPort = "\\\\.\\COM36"; } break;
	case 37: { strPort = "\\\\.\\COM37"; } break;
	case 38: { strPort = "\\\\.\\COM38"; } break;
	case 39: { strPort = "\\\\.\\COM39"; } break;
	case 40: { strPort = "\\\\.\\COM40"; } break;


	default: { strPort = "NULL"; } break;
	}
	return strPort;
}



LRESULT CDLLTestDlg::receivePacket(WPARAM wParam, LPARAM lParam)
{
	int dataType = wParam;
	BOOL bResult = lParam;
	CString strLogMsg;
	CString strTmp;
	CString tmpChar;
	COleDateTime ct;
	ct = COleDateTime::GetCurrentTime();

	RECEIVEDATA rcvData = zvGetRcvData();
	//온도 보정인자
	rcvData.TEMPERATURE -= TEMPERATURE_FACTOR;


	if(dataType == GENERAL_DATA){
		
		strLogMsg.Format("  %02d:%02d:%02d\t%02x %02x\t\tTEMPERATURE = %.1f HUMIDITY = %.1f%% MOISTURE = %d",ct.GetHour(),ct.GetMinute(),ct.GetSecond(),rcvData.ID[0],rcvData.ID[1],rcvData.TEMPERATURE,rcvData.HUMIDITY,rcvData.MOISTURE);
		
		AddLogString(strLogMsg);

		//  [10/11/2011 sbhwang] log file
		CString strDate,strTime;
		strDate.Format("%04d%02d%02d",ct.GetYear(),ct.GetMonth(),ct.GetDay());
		strTime.Format("%02d:%02d:%02d",ct.GetHour(),ct.GetMinute(),ct.GetSecond());
		CString filepath = strDate+".csv";
		CFile cfile(filepath,CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::modeNoInherit);
		cfile.SeekToEnd();	
		strLogMsg.Format("%s,%s,%02d%02d,%f,%f\n",strDate,strTime,rcvData.ID[0],rcvData.ID[1],rcvData.TEMPERATURE,rcvData.HUMIDITY);

		cfile.Write(strLogMsg,strLogMsg.GetLength());
		cfile.Close();
		//////////////////////////////////////////////////////////////////////////
		
	}

	else if(dataType == READDATA_CMD){
		if(bResult){
			strLogMsg = "";
			strLogMsg.Format("  %02d:%02d:%02d\t%02x %02x",ct.GetHour(),ct.GetMinute(),ct.GetSecond(),rcvData.ID[0],rcvData.ID[1]);
			strTmp = "";
			for(int i=0; i<sizeof(rcvData.READINGDATA);i++){
				tmpChar.Format(" %02x",rcvData.READINGDATA[i]);
				strTmp += tmpChar;
			};
			strLogMsg += "\t\t"+strTmp;
			AddLogString(strLogMsg);
			AfxMessageBox("데이터읽기 TRUE");
		}
		else
		{
			AfxMessageBox("데이터읽기 FALSE");
		}
		 
	}

 	else if(dataType == WRITEDATA_CMD){
		if(bResult){			
			AfxMessageBox("데이터쓰기 TRUE");
		}
		else
		{
			AfxMessageBox("데이터쓰기 FALSE");
		}


	}

	else if(dataType == COMMOVER_CMD){
		if(bResult)
			AfxMessageBox("통신종료 TRUE");
		else
		{
			AfxMessageBox("통신종료 FALSE");
		}


	}
	return 0;
}

/*
LRESULT CDLLTestDlg::receivePacket(WPARAM wParam, LPARAM lParam)
{
	BYTE* inData;
	int nType = wParam;
	int nLength = lParam;
	inData = zbGetReadData();
	ParseData(inData);
	
	RECEIVEDATA rcvData;
	rcvData = zvGetRcvData();
	
	COleDateTime ct;
	ct = COleDateTime::GetCurrentTime();
	CString strLogMsg;
	CString strTmp;
	CString tmpChar;
	if(nType == 2){
		strLogMsg.Format("  %02d:%02d:%02d\t%02x %02x\t     %02x",ct.GetHour(),ct.GetMinute(),ct.GetSecond(),mParsedata.DEVICE_ID[0],mParsedata.DEVICE_ID[1],mParsedata.COMMAND);
		strTmp = "";
		for(int i=0; i<sizeof(mParsedata.DATA);i++){
			tmpChar.Format(" %02x",mParsedata.DATA[i]);
			strTmp += tmpChar;
		};
		strLogMsg += "\t\t"+strTmp;
		//strLogMsg.Format("[RCV TIME : %02d-%02d %02d:%02d:%02d]\r\n",ct.GetMonth(),ct.GetDay(),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
		//if(nLength > 10)
		//	strLogMsg += "[Result : success]\r\n";
		//else
		//	strLogMsg += "[Result : fail]\r\n";
		//strLogMsg += "----------------------------------------------------------------------------------------------------------------------------------\r\n";
	}
	else
	{
		strLogMsg.Format("  %02d:%02d:%02d\t%02x %02x\t     %02x",ct.GetHour(),ct.GetMinute(),ct.GetSecond(),mParsedata.DEVICE_ID[0],mParsedata.DEVICE_ID[1],mParsedata.COMMAND);

		//strLogMsg.Format("[RCV TIME : %02d-%02d %02d:%02d:%02d]\r\n",ct.GetMonth(),ct.GetDay(),ct.GetHour(),ct.GetMinute(),ct.GetSecond());
		//strTmp.Format("[DEVICE ID : %02x %02x] [COMMAND : %02x]\r\n"
		//	,mParsedata.DEVICE_ID[0],mParsedata.DEVICE_ID[1],mParsedata.COMMAND);
		//strLogMsg += strTmp;

		strTmp = "";
		for(int i=0; i<sizeof(mParsedata.DATA);i++){
			tmpChar.Format(" %02x",mParsedata.DATA[i]);
			strTmp += tmpChar;
		};
		strLogMsg += "\t\t"+strTmp;
		//strLogMsg += "[DATA : "+strTmp+"]\r\n";
		//strLogMsg += "----------------------------------------------------------------------------------------------------------------------------------\r\n";
	}
	
	AddLogString(strLogMsg);

	return 0;
}
*/

void CDLLTestDlg::AddLogString( const TCHAR* str )
{
	m_cLogbox.SetSel(-1, -1);
	m_cLogbox.ReplaceSel(str);
	m_cLogbox.ReplaceSel(TEXT("\r"));
	m_cLogbox.LineScroll(1);
}


void CDLLTestDlg::addLog(CString szMsg, int nCondition)
{
	BOOL bAutoScroll = FALSE;
	int nMax = m_cLogbox.GetScrollLimit(SB_VERT) - 1;
	int nPos = m_cLogbox.GetScrollPos(SB_VERT);
	if (nPos == nMax) // 마지막 라인에 스크롤바가 위치해 있다면 자동으로 스크롤 처리
		bAutoScroll = TRUE;


	long nTotalTextLength = m_cLogbox.GetWindowTextLength();


	CHARRANGE cr;
	CHARFORMAT cf;  

	cr.cpMin = nTotalTextLength;
	cr.cpMax = nTotalTextLength + strlen(szMsg);    
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = (unsigned long)~CFE_AUTOCOLOR;
	if (nCondition == 1)
		cf.crTextColor = RGB(255,0,0);
	else if (nCondition == 2)
		cf.crTextColor = RGB(0,0,255);
	else if (nCondition == 3)
		cf.crTextColor = RGB(0,255,0);
	else
		cf.crTextColor = RGB(0,0,0);

	m_cLogbox.GetSel(cr);
	m_cLogbox.SetSel(-1, -1);
	m_cLogbox.SetSelectionCharFormat(cf);
	m_cLogbox.ReplaceSel(szMsg);
	m_cLogbox.SetSel(cr);
	m_cLogbox.LineScroll(1);
	
	if (bAutoScroll)
		m_cLogbox.SendMessage(WM_VSCROLL, SB_BOTTOM);
	else
		m_cLogbox.SendMessage(WM_VSCROLL, MAKELONG(SB_THUMBPOSITION,nPos));
		
}






