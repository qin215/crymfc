// CRY574ProMFCDemoDlg.cpp : 实现文件
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
 * 函数功能 : 点击"连接设备"按钮时，该函数被调用
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
       // m_StaticDeviceState.Format("设备状态：已连接！");
       // m_StaticDeviceInfo.Format("设备信息：%s \n", receiveBufferArrary);
       // UpdateData(FALSE);
    }
    else
    {
       // m_IsConnected = false;
        AfxMessageBox(_TEXT("连接设备失败！"), MB_OK | MB_ICONWARNING);
    }
}