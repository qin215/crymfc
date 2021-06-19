
// CRY574ProMFCDemoDlg.h : ͷ�ļ�
//

#pragma once
#include <vector>
#include "afxwin.h"
using namespace std;

// CCRY574ProMFCDemoDlg �Ի���
class CCRY574ProMFCDemoDlg : public CDialogEx
{
// ����
public:
	CCRY574ProMFCDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CRY574PROMFCDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
};
