
// CRY574ProMFCDemoDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "CRY574ProMFCDemo.h"
#include "CRY574ProMFCDemoDlg.h"
#include "afxdialogex.h"
#include "protocol.h"
#include "mywin.h"
#include "uart_cmd.h"

int String2HexData(const CString &in_str, UCHAR * outBuffer);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CRY574ProAPIWrapper.h"
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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
	, m_strMacAddr(_T("11:22:33:44:55:99"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_test_total = 0;
	m_test_ok_nr = 0;

	m_all_count = 0;
	m_all_ok_count = 0;

	m_bUartOpen = FALSE;
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

	DDX_Control(pDX, IDC_LEFT_CALI_STATUS, m_left_cali_status);
	DDX_Control(pDX, IDC_LEFT_CALI_VALUE, m_left_cali_value);

	DDX_Control(pDX, IDC_RIGHT_CALI_STATUS, m_right_cali_status);
	DDX_Control(pDX, IDC_RIGHT_CALI_VALUE, m_right_cali_value);

	DDX_Text(pDX, IDC_STATIC_TOTAL, m_test_total);
	DDX_Text(pDX, IDC_STATIC_OK_NUMBER, m_test_ok_nr);

	DDX_Control(pDX, IDC_COMBO_TYPE, m_combox);
	DDX_Control(pDX, IDC_STATIC_MODE, m_tws_mode);

	DDX_Control(pDX, IDC_STATIC_COLOR, m_ep_color);
	DDX_Control(pDX, IDC_STATIC_RAWDATA, m_psensor_rawdata);

	DDX_Control(pDX, IDC_STATIC_SW_VERSION, m_sw_version);


	DDX_Text(pDX, IDC_STATIC_ALL_CNT, m_all_count);
	DDX_Text(pDX, IDC_STATIC_ALL_OK_CNT, m_all_ok_count);
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
	ON_MESSAGE(WM_UART_USER_CMD, &CCRY574ProMFCDemoDlg::OnProcessUartMsg)

	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON5, &CCRY574ProMFCDemoDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_GPIB_TEST, &CCRY574ProMFCDemoDlg::OnBnClickedGpibTest)
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

	AfxInitRichEdit();

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
	font.CreatePointFont(300, _T("新宋体"));
	m_edit_status.SetFont(&font);//设置字体

	m_edit_status.SetWindowText(_T("测试中...."));

	//::EnableMenuItem(::GetSystemMenu(this->m_hWnd, false), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);//forbid close

	m_auto_test = FALSE;
	m_cycle_nr = 0;

	//CFileException e;
	CFile file;
	if (file.Open(_T("result.dat"), CFile::modeRead, NULL))
	{
		CArchive ar(&file, CArchive::load);

		try
		{
			m_result.Serialize(ar);
			m_test_ok_nr = m_result.m_ok_count;
			m_test_total = m_result.m_ok_count + m_result.m_failed_count;

			m_all_ok_count = m_result.m_total_ok_count;
			m_all_count = m_result.m_total_ok_count + m_result.m_total_failed_count;
			
		}
		catch (CMemoryException* e)
		{
			
		}
		catch (CFileException* e)
		{
			m_test_ok_nr = 0;
			m_test_total = 0;
			m_all_count = 0;
			m_all_ok_count = 0;
		}
		catch (CException* e)
		{
			m_test_ok_nr = 0;
			m_test_total = 0;
			m_all_count = 0;
			m_all_ok_count = 0;
		}

		ar.Close();
		file.Close();
	} 
	else 
	{
		m_test_total = 0;
		m_test_ok_nr = 0;

		m_all_count = 0;
		m_all_ok_count = 0;
	}

	m_combox.SetCurSel(TEST_TWS_MODE);

	int log_flag = 0;
	int console_flag = 0;
	if (!get_config_int_value(_TEXT("debug"), _TEXT("log"), &log_flag, 1))
	{
		log_flag = 0;
	}

	if (!get_config_int_value(_TEXT("debug"), _TEXT("console"), &console_flag, 0))
	{
		console_flag = 0;
	}

	if (console_flag)
	{
		enable_console_window();
	}

	enable_log_file();

	if (init_uart_buff())
	{
		m_bUartOpen = TRUE;
		register_uart_rsp_func(ua800_do_with_uart_rsp);
	}

	SetTimer(AUTOTEST_TIMER_ID, 1000, 0);

	int retcode;
	retcode = CRYBT_ResetDongle();
	Log_d(_T("CRYBT_ResetDongle retcode=%d"), retcode);


	TCHAR szVersion[128];

	// 软件版本号管理
#define DEFAULT_SW_VERSION _T("2.8.1.1")
	if (!get_config_string_value(_T("setting"), _T("version"), DEFAULT_SW_VERSION, szVersion, sizeof(szVersion) / sizeof(TCHAR)))
	{
		Log_e(_T("Get software version error, use default version(%s)"), DEFAULT_SW_VERSION);
	}
	else
	{
		Log_d(_T("Get software version ok, use version(%s)"), szVersion);
	}

	CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_CHECK_SW_VERSION);
	pWnd->SetWindowText(szVersion);

	m_default_sw_version = CString(szVersion);


	// 蓝牙名称设置
	if (!get_config_string_value(_T("setting"), _T("btname"), _T("Philips TAT5506"), szVersion, sizeof(szVersion) / sizeof(TCHAR)))
	{
		Log_e(_T("Get bt name failed, use default version(%s)"), _T("Philips TAT5506"));
	}
	else
	{
		Log_d(_T("Get bt name ok, use name(%s)"), szVersion);
	}

	current_bt_name = CString(szVersion);


	// 测试项目管理
	m_test_bitmap = get_test_item_setting_bitmap();

	memset(&m_test_array[0], -1, sizeof(m_test_array));			// -1 表示初始化, TRUE 测试结果OK， FALSE 测试结果失败

#if 0
	const CHAR msg[] = {0x53, 0x59, 0x4e, 0x54, 0x41, 0x58, 0x20, 0x45, 0x52, 0x52, 0x4f, 0x52, 0x00};
	TCHAR buff[128];

	MultiByteToWideChar(CP_ACP, 0, msg, -1, buff, sizeof(buff) / sizeof(TCHAR));
	Log_d(_T("test string='%s'"), buff);
#endif

	if (CRYBT_InitializePro() == TRUE)
	{
		dlg_update_ui(_T("初始化完成"));
	}
	else
	{
		dlg_update_ui(_T("初始化失败"));
		bRunning = FALSE;
		dlg_update_status_ui(STATE_ERROR);

		AfxMessageBox(_T("初始化失败，请检查DONGLE!"));

		//PostQuitMessage(0);			// don't quit. by qinjiangwei 2021/11/25
	}

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
	int count = 0;
	int retcode;

	bStopped = TRUE;

	CRYBT_SetBreak();

	while (bRunning)
	{
		Sleep(500);
		count++;
	}

	CRYBT_Release();

	close_logfile();
	if (m_bUartOpen)
	{
		win32_UART_Close(HX_WIN32_UART_PORT);
	}

	SaveResultToFile();
}


void CCRY574ProMFCDemoDlg::SaveResultToFile()
{
	CFile file;

	if (file.Open(_T("result.dat"), CFile::modeCreate | CFile::modeReadWrite))
	{
		CArchive ar(&file, CArchive::store);

		m_result = CTestResult(m_test_ok_nr, m_test_total - m_test_ok_nr, m_all_ok_count, m_all_count - m_all_ok_count);
		m_result.Serialize(ar);

		ar.Flush();
		ar.Close();
		file.Close();
	}
	else
	{
		Log_e(_T("store test result failed."));
	}
}



void CCRY574ProMFCDemoDlg::OnBnClickedButtonIni()
{
	// TODO: 在此添加控件通知处理程序代码
	if(CRYBT_InitializePro() == TRUE)
	{
		UpdateInfo(_T("Initialize successed"));
	}
}

#define MAX_EDIT_SHOW_CHARS		(20 * 1024)
void CCRY574ProMFCDemoDlg::UpdateInfo(CString strInfo)
{
	int nLen = m_editShow.SendMessage(WM_GETTEXTLENGTH);

	if (nLen >= MAX_EDIT_SHOW_CHARS)		//	删除一半的字符
	{
		CString fullText;
		CString rightText;
		CString rightNewLineText;

		m_editShow.GetWindowText(fullText);

		rightText = fullText.Right(nLen / 2);

		int pos = rightText.Find(_T("\r\n"));
		int rightLen = rightText.GetLength();

		rightNewLineText = rightText.Mid(pos + 2);

		m_editShow.SetSel(0, nLen);
		m_editShow.ReplaceSel(rightNewLineText);
	}

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
	UpdateData(TRUE);

	const char data[] = "05 5A 05 00 06 0E 00 0B 17";
	int side;


	char* pcRecv = new char[4096];
	CRYBT_SPPCommand(data, pcRecv, 1000, TRUE);
	CString strSPPRecv(pcRecv);

	BOOL ok = parse_spp_rsp_data(strSPPRecv, &side);
	if (ok)
	{
		AfxMessageBox(_T("光感已校准"));
	}
	else
	{
		AfxMessageBox(_T("光感未校准"));
	}

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

	int side;

	char* pcRecv = new char[4096];
	CRYBT_SPPCommand(near_hi_data, pcRecv, 1000, TRUE);
	CString strSPPRecv(pcRecv);
	CString strInfo;
	strInfo.Format(_T("SPP NEAR HIGH8 Recv:%s"),strSPPRecv);
	UpdateInfo(strInfo);

	UINT32 near_data = parse_spp_rsp_data(strSPPRecv, &side);
	

	CRYBT_SPPCommand(near_low_data, pcRecv, 1000, TRUE);
	strSPPRecv = CString(pcRecv);
	strInfo.Format(_T("SPP NEAR LOW8 Recv:%s"),strSPPRecv);
	UpdateInfo(strInfo);

	near_data <<= 8;
	near_data |= parse_spp_rsp_data(strSPPRecv, &side);

	UINT32 far_data = CRYBT_SPPCommand(far_hi_data, pcRecv, 1000, TRUE);
	strSPPRecv = CString(pcRecv);
	strInfo.Format(_T("SPP far high8 Recv:%s"),strSPPRecv);
	UpdateInfo(strInfo);

	far_data <<= 8;

	CRYBT_SPPCommand(far_low_data, pcRecv, 1000, TRUE);
	strSPPRecv = CString(pcRecv);
	strInfo.Format(_T("SPP far low8 Recv:%s"),strSPPRecv);
	UpdateInfo(strInfo);
	far_data |= parse_spp_rsp_data(strSPPRecv, &side);

	CString prompt;
	prompt.Format(_T("入耳校准值为:0X%04X, 出耳校准值为：0x%04X"), near_data, far_data);

	AfxMessageBox(prompt);

	delete pcRecv;
	pcRecv = NULL;
}


LRESULT CCRY574ProMFCDemoDlg::OnUpdatePrompt(WPARAM wParam, LPARAM lParam)
{
	TCHAR *pText = (TCHAR *)wParam;
	BOOL bDelete = (BOOL)lParam;

	CString infoText;
	infoText = _T("INFO: ") + CString(pText);
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
	// TODO: 在此添加控件通知处理程序代码
	if (bRunning)
	{
		AfxMessageBox(_T("正在测试，请等待"));
		return;
	}

	m_edit_status.SetWindowText(_T("测试中..."));
	m_state = STATE_INIT;

#define INIT_COLOR_REF	RGB(100, 100, 100)
	m_left_cali_status.SetForeColor(RGB(255, 255, 255));
	m_left_cali_status.SetBkColor(INIT_COLOR_REF);

	m_left_cali_value.SetForeColor(RGB(255, 255, 255));
	m_left_cali_value.SetBkColor(INIT_COLOR_REF);

	m_right_cali_status.SetForeColor(RGB(255, 255, 255));
	m_right_cali_status.SetBkColor(INIT_COLOR_REF);

	m_right_cali_value.SetForeColor(RGB(255, 255, 255));
	m_right_cali_value.SetBkColor(INIT_COLOR_REF);

	m_ep_color.SetForeColor(RGB(255, 255, 255));
	m_ep_color.SetBkColor(INIT_COLOR_REF);

	m_tws_mode.SetForeColor(RGB(255, 255, 255));
	m_tws_mode.SetBkColor(INIT_COLOR_REF);

	m_psensor_rawdata.SetForeColor(RGB(255, 255, 255));
	m_psensor_rawdata.SetBkColor(INIT_COLOR_REF);

	m_sw_version.SetForeColor(RGB(255, 255, 255));
	m_sw_version.SetBkColor(INIT_COLOR_REF);


	memset(&m_test_array[0], -1, sizeof(m_test_array));			// -1 表示初始化, TRUE 测试结果OK， FALSE 测试结果失败

	::EnableMenuItem(::GetSystemMenu(this->m_hWnd, false), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);//forbid close

	bStopped = FALSE;
	psensor_check_process();
	m_test_total++;
	m_all_count++;
	m_cycle_nr++;

	if (m_cycle_nr >= 10)
	{
		m_cycle_nr = 0;

		SaveResultToFile();
	}

}


void CCRY574ProMFCDemoDlg::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
	// 该功能要求使用 Windows 2000 或更高版本。
	// 符号 _WIN32_WINNT 和 WINVER 必须 >= 0x0500。
	// TODO: 在此处添加消息处理程序代码
}


void CCRY574ProMFCDemoDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!bRunning)
	{
		return;
	}

	bStopped = TRUE;
}


BOOL CCRY574ProMFCDemoDlg::UpdatePsensorData(psensor_cali_struct *pdata)
{
	CStatic *pWndStatus;
	CStatic *pWndData;
	CString strSide;
	CString strInfo;
	BOOL ret = FALSE;
	CString strEnSide;
	int sel = m_combox.GetCurSel();
	const TCHAR *sel_type[] =
	{
		_T("tws"),
		_T("left"),
		_T("right"),
	};

	enum 
	{
		TYPE_TWS = TEST_TWS_MODE,
		TYPE_LEFT = TEST_LEFT_EP_MODE,
		TYPE_RIGHT = TEST_RIGHT_EP_MODE
	};

	if (sel >= sizeof(sel_type) / sizeof(TCHAR *))
	{
		sel = TEST_TWS_MODE;
		m_combox.SetCurSel(sel);
	}

	if (pdata->side != LEFT_CHANNEL && pdata->side != RIGHT_CHANNEL)
	{
		AfxMessageBox(_T("耳机软件错误!"));
		return FALSE;
	}

	Log_d(_T("user select type(%d): %s, total=%d, ok_count=%d"), sel, sel_type[sel], m_test_total, m_test_ok_nr);

	if (pdata->side == LEFT_CHANNEL)
	{
		pWndStatus = &m_left_cali_status;
		pWndData = &m_left_cali_value;
		strSide = _T("左耳");
		strEnSide = _T("left earphone");
	}
	else
	{
		pWndStatus = &m_right_cali_status;
		pWndData = &m_right_cali_value;
		strSide = _T("右耳");
		strEnSide = _T("right earphone");
	}

	if (pdata->cali_flag == PSENSOR_NOT_EXIST)
	{
		strInfo.Format(_T("%s 未开机或者未组队"), strSide);
		pWndStatus->SetWindowText(strInfo);
	}
	else if (pdata->cali_flag == PSENSOR_CALI_OK)
	{
		strInfo.Format(_T("%s 光感已校准"), strSide);
		pWndStatus->SetWindowText(strInfo);
		CString prompt;

		if ((pdata->base_value > pdata->gray_value) || (pdata->gray_value - pdata->base_value < 0xB0))
		{
			prompt.Format(_T("入耳校准值:%d(%X), 出耳校准值:%d(%X), 校准失败！"), pdata->gray_value, pdata->gray_value , pdata->base_value, pdata->base_value);
			Log_d(_T("bda(%s) %s in ear value=0x%04x, out ear value=0x%04x, FAILED."), current_bt_device, strEnSide, pdata->gray_value , pdata->base_value);
			//AfxMessageBox(prompt);
		}
		else
		{
			prompt.Format(_T("入耳校准值:%d(%X), 出耳校准值:%d(%X), 校准成功！"), pdata->gray_value, pdata->gray_value, pdata->base_value, pdata->base_value);
			Log_d(_T("bda(%s) %s in ear value=0x%04x, out ear value=0x%04x, SUCCESS."), current_bt_device, strEnSide, pdata->gray_value , pdata->base_value);
			ret = TRUE;
		}

		pWndData->SetWindowText(prompt);
	}
	else if (pdata->cali_flag == PSENSOR_NOT_CALI)
	{
		strInfo.Format(_T("%s光感未校准"), strSide);
		pWndStatus->SetWindowText(strInfo);
	}
	else
	{
		pWndStatus->SetWindowText(_T("耳机数据错误"));
	}

	if ((sel == TYPE_LEFT && pdata->side == LEFT_CHANNEL) || \
		(sel == TYPE_RIGHT && pdata->side == RIGHT_CHANNEL) || \
		(sel == TYPE_TWS))
	{
		return ret;
	}
	else
	{
		return TRUE;
	}
}


BOOL CCRY574ProMFCDemoDlg::UpdateTwsModeData(tws_mode_t *agent, tws_mode_t *partner)
{
	CStatic *pWndStatus = &m_tws_mode;
	CString strSide;
	CString strInfo;
	BOOL ret = FALSE;
	CString strEnSide;
	int sel = m_combox.GetCurSel();
	const TCHAR *sel_type[] =
	{
		_T("tws"),
		_T("left"),
		_T("right"),
	};

	enum 
	{
		TYPE_TWS = TEST_TWS_MODE,
		TYPE_LEFT = TEST_LEFT_EP_MODE,
		TYPE_RIGHT = TEST_RIGHT_EP_MODE
	};

	if (sel >= sizeof(sel_type) / sizeof(TCHAR *))
	{
		sel = TYPE_TWS;
		m_combox.SetCurSel(sel);
	}

	if (agent->tws_mode == TWS_ERROR_MODE && partner->tws_mode == TWS_ERROR_MODE)
	{
		strInfo.Format(_T("左右耳未开机或者软件错误"));
		pWndStatus->SetWindowText(strInfo);

		return FALSE;
	}

	Log_d(_T("user select type(%d): %s, total=%d, ok_count=%d"), sel, sel_type[sel], m_test_total, m_test_ok_nr);

	enum 
	{
		ERROR_OK,			// NO error;
		ERROR_BOTH_LEFT,
		ERROR_BOTH_RIGHT,
		ERROR_NOT_GET_LEFT,
		ERROR_NOT_GET_RIGHT
	};

	int error_code = ERROR_OK;
	CString firstPrompt;
	CString secondPrompt;
	CString errorPrompt;
	tws_mode_t *pleft = NULL;
	tws_mode_t *pright = NULL;

	if (agent->tws_mode == TWS_PRODUCT_MODE || agent->tws_mode == TWS_USER_MODE)
	{
		if (agent->tws_side == LEFT_CHANNEL)
		{
			pleft = agent;
		}
		else if (agent->tws_side == RIGHT_CHANNEL)
		{
			pright = agent;
		}
		else
		{
			Log_e(_T("agent data error: tws_mode=%d, channel=%d"), agent->tws_mode, agent->tws_side);
		}
	}
	
	if (partner->tws_mode == TWS_PRODUCT_MODE || partner->tws_mode == TWS_USER_MODE)
	{
		if (partner->tws_side == LEFT_CHANNEL)
		{
			if (pleft)
			{
				error_code = ERROR_BOTH_LEFT;
				errorPrompt = _T("重复左声道");
			}

			pleft = partner;
		}
		else if (partner->tws_side == RIGHT_CHANNEL)
		{
			if (pright)
			{
				error_code = ERROR_BOTH_RIGHT;
				errorPrompt = _T("重复右声道");
			}

			pright = partner;
		}
		else
		{
			Log_e(_T("partner data error: tws_mode=%d, channel=%d"), partner->tws_mode, partner->tws_side);
		}

		secondPrompt = strSide + strInfo;
	}

	if (pleft)
	{
		if (pleft->tws_mode == TWS_PRODUCT_MODE)
		{
			strInfo.Format(_T("产测模式"));
		}
		else
		{
			strInfo.Format(_T("用户模式"));
		}

		if (!pright)
		{
			if (errorPrompt.IsEmpty())
			{
				errorPrompt = _T("右耳未开机");
			}
		}

		firstPrompt = _T("左耳:") + strInfo + _T(",");
	}

	if (pright)
	{
		if (pright->tws_mode == TWS_PRODUCT_MODE)
		{
			strInfo.Format(_T("产测模式"));
		}
		else
		{
			strInfo.Format(_T("用户模式"));
		}

		if (!pleft)
		{
			if (errorPrompt.IsEmpty())
			{
				errorPrompt = _T("左耳未开机");
			}
		}

		secondPrompt = _T("右耳:") + strInfo + _T(",");
	}


	pWndStatus->SetWindowText(firstPrompt + secondPrompt + errorPrompt);


	if ((sel == TYPE_LEFT))
	{
		if (!pleft)
		{
			return FALSE;
		}

		if (pleft->tws_mode == TWS_USER_MODE)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
		
	if (sel == TYPE_RIGHT)
	{
		if (!pright)
		{
			return FALSE;
		}

		if (pright->tws_mode == TWS_USER_MODE)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	if (sel == TYPE_TWS)
	{
		if (!pright || !pleft)
		{
			return FALSE;
		}

		if ((pleft->tws_mode == TWS_USER_MODE) && (pright->tws_mode == TWS_USER_MODE))
		{
			return TRUE;
		}



		return FALSE;
	}
	
	return FALSE;
}

LRESULT CCRY574ProMFCDemoDlg::OnUpdateStatus(WPARAM wParam, LPARAM lParam)
{
	m_state = wParam;
	TCHAR *pStr;
	//CString *pInfo = NULL;

	if (m_state != STATE_TWS_CALI_DATA)
	{
		pStr = (TCHAR *)lParam;
	}

	if (m_state == STATE_SUCCESS)
	{
		m_edit_status.SetWindowText(_T("成功"));
		m_test_ok_nr++;
		m_all_ok_count++;
		UpdateData(FALSE);
	}
	else if (m_state == STATE_FAIL)
	{
		m_edit_status.SetWindowText(_T("失败"));
	}
	else if (m_state == STATE_ERROR)
	{
		m_edit_status.SetWindowText(_T("错误"));
	}
	else if (m_state == STATE_PROCESS)
	{
		m_edit_status.SetWindowText(_T("测试中..."));
	}
	else if (m_state == STATE_ABORT)
	{
		m_edit_status.SetWindowText(_T("终止中..."));
	}
	else if (m_state == STATE_DONE)
	{
		BOOL ret;

		::EnableMenuItem(::GetSystemMenu(this->m_hWnd, false), SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);

		ret = TRUE;
		for (int i = 0; i < TEST_UI_SHOW_NR; i++)
		{
			if (m_test_bitmap & (1 << i))
			{
				ret = ret && (m_test_array[i] == TRUE);
			}
		}

		if (ret)
		{
			dlg_update_status_ui(STATE_SUCCESS);
		}
		else
		{
			dlg_update_status_ui(STATE_FAIL);
		}
	}
	else if (m_state == STATE_CALI_STATUS)
	{
		m_left_cali_status.SetWindowText(CString(pStr));
	}
	else if (m_state == STATE_CALI_VALUE)
	{
		m_left_cali_value.SetWindowText(CString(pStr));
	}
	else if (m_state == STATE_TWS_CALI_DATA)
	{
		psensor_cali_data_t *pleft;
		psensor_cali_data_t *pright;
		BOOL ret;
		BOOL rawdata_ret;
		void *ptr = (UCHAR *)lParam;

#define FONT_WHITE_COLOR RGB(255, 255, 255)
#define FONT_BLACK_COLOR RGB(0, 0, 0)

		pleft = (psensor_cali_data_t *)lParam;
		ret = UpdatePsensorData(pleft);
		m_left_cali_status.SetForeColor(FONT_BLACK_COLOR);
		m_left_cali_value.SetForeColor(FONT_BLACK_COLOR);
		if (ret)
		{
			m_left_cali_status.SetBkColor(RGB(0, 255, 0));
			m_left_cali_value.SetBkColor(RGB(0, 255, 0));
		}
		else
		{
			m_left_cali_status.SetBkColor(RGB(255, 0, 0));
			m_left_cali_value.SetBkColor(RGB(255, 0, 0));
		}
		pright = pleft + 1;
		
		m_right_cali_status.SetForeColor(FONT_BLACK_COLOR);
		m_right_cali_value.SetForeColor(FONT_BLACK_COLOR);
		if (UpdatePsensorData(pright))
		{
			m_right_cali_status.SetBkColor(RGB(0, 255, 0));
			m_right_cali_value.SetBkColor(RGB(0, 255, 0));
		}
		else
		{
			m_right_cali_status.SetBkColor(RGB(255, 0, 0));
			m_right_cali_value.SetBkColor(RGB(255, 0, 0));
			ret = FALSE;
		}

		m_psensor_rawdata.SetForeColor(FONT_BLACK_COLOR);


		int sel = m_combox.GetCurSel();

		if (sel == TEST_TWS_MODE)
		{
			rawdata_ret = check_psensor_rawdata();
		}
		else if (sel == TEST_LEFT_EP_MODE)		// left
		{
			rawdata_ret = check_psensor_left_rawdata();
		}
		else  // right
		{
			rawdata_ret = check_psensor_right_rawdata();
		}

		if (rawdata_ret)
		{
			m_psensor_rawdata.SetBkColor(RGB(0, 255, 0));
		}
		else
		{
			m_psensor_rawdata.SetBkColor(RGB(255, 0, 0));
		}

		ret =  rawdata_ret && ret;

		CString strPrompt;
		race_rsp_rawdata_t *rawleft = get_ep_psensor_rawdata(ONEWIRE_LEFT_CHANNEL);
		race_rsp_rawdata_t *rawright = get_ep_psensor_rawdata(ONEWIRE_RIGHT_CHANNEL);


		strPrompt.Format(_T("左耳光感值:%d(0x%x), 右耳光感值:%d(0x%x)"), rawleft->raw_data, rawleft->raw_data, rawright->raw_data, rawright->raw_data);
		m_psensor_rawdata.SetWindowText(strPrompt);

		m_test_array[TEST_PSENSOR_INDEX] = ret;

		delete ptr;
	}
	else if (m_state == STATE_TWS_MODE_DATA)
	{
		tws_mode_t *agent;
		tws_mode_t *partner;
		BOOL ret;
		void *ptr = (UCHAR *)lParam;

		agent = (tws_mode_t *)lParam;
		partner = agent + 1;
		ret = UpdateTwsModeData(agent, partner);
		m_test_array[TEST_USER_MODE_INDEX] = ret;
		m_tws_mode.SetForeColor(FONT_BLACK_COLOR);
		if (ret)
		{
			m_tws_mode.SetBkColor(RGB(0, 255, 0));
		}
		else
		{
			m_tws_mode.SetBkColor(RGB(255, 0, 0));
		}

		delete ptr;
	}
	else if (m_state == STATE_TWS_VERSION_DATA)
	{
		tws_sw_version_t *ptr = (tws_sw_version_t *)lParam;
		TCHAR *pVersion;
		BOOL ret = FALSE;
		CString strPrompt;
		CString strVersion;

		pVersion = ptr->pAgent;
		if (pVersion)
		{
			strVersion = CString(pVersion);
			strPrompt = _T("主耳版本：") +  strVersion + _T(",");

			if (strVersion == m_default_sw_version)
			{
				ret = TRUE;
			}
		}

		pVersion = ptr->pPartner;
		if (pVersion)
		{
			strVersion = CString(pVersion);
			strPrompt += _T("付耳版本：") + strVersion;

			ret = ret && (strVersion == m_default_sw_version);
		}

		m_sw_version.SetWindowText(strPrompt);
		
		m_test_array[TEST_SW_VERSION_INDEX] = ret;
		m_sw_version.SetForeColor(FONT_BLACK_COLOR);

		if (ret)
		{
			m_sw_version.SetBkColor(RGB(0, 255, 0));
		}
		else
		{
			m_sw_version.SetBkColor(RGB(255, 0, 0));
		}

		if (ptr->pAgent)
		{
			delete ptr->pAgent;
		}

		if (ptr->pPartner)
		{
			delete ptr->pPartner;
		}

		delete ptr;
	}
	else if (m_state == STATE_TWS_EP_COLOR)
	{
		int *ptr = (int *)lParam;
		int *tmp = ptr;
		int left_color;
		int right_color;
		BOOL ret = FALSE;
		CString strPrompt;
		CString strColor;

		left_color = *ptr++;
		right_color = *ptr++;

		int color_value = get_test_item_setting_value(_T("color"));			// 

		int sel = m_combox.GetCurSel();

		if (sel == TEST_TWS_MODE)
		{
			ret = (left_color == color_value) && (right_color == color_value);
		}
		else if (sel == TEST_LEFT_EP_MODE)
		{
			ret = (left_color == color_value) ;
		}
		else
		{
			ret = (right_color == color_value);
		}
		
		m_test_array[TEST_EP_COLOR_INDEX] = ret;

		if (left_color == EP_COLOR_BLACK)
		{
			strColor = CString(_T("黑色"));
		}
		else if (left_color == EP_COLOR_WHITE)
		{
			strColor = CString(_T("白色"));
		}
		else 
		{
			strColor = CString(_T("未知"));
		}

		strPrompt = _T("左耳：") +  strColor + _T(",");

		if (right_color == EP_COLOR_BLACK)
		{
			strColor = CString(_T("黑色"));
		}
		else if (right_color == EP_COLOR_WHITE)
		{
			strColor = CString(_T("白色"));
		}
		else 
		{
			strColor = CString(_T("未知"));
		}

		strPrompt += _T("右耳：") +  strColor;
		m_ep_color.SetWindowText(strPrompt);
		m_ep_color.SetForeColor(FONT_BLACK_COLOR);
		if (ret)
		{
			m_ep_color.SetBkColor(RGB(0, 255, 0));
		}
		else
		{
			m_ep_color.SetBkColor(RGB(255, 96, 96));
		}
	
		delete tmp;
	}
	else if (m_state == STATE_PSENSOR_CALIBRATE_DONE)
	{
		int result = (int)lParam;

		m_test_array[TEST_PSENSOR_CALI_INDEX] = result;
	}
	else if (m_state >= STATE_PSENSOR_SEND_CALIBRATE_OUT_EAR && m_state <=  STATE_PSENSOR_QUERY_CALIBRATE_IN_EAR)
	{
		m_edit_status.SetWindowText(CString(pStr));
	}

	return 0;
}

/*
 * 根据测试结果对文字进行背景渲染
 */
HBRUSH CCRY574ProMFCDemoDlg::ChangeCtrlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return NULL;
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
		else if (m_state == STATE_FAIL || m_state == STATE_ERROR || m_state == STATE_ABORT)
		{
			pDC->SetTextColor(RGB(0,0,0));
			return m_FailBrushBack;
		}
	}

	HBRUSH hbr;
	hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}


void CCRY574ProMFCDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
	static int count = 0;

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_AUTOTEST);
	m_auto_test = pBtn->GetCheck();

	if (nIDEvent == AUTOTEST_TIMER_ID)
	{
		if (!bRunning)
		{
			count++;
			if (count >= 3)		// 延迟两秒
			{
				if (m_auto_test && !bStopped)
				{
					//psensor_check_process();
					//m_test_total++;
					OnBnClickedButton4();
				}
			}
		}
		else
		{
			count = 0;
		}
	}

	UpdateData(FALSE);		// 变量值传递到控件

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
}

LRESULT CCRY574ProMFCDemoDlg::OnProcessUartMsg(WPARAM wParam, LPARAM lParam)
{
	if (wParam == UART_USER_CMD_START_TESTING)
	{
		OnBnClickedButton4();
	}

	return 0;
}


void CCRY574ProMFCDemoDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	m_test_total = 0;
	m_test_ok_nr = 0;

	SaveResultToFile();
	UpdateData(FALSE);
}


void CCRY574ProMFCDemoDlg::OnBnClickedGpibTest()
{
	// TODO: 在此添加控件通知处理程序代码
}
