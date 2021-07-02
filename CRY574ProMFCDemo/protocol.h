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

	typedef struct race_cmd_relay_header_struct 
	{
		BYTE dst_type;
		BYTE dst_id;
		BYTE partner_data[1];
	} relay_rsp_t;

#define payload u.rpayload

	typedef struct race_cmd_struct 
	{
		UCHAR frame_start;
		UCHAR frame_type;
		UINT16 frame_len;
		UINT16 frame_cmd;

		union 
		{
			BYTE rpayload[1];
			relay_rsp_t rsp; 
		} u;

	} race_cmd_t;


#pragma pack(pop)
#define API_OK 0

#ifndef kal_uint8 
#define kal_uint8 unsigned char
#endif

	enum _RACE_CMD_ENUM 
	{
		PSENSOR_RACE_CAL_CT = 3,
		PSENSOR_RACE_CAL_G2 = 4,
		PSENSOR_RACE_SWITCH_DBG = 5,
		PSENSOR_RACE_ENTER_DISCOVERY = 6,
		PSENSOR_RACE_ENTER_TWS_PAIRING = 7,
		PSENSOR_RACE_SENSOR_INIT_OK = 8,			// ONLY for debuggging
		PSENSOR_RACE_IN_EAR = 9,
		PSENSOR_RACE_OUT_EAR = 0xA,
		PSENSOR_RACE_RESET_FACTORY = 0XB,
		PSENSOR_RACE_ANC_ON = 0XC,
		PSENSOR_RACE_ANC_OFF = 0XD,
		PSENSOR_RACE_SPP_LOG_ON = 0XE,
		PSENSOR_RACE_SPP_LOG_OFF = 0XF,

		PSENSOR_DUMP_INFO = 0X10,
		PSENSOR_TEST_FAST_PAIRING = 0X11,
		PSENSOR_CHECK_CUSTOMER_UI = 0X12,
		PSENSOR_CHECK_PRODUCT_MODE = 0X13,
		PSENSOR_CHECK_PSENSOR_SIMU = 0X14,
		PSENSOR_SET_PRODUCT_MODE = 0x15,
		PSENSOR_CLEAN_PRODUCT_MODE = 0x16,
		PSENSOR_GET_CALI_STATUS = 0x17,

		PSENSOR_SET_CUSTOMER_UI = 0X18,
		PSENSOR_CLEAN_CUSTOMER_UI = 0X19,

		PSENSOR_GET_INEAR_STATUS = 0x1A,


		PSENSOR_GET_NEAR_THRESHOLD_HIGH = 0X1B,
		PSENSOR_GET_NEAR_THRESHOLD_LOW = 0X1C,

		PSENSOR_GET_FAR_THRESHOLD_HIGH = 0X1D,
		PSENSOR_GET_FAR_THRESHOLD_LOW = 0X1E,

		PSENSOR_GET_RAW_DATA_HIGH = 0X20,
		PSENSOR_GET_RAW_DATA_LOW = 0x21,

		PSENSOR_QUERY_CALI_STATUS = 0X22,
		PSENSOR_ONE_PARAM_END = 0X23,

		PSENSOR_GET_CALI_DATA = 0X30,
		PSENSOR_GET_RAW_DATA = 0x31,
	};


#define RACE_CMD_FRAME_START 0X5
#define RACE_CMD_REQ	0X5A
#define RACE_CMD_RSP	0x5B
#define RACE_CMD_RELAY_RSP 0x5D
#define RACE_CMD_GET_PARTNER_ID 0X0D00
#define RACE_CMD_RELAY_PARTER_CMD	0x0D01





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