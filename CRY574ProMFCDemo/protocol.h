#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#define uint8_t unsigned char
#define uint16_t unsigned short

	enum 
	{
		STATE_INIT,
		STATE_SUCCESS,
		STATE_FAIL,
		STATE_ABORT,
		STATE_PROCESS,
		STATE_ERROR,
		STATE_DONE,
		STATE_CALI_STATUS,
		STATE_CALI_VALUE
	};

#define API_OK 0

UINT32 parse_race_cmd_rsp(const uint8_t *pdata, int data_len);

void dlg_update_ui(const CString& promptinfo);

INT32 psensor_check_process();

UINT32 parse_spp_rsp_data(CString& strRSP);

int Char2Int(TCHAR c);

BYTE Hex2Char(BYTE data);

BYTE Char2Hex(BYTE hex);

int String2HexData(const CString &in_str, UCHAR * outBuffer);

int Binary2HexData(const UCHAR * inBuff, const int len, UCHAR *outBuff, int out_len);

void dlg_update_status_ui(INT state,const CString& info = _T("oK"));

extern CWinThread *pWorkThread;
extern BOOL bStopped;
extern BOOL bRunning;

#ifdef __cplusplus
}
#endif
#endif