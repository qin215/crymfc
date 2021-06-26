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
		STATE_CALI_VALUE,
		STATE_TWS_CALI_DATA
	};

	enum 
	{
		PSENSOR_NOT_EXIST = 0XFF,		// 耳机单边
		PSENSOR_CALI_OK = 1,			// 已校准
		PSENSOR_NOT_CALI = 0		// 未校准
	};

	enum 
	{
		LEFT_CHANNEL = 0,
		RIGHT_CHANNEL
	};

#pragma pack(push)
#pragma pack(1)
	typedef struct psensor_cali_struct
	{
		UCHAR side;
		UCHAR cali_flag;
		UINT16 base_value;
		UINT16 gray_value;
	} psensor_cali_data_t;
#pragma pack(pop)
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

BOOL parse_spp_rsp_data_2(CString& strRSP, psensor_cali_data_t *left_earphone, 
						  psensor_cali_data_t *right_earphone);

void dlg_update_status_data(INT state, void *data_ptr);

extern CWinThread *pWorkThread;
extern BOOL bStopped;
extern BOOL bRunning;

#ifdef __cplusplus
}
#endif
#endif