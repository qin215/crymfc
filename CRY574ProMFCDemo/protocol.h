#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#define uint8_t unsigned char
#define uint16_t unsigned short
#define kal_uint16 unsigned short

#ifndef Boolean
#define Boolean BOOL
#endif

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
		STATE_TWS_CALI_DATA,
		STATE_TWS_MODE_DATA,
		STATE_TWS_VERSION_DATA
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

	typedef struct race_cmd_sw_version_struct 
	{
		BYTE status;
		BYTE recv_count;
		BYTE role;
		BYTE str_len;
		CHAR version[1];
	} race_rsp_sw_version_t;

	typedef struct race_cmd_anc_gain_struct 
	{
		BYTE status;
		BYTE id;
		kal_uint16 left_ff_gain;
		kal_uint16 left_fb_gain;
		kal_uint16 right_ff_gain;
		kal_uint16 right_fb_gain;
		kal_uint16 left_spk_gain;
		kal_uint16 right_spk_gain;
	} race_rsp_anc_gain_t;

	typedef struct race_cmd_nvkey_struct 
	{
		kal_uint16 nvkey_len;
		byte data[1];
	} race_rsp_nvkey_t;

#define payload u.rpayload
#define sw_ver_rsp u.sw_rsp
#define anc_gain_rsp u.gain_rsp
#define nvkey_value_rsp u.nvkey_rsp

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
			race_rsp_sw_version_t sw_rsp;
			race_rsp_anc_gain_t gain_rsp;
			race_rsp_nvkey_t nvkey_rsp;
		} u;

	} race_cmd_t;



	typedef struct onewire_struct 
	{
		uint8_t header;
		uint8_t type;
		uint16_t len;
		uint16_t cmd;
		uint8_t event;
		uint8_t side;
		uint8_t param[1];
	}  onewire_frame_t;

#pragma pack(pop)
#define API_OK 0

#ifndef kal_uint8 
#define kal_uint8 unsigned char
#endif

#define FUNC_DLL_EXPORT

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


#define RACE_CMD_FRAME_START		0X5
#define RACE_CMD_REQ				0X5A
#define RACE_CMD_RSP				0x5B
#define RACE_CMD_RELAY_RSP			0x5D
#define RACE_CMD_GET_PARTNER_ID		0X0D00
#define RACE_CMD_RELAY_PARTER_CMD	0x0D01
#define RACE_CMD_GET_SW_VERSION		0x1C07


#define RACE_CMD_WRITE_ANC_GAIN		0X0E06

#define RACE_CMD_READ_NVKEY			0X0A00

/*
 * return true use customer ui, otherwise use system ui.
 */
#define CUSTOMER_UI_INDEX 					0
#define CUSTOMER_PRODUCT_INDEX			1
#define CUSTOMER_PSENSOR_SIM_INDEX 		2			// 开发板模拟出入耳
#define CUSTOMER_SPP_LOG_INDEX			3
#define CUSTOM_CONF_NUM					(CUSTOMER_SPP_LOG_INDEX + 1)

#define SPP_RSP_ERROR	0xFFFFFFFF
#define TWS_PARTNER_ID 0x5

enum 
{
	TWS_USER_MODE = 0,		// 用户模式
	TWS_PRODUCT_MODE,		// 产测模式
	TWS_ERROR_MODE
};

enum 
{
	TEST_PSENSOR_INDEX = 0,
	TEST_USER_MODE_INDEX,
	TEST_SW_VERSION_INDEX,
	TEST_UI_SHOW_NR = 8,			// 0~7 进行UI显示
	TEST_WRITE_ANC_GAIN_INDEX = 8,
	TEST_FACTORY_RESET_INDEX,		// 恢复出厂设置最后一项
	TEST_NR
};


typedef struct tws_mode_struct
{
	UCHAR tws_side;
	UCHAR tws_mode;
} tws_mode_t;

typedef struct tws_sw_version_struct
{
	TCHAR *pAgent;
	TCHAR *pPartner;
} tws_sw_version_t;

typedef UINT32 (*race_cmd_rsp_callback_func)(race_cmd_t *pcmd , int data_len, int *pside);

extern CWinThread *pWorkThread;
extern BOOL bStopped;
extern BOOL bRunning;

extern CString current_bt_name;

UINT32 parse_race_cmd_rsp(race_cmd_t *pdata, int data_len, int *pside);

void dlg_update_ui(const CString& promptinfo);

INT32 psensor_check_process();

UINT32 parse_spp_rsp_data(CString& strRSP, int *pside);

int Char2Int(TCHAR c);

BYTE Hex2Char(BYTE data);

BYTE Char2Hex(BYTE hex);

int String2HexData(const CString &in_str, UCHAR * outBuffer);

int Binary2HexData(const UCHAR * inBuff, const int len, UCHAR *outBuff, int out_len);

void dlg_update_status_ui(INT state,const CString& info = _T("oK"));

BOOL parse_spp_rsp_data_2(CString& strRSP, psensor_cali_data_t *left_earphone, 
						  psensor_cali_data_t *right_earphone);

void dlg_update_status_data(INT state, void *data_ptr);

const TCHAR * get_tws_side_str(int side);

void check_tws_mode();

/*
 * buffer中找到关键字所表示的帧数据
 */
onewire_frame_t * onewire_get_one_rsp_frame(BYTE id1, BYTE id2, kal_uint8 * protocol_buffer, int *plen);

TCHAR * get_agent_sw_version();
TCHAR * get_partner_sw_version();

UINT32 get_partner_id(CString& strRSP);

UCHAR * cons_relay_race_cmd(UCHAR id, LPCCH cmd_str);


BYTE *get_partner_rsp_str(BYTE id, const CString& strRsp, int *plen);

UINT32 send_partner_relay_cmd_help(const char *partner_cmd, race_cmd_rsp_callback_func cbFunc);

void check_software_version();

FUNC_DLL_EXPORT Boolean get_config_string_value(const TCHAR *segment, const TCHAR *key, const TCHAR *default_value, TCHAR *out_buf, int len);


FUNC_DLL_EXPORT Boolean get_config_int_value(const TCHAR *segment, const TCHAR *key, int *pvalue, int default_value);

BOOL write_agent_anc_gain();

BOOL write_partner_anc_gain();

void send_system_factory_cmd();

int get_test_item_setting_bitmap();

void test_read_nvkey();

#ifdef __cplusplus
}
#endif
#endif