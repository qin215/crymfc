
// CRY574ProMFCDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CRY574ProMFCDemo.h"
#include "CRY574ProMFCDemoDlg.h"
#include "afxdialogex.h"
#include "protocol.h"

int String2HexData(const CString &in_str, UCHAR * outBuffer);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CRY574ProAPIWrapper.h"
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CCRY574ProMFCDemoDlg �Ի���





CCRY574ProMFCDemoDlg::CCRY574ProMFCDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCRY574ProMFCDemoDlg::IDD, pParent)
	, m_bHex(TRUE)
	, m_strSppCmd(_T("03 00 10 00 00 00 00 00 96 06 1b ac 00 01 02 03 04 05 06 07 08 09 0a 0b"))
	, m_strATCmd(_T(""))
	, m_strMacAddr(_T("11:22:33:44:55:99"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCRY574ProMFCDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHOW, m_editShow);
	DDX_Text(pDX, IDC_EDIT_ATCMD, m_strATCmd);
	DDX_Check(pDX, IDC_CHECK_HEX, m_bHex);
	DDX_Text(pDX, IDC_EDIT_SPPCMD, m_strSppCmd);
	DDX_Text(pDX, IDC_EDIT_MACADDR, m_strMacAddr);

	DDX_Control(pDX, IDC_BUTTON4, m_button_start);
	DDX_Control(pDX, IDC_EDIT_STATUS, m_edit_status);

	DDX_Control(pDX, IDC_BUTTON_STOP, m_button_stop);

	DDX_Control(pDX, IDC_STATIC_CALI_STATUS, m_cali_status);
	DDX_Control(pDX, IDC_STATIC_CALI_VALUE, m_cali_value);
}

BEGIN_MESSAGE_MAP(CCRY574ProMFCDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_INI, &CCRY574ProMFCDemoDlg::OnBnClickedButtonIni)
	ON_BN_CLICKED(IDC_BUTTON_INQUIRY1, &CCRY574ProMFCDemoDlg::OnBnClickedButtonInquiry1)
	ON_BN_CLICKED(IDC_BUTTON_INQUIRY2, &CCRY574ProMFCDemoDlg::OnBnClickedButtonInquiry2)
	ON_BN_CLICKED(IDC_BUTTON_INQUIRY3, &CCRY574ProMFCDemoDlg::OnBnClickedButtonInquiry3)
	ON_BN_CLICKED(IDC_BUTTON_MACCON, &CCRY574ProMFCDemoDlg::OnBnClickedButtonMaccon)
	ON_BN_CLICKED(IDC_BUTTON_AUTOCON, &CCRY574ProMFCDemoDlg::OnBnClickedButtonAutocon)
	ON_BN_CLICKED(IDC_BUTTON_DISCON, &CCRY574ProMFCDemoDlg::OnBnClickedButtonDiscon)
	ON_BN_CLICKED(IDC_BUTTON_A2DP, &CCRY574ProMFCDemoDlg::OnBnClickedButtonA2dp)
	ON_BN_CLICKED(IDC_BUTTON_HFP, &CCRY574ProMFCDemoDlg::OnBnClickedButtonHfp)
	ON_BN_CLICKED(IDC_BUTTON_GETINFO, &CCRY574ProMFCDemoDlg::OnBnClickedButtonGetinfo)
	ON_BN_CLICKED(IDC_BUTTON_RELEASE, &CCRY574ProMFCDemoDlg::OnBnClickedButtonRelease)
	ON_BN_CLICKED(IDC_BUTTON_BREAK, &CCRY574ProMFCDemoDlg::OnBnClickedButtonBreak)
	ON_BN_CLICKED(IDC_BUTTON_SENDAT, &CCRY574ProMFCDemoDlg::OnBnClickedButtonSendat)
	ON_BN_CLICKED(IDC_BUTTON_SPP_CONFIG, &CCRY574ProMFCDemoDlg::OnBnClickedButtonSppConfig)
	ON_BN_CLICKED(IDC_BUTTON_SPP, &CCRY574ProMFCDemoDlg::OnBnClickedButtonSpp)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT_SPP, &CCRY574ProMFCDemoDlg::OnBnClickedButtonDisconnectSpp)
	ON_BN_CLICKED(IDC_BUTTON_SENDSPP, &CCRY574ProMFCDemoDlg::OnBnClickedButtonSendspp)
	ON_BN_CLICKED(IDC_BUTTON_PAIR, &CCRY574ProMFCDemoDlg::OnBnClickedButtonPair)
	ON_BN_CLICKED(IDC_BUTTON2, &CCRY574ProMFCDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CCRY574ProMFCDemoDlg::OnBnClickedButton3)
	ON_MESSAGE(WM_UPDATE_STATIC, &CCRY574ProMFCDemoDlg::OnUpdatePrompt)
	ON_BN_CLICKED(IDC_BUTTON4, &CCRY574ProMFCDemoDlg::OnBnClickedButton4)
	ON_WM_UPDATEUISTATE()
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CCRY574ProMFCDemoDlg::OnBnClickedButtonStop)
	ON_MESSAGE(WM_UPDATE_STATUS, &CCRY574ProMFCDemoDlg::OnUpdateStatus)
	
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCRY574ProMFCDemoDlg ��Ϣ�������
 void CCRY574ProMFCDemoDlg::Test(unsigned char * pInBuff,unsigned char *pOutBuff,int nLen)
{
	int j = 0;
	BOOL bLastSpace = FALSE;
	for (int i = 0;i<(nLen-1);i++)
	{
		if (pInBuff[i] == 0xC2 && pInBuff[i+1] == 0xA0)
		{
			pOutBuff[j] = 0x20;
			i++;
			bLastSpace = TRUE;
		}
		else
		{
			pOutBuff[j] = pInBuff[i];
			bLastSpace = FALSE;
		}
		j++;
	}
	if (!bLastSpace)
	{
		pOutBuff[j] = pInBuff[nLen-1];
	}
}

BOOL CCRY574ProMFCDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	unsigned char p[10] = {0};
	p[0] = 0xC1;
	p[1] = 0xA0;
	p[2] = 0xC3;
	p[3] = 0xA0;
	p[4] = 0xC4;
	p[5] = 0xA0;
	p[6] = 0xC5;
	p[7] = 0xA0;
	p[8] = 0xC2;
	p[9] = 0xAA;
	unsigned char o[10] = {0};
	
	Test(p,o,10);

	psensor_check_process();

	const UINT16 control_ids[] =
	{
		IDC_BUTTON_INQUIRY2,
		IDC_BUTTON_INQUIRY3,
		IDC_BUTTON_MACCON,
		IDC_BUTTON_AUTOCON,
		IDC_BUTTON_DISCON,
		IDC_BUTTON_A2DP,
		IDC_BUTTON_GETINFO,
		IDC_BUTTON_HFP,
		IDC_BUTTON_RELEASE,
		IDC_BUTTON7,
		IDC_BUTTON_BREAK,
		IDC_BUTTON_SENDAT,
		IDC_EDIT1,
		IDC_EDIT_ATCMD,
		IDC_BUTTON_SPP,
		IDC_BUTTON_SPP_CONFIG,
		IDC_BUTTON_DISCONNECT_SPP,
		IDC_BUTTON_SENDSPP,
		IDC_EDIT_SPPCMD,
		IDC_CHECK1,
		IDC_CHECK_HEX,
		IDC_EDIT2,
		IDC_EDIT_MACADDR,
		IDC_BUTTON1,
		IDC_BUTTON_PAIR,
		IDC_BUTTON2,
		IDC_BUTTON3,
		IDC_BUTTON_INI,
		IDC_BUTTON_INQUIRY1
	};

	for (int i = 0; i < sizeof(control_ids) / sizeof(UINT16); i++)
	{
		CWnd *pwnd = (CWnd *)GetDlgItem(control_ids[i]);
		pwnd->ShowWindow(SW_HIDE);
	}

	m_SuccessBrushBack.CreateSolidBrush(RGB(0,255,0)); 
	m_FailBrushBack.CreateSolidBrush(RGB(255,0,0));
	m_ProcessBrushBack.CreateSolidBrush(RGB(200, 200, 200));
	m_state = STATE_INIT;

	static CFont font;
	font.DeleteObject();
	font.CreatePointFont(300, _T("������"));
	m_edit_status.SetFont(&font);//��������

	m_edit_status.SetWindowTextW(_T("������...."));

	::EnableMenuItem(::GetSystemMenu(this->m_hWnd, false), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);//forbid close

	 SetTimer(AUTOTEST_TIMER_ID, 1000, 0);  //������൱���趨��timer,���Ҫͣ��timer����KillTimer(TIMERID)


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCRY574ProMFCDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCRY574ProMFCDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCRY574ProMFCDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CCRY574ProMFCDemoDlg::OnBnClickedButtonTest()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	
//}



void CCRY574ProMFCDemoDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	int count = 0;

	bStopped = TRUE;

	CRYBT_SetBreak();

	while (bRunning)
	{
		Sleep(500);
		count++;
	}
}




void CCRY574ProMFCDemoDlg::OnBnClickedButtonIni()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(CRYBT_InitializePro() == TRUE)
	{
		UpdateInfo(_T("Initialize successed"));
	}
}
void CCRY574ProMFCDemoDlg::UpdateInfo(CString strInfo)
{
	//int nLen = m_editShow.SendMessage(WM_GETTEXTLENGTH);
	m_editShow.SetSel(-1, -1); 
	strInfo += _T("\r\n");
	m_editShow.ReplaceSel(strInfo);
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonInquiry1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nInqCount = 0;
	CRYBT_InquiryAllOneTime(5,FALSE,nInqCount);
	CString strInfo;
	strInfo.Format(_T("%d BT devices has been inquiried"),nInqCount);
	UpdateInfo(strInfo);
	for (int i = 0 ;i<nInqCount;i++)
	{
		char* pcMac = new char[256];
		int nRssi = 0;
		CRYBT_GetInquiryMac(i,pcMac);
		CRYBT_GetInquiryRssi(i,nRssi);
		CString strMac(pcMac);
		CString strInfo;
		strInfo.Format(_T("MAC:%s RSSI:%d"),strMac,nRssi);
		UpdateInfo(strInfo);
		delete pcMac;
		pcMac = NULL;
	}
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonInquiry2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nInqCount = 0;
	CRYBT_InquiryAllLoop(30,5,FALSE,nInqCount);
	CString strInfo;
	strInfo.Format(_T("%d BT devices has been inquiried"),nInqCount);
	UpdateInfo(strInfo);
	for (int i = 0 ;i<nInqCount;i++)
	{
		char* pcMac = new char[256];
		int nRssi = 0;
		CRYBT_GetInquiryMac(i,pcMac);
		CRYBT_GetInquiryRssi(i,nRssi);
		CString strMac(pcMac);
		CString strInfo;
		strInfo.Format(_T("MAC:%s RSSI:%d"),strMac,nRssi);
		UpdateInfo(strInfo);
		delete pcMac;
		pcMac = NULL;
	}
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonInquiry3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nInqCount = 0;
	char* pcMac = new char[256];
	CRYBT_InquiryFirstOneTime(pcMac,5);
	CString strMac(pcMac);
	UpdateInfo(strMac);
	delete pcMac;
	pcMac = NULL;
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonMaccon()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	char* pcMac = new char[64];
	int len = m_strMacAddr.GetLength();
	for (int i = 0;i<len;i++)
	{
		pcMac[i] = m_strMacAddr[i];
	}
	pcMac[len] = '\0';

	int retCode = CRYBT_MacConnect(pcMac);
	if (retCode == API_OK)
	{
		UpdateInfo(_T("Connected"));

	} 
	else
	{
		CString strinfo;
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}
	delete [] pcMac;
	pcMac = NULL;
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonAutocon()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int retCode = CRYBT_AutoConnect(30000);
	if (retCode == API_OK)
	{
		UpdateInfo(_T("Connected"));
	} 
	else
	{
		CString strinfo;
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonDiscon()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int retCode = CRYBT_Disconnect();
	if (retCode == API_OK)
	{
		UpdateInfo(_T("Disconnected"));
	} 
	else
	{
		CString strinfo;
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonA2dp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int retCode = CRYBT_A2dpStreaming();
	if (retCode == API_OK)
	{
		UpdateInfo(_T("A2DP Mode"));
	} 
	else
	{
		CString strinfo;
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);

	}
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonHfp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int retCode = CRYBT_HfpStreaming(1);
	if (retCode == API_OK)
	{
		UpdateInfo(_T("HFP Mode"));
	} 
	else
	{
		CString strinfo;
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonGetinfo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nA2dpVol = 0;
	int retCode = CRYBT_QuiryA2dpVol(nA2dpVol);
	CString strinfo;
	if (retCode == API_OK)
	{
		strinfo.Format(_T("A2DP VOL: %d"),nA2dpVol);
		UpdateInfo(strinfo);
	} 
	else
	{
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}

	int nBattery = 0;
	retCode = CRYBT_GetBattery(nBattery);
	if (retCode == API_OK)
	{
		strinfo.Format(_T("Battery: %d"),nBattery);
		UpdateInfo(strinfo);
	} 
	else
	{
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}

	char* pcName = new char[256];
	retCode = CRYBT_QuiryName(pcName);
	if (retCode == API_OK)
	{
		CString strName(pcName);
		strinfo.Format(_T("Name: %s"),strName);
		UpdateInfo(strinfo);
	} 
	else
	{
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}
	delete pcName;
	pcName = NULL;

	int nRssi = 0;
	retCode = CRYBT_QuiryRssi(nRssi);
	if (retCode == API_OK)
	{
		strinfo.Format(_T("RSSI: %ddBm"),nRssi);
		UpdateInfo(strinfo);
	} 
	else
	{
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}

	int nMode = 0;
	retCode = CRYBT_GetCurrentMode(nMode);
	if (retCode == API_OK)
	{
		strinfo.Format(_T("MODE: %d"),nMode);//nMode 0
		UpdateInfo(strinfo);
	} 
	else
	{
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonRelease()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CRYBT_Release();
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonBreak()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CRYBT_SetBreak();
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonSendat()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	int len = m_strATCmd.GetLength();
	char sendCmd[64] = {0};
	sendCmd[0] = 0;
	for(int i = 0; i < len &&  i < 64;i++)
		sendCmd[i] = (char)m_strATCmd.GetAt(i);
		
	char* pcRecv = new char[4096];
	CRYBT_ATCommand(sendCmd,pcRecv,5000);
	CString strATRecv(pcRecv);
	CString strInfo;
	strInfo.Format(_T("ATRecv:%s"),strATRecv);
	UpdateInfo(strInfo);
	delete pcRecv;
	pcRecv = NULL;
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonSppConfig()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CRYBT_SetDefaultProfile(1);
	UpdateInfo(_T("����SPP�ɹ�"));
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonSpp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	char* pcMac = new char[64];
	int len = m_strMacAddr.GetLength();
	for (int i = 0;i<len;i++)
	{
		pcMac[i] = m_strMacAddr[i];
	}
	pcMac[len] = '\0';

	int retCode = CRYBT_ConnectSPP(pcMac);
	if (retCode == API_OK)
	{
		UpdateInfo(_T("Connected SPP"));
	} 
	else
	{
		CString strinfo;
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}

	delete []pcMac;
	pcMac = NULL;
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonDisconnectSpp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int retCode = CRYBT_DisConnectSPP();
	if (retCode == API_OK)
	{
		UpdateInfo(_T("Disconnect SPP"));
	} 
	else
	{
		CString strinfo;
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonSendspp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	int len = m_strSppCmd.GetLength();
	char sendCmd[512] = {0};
	sendCmd[0] = 0;
	for(int i = 0; i < len &&  i < 512;i++)
		sendCmd[i] = (char)m_strSppCmd.GetAt(i);

	char* pcRecv = new char[4096];
	CRYBT_SPPCommand(sendCmd,pcRecv,1000,m_bHex);
	CString strSPPRecv(pcRecv);
	CString strInfo;
	strInfo.Format(_T("SPP Recv:%s"),strSPPRecv);
	UpdateInfo(strInfo);
	delete pcRecv;
	pcRecv = NULL;
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonPair()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	char* pcMac = new char[64];
	int len = m_strMacAddr.GetLength();
	for (int i = 0;i<len;i++)
	{
		pcMac[i] = m_strMacAddr[i];
	}
	pcMac[len] = '\0';

	int retCode = CRYBT_Pair(pcMac,5000);
	if (retCode == API_OK)
	{
		UpdateInfo(_T("Paired"));
	} 
	else
	{
		CString strinfo;
		strinfo.Format(_T("error code is %d"),retCode);
		UpdateInfo(strinfo);
	}

	delete []pcMac;
	pcMac = NULL;
}

void CCRY574ProMFCDemoDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);

	const char data[] = "05 5A 05 00 06 0E 00 0B 17";


	char* pcRecv = new char[4096];
	CRYBT_SPPCommand(data, pcRecv, 1000, TRUE);
	CString strSPPRecv(pcRecv);

	BOOL ok = parse_spp_rsp_data(strSPPRecv);
	if (ok)
	{
		AfxMessageBox(_T("�����У׼"));
	}
	else
	{
		AfxMessageBox(_T("���δУ׼"));
	}

	CString strInfo;
	strInfo.Format(_T("SPP Recv:%s"),strSPPRecv);
	UpdateInfo(strInfo);

	delete pcRecv;
	pcRecv = NULL;
}



void CCRY574ProMFCDemoDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	const char near_hi_data[] = "05 5A 05 00 06 0E 00 0B 1B";
	const char near_low_data[] = "05 5A 05 00 06 0E 00 0B 1C";

	const char far_hi_data[] = "05 5A 05 00 06 0E 00 0B 1D";
	const char far_low_data[] = "05 5A 05 00 06 0E 00 0B 1E";

	char* pcRecv = new char[4096];
	CRYBT_SPPCommand(near_hi_data, pcRecv, 1000, TRUE);
	CString strSPPRecv(pcRecv);
	CString strInfo;
	strInfo.Format(_T("SPP NEAR HIGH8 Recv:%s"),strSPPRecv);
	UpdateInfo(strInfo);

	UINT32 near_data = parse_spp_rsp_data(strSPPRecv);
	

	CRYBT_SPPCommand(near_low_data, pcRecv, 1000, TRUE);
	strSPPRecv = CString(pcRecv);
	strInfo.Format(_T("SPP NEAR LOW8 Recv:%s"),strSPPRecv);
	UpdateInfo(strInfo);

	near_data <<= 8;
	near_data |= parse_spp_rsp_data(strSPPRecv);

	UINT32 far_data = CRYBT_SPPCommand(far_hi_data, pcRecv, 1000, TRUE);
	strSPPRecv = CString(pcRecv);
	strInfo.Format(_T("SPP far high8 Recv:%s"),strSPPRecv);
	UpdateInfo(strInfo);

	far_data <<= 8;

	CRYBT_SPPCommand(far_low_data, pcRecv, 1000, TRUE);
	strSPPRecv = CString(pcRecv);
	strInfo.Format(_T("SPP far low8 Recv:%s"),strSPPRecv);
	UpdateInfo(strInfo);
	far_data |= parse_spp_rsp_data(strSPPRecv);

	CString prompt;
	prompt.Format(_T("���У׼ֵΪ:0X%04X, ����У׼ֵΪ��0x%04X"), near_data, far_data);

	AfxMessageBox(prompt);

	delete pcRecv;
	pcRecv = NULL;
}


LRESULT CCRY574ProMFCDemoDlg::OnUpdatePrompt(WPARAM wParam, LPARAM lParam)
{
	CString *pText = (CString *)wParam;
	BOOL bDelete = (BOOL)lParam;

	CString infoText;
	infoText = _T("INFO: ") + CString(*pText);
	UpdateInfo(infoText);

	if (bDelete)
	{
		delete pText;
	}

	if (bRunning)
	{
		m_button_start.EnableWindow(FALSE);
		m_button_stop.EnableWindow(TRUE);
	}
	else
	{
		m_button_start.EnableWindow(TRUE);
		m_button_stop.EnableWindow(FALSE);
	}
	
	return 0;
}


void CCRY574ProMFCDemoDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (bRunning)
	{
		AfxMessageBox(_T("���ڲ��ԣ���ȴ�"));
		return;
	}

	m_edit_status.SetWindowText(_T("������..."));
	m_state = STATE_INIT;
	m_cali_status.SetWindowText(_T("����"));
	m_cali_value.SetWindowText(_T("����"));


	psensor_check_process();
}


void CCRY574ProMFCDemoDlg::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
	// �ù���Ҫ��ʹ�� Windows 2000 ����߰汾��
	// ���� _WIN32_WINNT �� WINVER ���� >= 0x0500��
	// TODO: �ڴ˴������Ϣ����������
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (!bRunning)
	{
		return;
	}

	bStopped = TRUE;

	while (bRunning)
	{
		Sleep(500);
	}
}


LRESULT CCRY574ProMFCDemoDlg::OnUpdateStatus(WPARAM wParam, LPARAM lParam)
{
	m_state = wParam;
	CString *pInfo = (CString *)lParam;

	if (m_state == STATE_SUCCESS)
	{
		m_edit_status.SetWindowText(_T("�ɹ�"));
	}
	else if (m_state == STATE_FAIL)
	{
		m_edit_status.SetWindowText(_T("ʧ��"));
	}
	else if (m_state == STATE_ERROR)
	{
		m_edit_status.SetWindowText(_T("����"));
	}
	else if (m_state == STATE_PROCESS)
	{
		m_edit_status.SetWindowText(_T("������..."));
	}
	else if (m_state == STATE_ABORT)
	{
		m_edit_status.SetWindowText(_T("��ֹ��..."));
	}
	else if (m_state == STATE_DONE)
	{
		::EnableMenuItem(::GetSystemMenu(this->m_hWnd, false), SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
	}
	else if (m_state == STATE_CALI_STATUS)
	{
		m_cali_status.SetWindowText(CString(*pInfo));
	}
	else if (m_state == STATE_CALI_VALUE)
	{
		m_cali_value.SetWindowText(CString(*pInfo));
	}

	delete pInfo;

	return 0;
}

HBRUSH CCRY574ProMFCDemoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_STATIC && m_edit_status.GetSafeHwnd() == pWnd->GetSafeHwnd()) 
	{
		if (m_state == STATE_INIT)
		{
			pDC->SetTextColor(RGB(255,255,255));
			return m_ProcessBrushBack;
		}
		else if (m_state == STATE_SUCCESS)
		{
			pDC->SetTextColor(RGB(0,0,0));
			return m_SuccessBrushBack;
		}
		else
		{
			pDC->SetTextColor(RGB(0,0,0));
			return m_FailBrushBack;
		}

	}


	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}


void CCRY574ProMFCDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnTimer(nIDEvent);

	if (nIDEvent == AUTOTEST_TIMER_ID)
	{
		if (!bRunning)
		{
			psensor_check_process();
		}
	}
}
