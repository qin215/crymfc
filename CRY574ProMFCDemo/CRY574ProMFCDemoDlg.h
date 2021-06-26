
// CRY574ProMFCDemoDlg.h : 头文件
//

#pragma once
#include <vector>
#include "afxwin.h"
#include "protocol.h"
using namespace std;


#define WM_UPDATE_STATIC (WM_USER + 100)  
#define WM_UPDATE_STATUS (WM_USER + 101)

#define AUTOTEST_TIMER_ID		1

// CCRY574ProMFCDemoDlg 对话框
class CCRY574ProMFCDemoDlg : public CDialogEx
{
// 构造
public:
	CCRY574ProMFCDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CRY574PROMFCDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	
	afx_msg void OnDestroy();

	CEdit m_editShow;
	afx_msg void OnBnClickedButtonIni();



private:
	void UpdateInfo(CString strInfo);
	LRESULT OnUpdatePrompt(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateStatus(WPARAM wParam, LPARAM lParam);

	BOOL UpdatePsensorData(psensor_cali_struct *pdata);

	CButton m_button_start;
	CEdit  m_edit_status;

	CButton m_button_stop;

	CBrush  m_SuccessBrushBack;
	CBrush  m_FailBrushBack;
	CBrush  m_ProcessBrushBack;

	INT		m_state;

	CStatic m_left_cali_status;
	CStatic m_left_cali_value;

	CStatic m_right_cali_status;
	CStatic m_right_cali_value;

	CComboBox m_combox;

	BOOL m_auto_test;

	INT m_test_total;
	INT m_test_ok_nr;


public:
	afx_msg void OnBnClickedButtonInquiry1();
	afx_msg void OnBnClickedButtonInquiry2();
	afx_msg void OnBnClickedButtonInquiry3();
	afx_msg void OnBnClickedButtonMaccon();
	afx_msg void OnBnClickedButtonAutocon();
	afx_msg void OnBnClickedButtonDiscon();
	afx_msg void OnBnClickedButtonA2dp();
	afx_msg void OnBnClickedButtonHfp();
	afx_msg void OnBnClickedButtonGetinfo();
	afx_msg void OnBnClickedButtonRelease();
	afx_msg void OnBnClickedButtonBreak();
	CString m_strATCmd;
	afx_msg void OnBnClickedButtonSendat();
	BOOL m_bHex;
	afx_msg void OnBnClickedButtonSppConfig();
	afx_msg void OnBnClickedButtonSpp();
	afx_msg void OnBnClickedButtonDisconnectSpp();
	afx_msg void OnBnClickedButtonSendspp();
	CString m_strSppCmd;
	CString m_strMacAddr;
	void Test(unsigned char * p,unsigned char *o,int len);
	afx_msg void OnBnClickedButtonPair();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();

	
	afx_msg void OnBnClickedButton4();
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	afx_msg void OnBnClickedButtonStop();


	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
