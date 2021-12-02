// CRY574ProMFCDemoDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "CRY574ProMFCDemo.h"
#include "CRY574ProMFCDemoDlg.h"
#include "afxdialogex.h"
#include "protocol.h"
#include "mywin.h"
#include "uart_cmd.h"
#include "GPIB.h"

#include "include/visa.h"
#pragma comment(lib, "visa32.lib")

static ViSession m_ViSessionRM;
static ViSession m_ViSession;

static BOOL m_IsConnected = FALSE;
static BOOL m_power1_on = FALSE;
static BOOL m_power2_on = FALSE;;

static double m_voltage = 0.0;
static double m_current = 0.4;

static void send_vi_cmd(const char *str_cmd);

/*
 * 函数功能 : 点击"连接设备"按钮时，该函数被调用
 */
BOOL GPIBOpenDevice()
{
	if (m_IsConnected)
	{
		return TRUE;
	}

#if 1
    viOpenDefaultRM(&m_ViSessionRM);

    if (VI_SUCCESS == viOpen(m_ViSessionRM, "GPIB0::6::INSTR", VI_NULL, VI_NULL, &m_ViSession))
    {
        char receiveBufferArrary[256] = {0};

        viPrintf(m_ViSession, "*RST\n");
        viPrintf(m_ViSession, "*IDN?\n"); 

        viScanf(m_ViSession, "%t", &receiveBufferArrary);

        m_IsConnected = true;
		CString strState;
        strState.Format(_TEXT("已连接！"));
		dlg_update_ui(strState);

		CString strPrompt(receiveBufferArrary);
		dlg_update_ui(strPrompt);

		viSetAttribute(m_ViSession, VI_ATTR_TMO_VALUE, 5000);			// 超时时间5s
		return TRUE;
    }
    else
    {
        m_IsConnected = false;
        AfxMessageBox(_TEXT("连接设备失败！"), MB_OK | MB_ICONWARNING);

		return FALSE;
    }
#else
	return FALSE;
#endif
}

/*
 * 开关对应通道的电压
 */
void EnableChannelVoltage(BOOL on, int channel)
{
	ViByte buff[128];

	if (on)
	{
		SetupChannelVoltage(m_voltage, channel);
	}
	else
	{
		sprintf_s((char *)buff, sizeof(buff), ":OUTPut%d:STAT OFF", channel);
		send_vi_cmd((const char *)buff);
	}

	if (channel == CHANNEL_ONE)
	{
		m_power1_on = on;
	}
	else
	{
		m_power2_on = on;
	}
}


/*
 * 设置对应通道的电压
 */
void SetupChannelVoltage(double voltage, int channel)
{
	if (!m_IsConnected)
	{
		 AfxMessageBox(_TEXT("请先连接设备！"), MB_OK | MB_ICONWARNING);
		 return;
	}

	if ((voltage < 0.0) || (voltage > 5.0))
	{
		AfxMessageBox(_TEXT("输入电压范围（0v~5v)不对！"), MB_OK | MB_ICONWARNING);
		return;
	}

	if ((m_current <= 0.0) || (m_current > 0.5))
	{
		AfxMessageBox(_TEXT("输入电流范围（0A~0.5A)不对!"), MB_OK | MB_ICONWARNING);
		return;
	}

	if (channel != CHANNEL_ONE && channel != CHANNEL_TWO)
	{
		AfxMessageBox(_TEXT("通道范围（1，2)不对!"), MB_OK | MB_ICONWARNING);
		return;
	}

	ViByte buff[128];

	m_voltage = voltage;

	sprintf_s((char *)buff, sizeof(buff), ":SOUR%d:VOLT:LEVel %2.1f", channel, m_voltage);
	send_vi_cmd((const char *)buff);
	sprintf_s((char *)buff, sizeof(buff), "SOUR%d:CURRent:LIMit:VALue %1.1f", channel, m_current);
	send_vi_cmd((const char *)buff);
	sprintf_s((char *)buff, sizeof(buff), ":OUTPut%d:STAT ON", channel);
	send_vi_cmd((const char *)buff);
}

/*
 * 发送指令给仪器
 */
static void send_vi_cmd(const char *str_cmd)
{
	ViStatus status = 0;
	ViUInt32 retlen = 0;

#if 1
	status = viWrite(m_ViSession, (ViByte *)str_cmd, (ViUInt32)strlen(str_cmd), &retlen);
	if (status < VI_SUCCESS)
	{
		printf("viWrite error=%d!\n", status);
		return;
	}
	else
	{
		printf("viWrite ok\n");
	}
#endif
}


void GPIBCloseDevice()
{
	if (!m_IsConnected)
	{
		return;
	}

	m_IsConnected = false;
#if 1
	viClose(m_ViSession);
	viClose(m_ViSessionRM);
#endif
}
