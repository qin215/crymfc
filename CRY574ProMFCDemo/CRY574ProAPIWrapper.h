#pragma once
#ifdef CRY574PROAPI_EXPORTS
#define CRY574PRO_API __declspec(dllexport)
#else
#define CRY574PRO_API __declspec(dllimport)
#endif
#define CRY_BT_ERRORCODE int
//初始化句柄  PROA-Vid = 0x0483 Pid = 0x5750			PROB-Vid = 0x0483 Pid = 0x5751
EXTERN_C BOOL CRY574PRO_API CRYBT_InitializePro(WORD Vid = 0x0483,WORD Pid = 0x5750); //初始化设备
//获取句柄vid pid  判定dongle类型  
EXTERN_C void CRY574PRO_API CRYBT_GetDeviceID(WORD& vid, WORD& pid);
//释放资源
EXTERN_C void CRY574PRO_API CRYBT_Release();
//线程间中断 配对 搜索等操作
EXTERN_C void CRY574PRO_API CRYBT_SetBreak();

//恢复出厂配置  目前只支持1个模式
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetDefaultProfile(int nProfileMode =0);
//设置dongle 搜索功率  范围 -18 ~ 6
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetSearchPower(int nPower);//-18 - 6
//设置耳机配对密码  //default "0000"
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetPassword(const char* pcPassword);//default "0000"
//0->44100  1->48000
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetSampleRate(int sampleRateIndex = 0);//0->44100  1->48000
//设置hfp  宽窄带    0,1 宽带  2，3窄带   两套指令以防切换模式不成功
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetBandMode(int bandModeIndex);
//复位dongle  
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_ResetDongle();
//取消配对
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_UnPairBtDevice();
//清空连接协议
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_CloseLists();
//取消搜索
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_InquiryCancel();
//接听电话
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_AnswerCall();


//断开连接
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_Disconnect();
//搜索一次一定时间内所有处于配对模式蓝牙设备
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_InquiryAllOneTime(int nInquiryTimeS,BOOL bWithName,int& nCount);
//一定时间内搜索多次
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_InquiryAllLoop(int nLoopTimeoutS,int nInquiryTimeS,BOOL bWithName,int& nCount);
//搜索到第一个蓝牙设备后返回
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_InquiryFirstOneTime(char* pcMacAddr,int nInquiryTimeS = 5);
		 
//根据搜索设备数量，获取mac
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetInquiryMac(int nIndex,char* pcMacAddr);
//根据搜索设备数量，获取Name
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetInquiryName(int nIndex,char* pcName);
//根据搜索设备数量，获取Rssi
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetInquiryRssi(int nIndex,int& nValue);
		 
//mac配对
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_Pair(const char* pcMacAddr,int nPairTimeoutMS = 5000);
//mac连接
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_Connect(const char* pcMacAddr,int nConnectCode = 1016,int nPairTimeoutMS = 5000,int nSleepMSAfterPair = 0,int nCon1TimeoutMS = 12000,int nCon2TimeoutMS = 10000);
//mac连接
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_MacConnect(const char* pcMacAddr,int nConnectCode = 1016,int nPairTimeoutMS = 5000,int nSleepMSAfterPair = 0,int nCon1TimeoutMS = 12000,int nCon2TimeoutMS = 10000);
//自动连接
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_AutoConnect(int nTimeOutMS,int nInquiryTimeS = 5,int nConnectCode = 1016,int nPairTimeoutMS = 5000,int nSleepMSAfterPair = 0,int nCon1TimeoutMS = 12000,int nCon2TimeoutMS = 10000);
//切换至a2dp
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_A2dpStreaming(int nTimeOutMS = 3000);
//切换至HFP
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_HfpStreaming(int nHFPStateIndex,int nTimeOutMS = 5000,BOOL bCmd = FALSE); 

//设置hfp音量
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetHFPVolume(int nVol);
//设置a2dp音量
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetA2DPVolume(int nVol);
//获取hfp音量
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetHFPVol(int& nValue);
//获取a2dp音量
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetA2dpVol(int& nValue);
//查询a2dp音量
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_QuiryA2dpVol(int& nValue,int nTimeoutMS = 1000);
//获取电量
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetBattery(int& nValue);
//查询电量
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_QuiryBattery(int& nValue,int nTimeoutMS = 5000);
//获取名字
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetName(char* pcName);
//查询名字
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_QuiryName(char* pcName,int nTimeoutMS = 2000);
//根据mac地址查询名字
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_QuiryNameWithMacAddr(const char* pcMacAddr,char* pcName,int nTimeoutMS = 2000);
//获取rssi
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetRssi(int& nValue);
//查询rssi
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_QuiryRssi(int& nValue,int nTimeoutMS = 2000);
//获取连接Mac地址
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetConnectMacAddr(char* pcMacAddr);
//获取当前模式 0-unknown 1-a2dp 2-dial 3-ring
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetCurrentMode(int& nMode);//0 = UnKnow, 1=A2DP, 2 = HFP


EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_ATCommand(const char* pcATSend,char* pcATRecv,int nTimeoutMS);

//spp连接
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_ConnectSPP(const char* pcMacAddr,int nCon1TimeoutMS = 5000);
//断开spp连接
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_DisConnectSPP();
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SPPCommand(const char* pcSPPSend,char* pcSPPRecv,int nTimeoutMS,BOOL bHex);


