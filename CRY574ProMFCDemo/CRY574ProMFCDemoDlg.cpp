
// CRY574ProMFCDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CRY574ProMFCDemo.h"
#include "CRY574ProMFCDemoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CRY574ProAPIWrapper.h"
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
#define API_OK 0
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CCRY574ProMFCDemoDlg 对话框




CCRY574ProMFCDemoDlg::CCRY574ProMFCDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCRY574ProMFCDemoDlg::IDD, pParent)
	, m_bHex(TRUE)
	, m_strSppCmd(_T("03 00 10 00 00 00 00 00 96 06 1b ac 00 01 02 03 04 05 06 07 08 09 0a 0b"))
	, m_strATCmd(_T(""))
	, m_strMacAddr(_T("5c:c6:e9:00:49:97 "))
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
END_MESSAGE_MAP()


// CCRY574ProMFCDemoDlg 消息处理程序
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

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
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
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCRY574ProMFCDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCRY574ProMFCDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CCRY574ProMFCDemoDlg::OnBnClickedButtonTest()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	
//}



void CCRY574ProMFCDemoDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	
	// TODO: 在此处添加消息处理程序代码
}




void CCRY574ProMFCDemoDlg::OnBnClickedButtonIni()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	CRYBT_Release();
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonBreak()
{
	// TODO: 在此添加控件通知处理程序代码
	//CRYBT_SetBreak();
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonSendat()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	CRYBT_SetDefaultProfile(1);
	UpdateInfo(_T("配置SPP成功"));
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonSpp()
{
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	const char data[] = "05 5A 05 00 06 0E 00 0B 17";


	char* pcRecv = new char[4096];
	CRYBT_SPPCommand(data,pcRecv,1000, TRUE);
	CString strSPPRecv(pcRecv);
	CString strInfo;
	strInfo.Format(_T("SPP Recv:%s"),strSPPRecv);


	UpdateInfo(strInfo);
	delete pcRecv;
	pcRecv = NULL;
}


void CCRY574ProMFCDemoDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	const char near_hi_data[] = "05 5A 05 00 06 0E 00 0B 1B";
	const char near_low_data[] = "05 5A 05 00 06 0E 00 0B 1C";

	const char far_hi_data[] = "05 5A 05 00 06 0E 00 0B 1D";
	const char far_low_data[] = "05 5A 05 00 06 0E 00 0B 1E";

	char* pcRecv = new char[4096];
	CRYBT_SPPCommand(near_hi_data,pcRecv,1000, TRUE);
	CString strSPPRecv(pcRecv);
	CString strInfo;

	CRYBT_SPPCommand(near_low_data,pcRecv,1000, TRUE);
	strSPPRecv += _T("\n");
	strSPPRecv += CString(pcRecv);

	strInfo.Format(_T("SPP Recv:%s"),strSPPRecv);

	UpdateInfo(strInfo);
	delete pcRecv;
	pcRecv = NULL;


}
