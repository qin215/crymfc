// CRY574ProMFCDemoDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "CRY574ProMFCDemo.h"
#include "CRY574ProMFCDemoDlg.h"
#include "afxdialogex.h"
#include "protocol.h"
#include "mywin.h"
#include "uart_cmd.h"

#include "include/visa.h"
#pragma comment(lib, "msc/visa32.lib")

extern "C" 
{
	void GPIBOpenDevice();
};


ViSession m_ViSessionRM;
ViSession m_ViSession;

/*
 * �������� : ���"�����豸"��ťʱ���ú���������
 */
void GPIBOpenDevice()
{
    viOpenDefaultRM(&m_ViSessionRM);

    if (VI_SUCCESS == viOpen(m_ViSessionRM, "GPIB0::1::INSTR", VI_NULL, VI_NULL, &m_ViSession))
    {
        char receiveBufferArrary[256] = {0};

        viPrintf(m_ViSession, "*RST\n");
        viPrintf(m_ViSession, "*IDN?\n"); 

        viScanf(m_ViSession, "%t", &receiveBufferArrary);

       // m_IsConnected = true;
       // m_StaticDeviceState.Format("�豸״̬�������ӣ�");
       // m_StaticDeviceInfo.Format("�豸��Ϣ��%s \n", receiveBufferArrary);
       // UpdateData(FALSE);
    }
    else
    {
       // m_IsConnected = false;
        AfxMessageBox(_TEXT("�����豸ʧ�ܣ�"), MB_OK | MB_ICONWARNING);
    }
}