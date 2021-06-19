#pragma once
#ifdef CRY574PROAPI_EXPORTS
#define CRY574PRO_API __declspec(dllexport)
#else
#define CRY574PRO_API __declspec(dllimport)
#endif
#define CRY_BT_ERRORCODE int
//��ʼ�����  PROA-Vid = 0x0483 Pid = 0x5750			PROB-Vid = 0x0483 Pid = 0x5751
EXTERN_C BOOL CRY574PRO_API CRYBT_InitializePro(WORD Vid = 0x0483,WORD Pid = 0x5750); //��ʼ���豸
//��ȡ���vid pid  �ж�dongle����  
EXTERN_C void CRY574PRO_API CRYBT_GetDeviceID(WORD& vid, WORD& pid);
//�ͷ���Դ
EXTERN_C void CRY574PRO_API CRYBT_Release();
//�̼߳��ж� ��� �����Ȳ���
EXTERN_C void CRY574PRO_API CRYBT_SetBreak();

//�ָ���������  Ŀǰֻ֧��1��ģʽ
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetDefaultProfile(int nProfileMode =0);
//����dongle ��������  ��Χ -18 ~ 6
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetSearchPower(int nPower);//-18 - 6
//���ö����������  //default "0000"
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetPassword(const char* pcPassword);//default "0000"
//0->44100  1->48000
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetSampleRate(int sampleRateIndex = 0);//0->44100  1->48000
//����hfp  ��խ��    0,1 ���  2��3խ��   ����ָ���Է��л�ģʽ���ɹ�
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetBandMode(int bandModeIndex);
//��λdongle  
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_ResetDongle();
//ȡ�����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_UnPairBtDevice();
//�������Э��
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_CloseLists();
//ȡ������
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_InquiryCancel();
//�����绰
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_AnswerCall();


//�Ͽ�����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_Disconnect();
//����һ��һ��ʱ�������д������ģʽ�����豸
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_InquiryAllOneTime(int nInquiryTimeS,BOOL bWithName,int& nCount);
//һ��ʱ�����������
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_InquiryAllLoop(int nLoopTimeoutS,int nInquiryTimeS,BOOL bWithName,int& nCount);
//��������һ�������豸�󷵻�
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_InquiryFirstOneTime(char* pcMacAddr,int nInquiryTimeS = 5);
		 
//���������豸��������ȡmac
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetInquiryMac(int nIndex,char* pcMacAddr);
//���������豸��������ȡName
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetInquiryName(int nIndex,char* pcName);
//���������豸��������ȡRssi
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetInquiryRssi(int nIndex,int& nValue);
		 
//mac���
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_Pair(const char* pcMacAddr,int nPairTimeoutMS = 5000);
//mac����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_Connect(const char* pcMacAddr,int nConnectCode = 1016,int nPairTimeoutMS = 5000,int nSleepMSAfterPair = 0,int nCon1TimeoutMS = 12000,int nCon2TimeoutMS = 10000);
//mac����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_MacConnect(const char* pcMacAddr,int nConnectCode = 1016,int nPairTimeoutMS = 5000,int nSleepMSAfterPair = 0,int nCon1TimeoutMS = 12000,int nCon2TimeoutMS = 10000);
//�Զ�����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_AutoConnect(int nTimeOutMS,int nInquiryTimeS = 5,int nConnectCode = 1016,int nPairTimeoutMS = 5000,int nSleepMSAfterPair = 0,int nCon1TimeoutMS = 12000,int nCon2TimeoutMS = 10000);
//�л���a2dp
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_A2dpStreaming(int nTimeOutMS = 3000);
//�л���HFP
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_HfpStreaming(int nHFPStateIndex,int nTimeOutMS = 5000,BOOL bCmd = FALSE); 

//����hfp����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetHFPVolume(int nVol);
//����a2dp����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SetA2DPVolume(int nVol);
//��ȡhfp����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetHFPVol(int& nValue);
//��ȡa2dp����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetA2dpVol(int& nValue);
//��ѯa2dp����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_QuiryA2dpVol(int& nValue,int nTimeoutMS = 1000);
//��ȡ����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetBattery(int& nValue);
//��ѯ����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_QuiryBattery(int& nValue,int nTimeoutMS = 5000);
//��ȡ����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetName(char* pcName);
//��ѯ����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_QuiryName(char* pcName,int nTimeoutMS = 2000);
//����mac��ַ��ѯ����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_QuiryNameWithMacAddr(const char* pcMacAddr,char* pcName,int nTimeoutMS = 2000);
//��ȡrssi
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetRssi(int& nValue);
//��ѯrssi
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_QuiryRssi(int& nValue,int nTimeoutMS = 2000);
//��ȡ����Mac��ַ
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetConnectMacAddr(char* pcMacAddr);
//��ȡ��ǰģʽ 0-unknown 1-a2dp 2-dial 3-ring
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_GetCurrentMode(int& nMode);//0 = UnKnow, 1=A2DP, 2 = HFP


EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_ATCommand(const char* pcATSend,char* pcATRecv,int nTimeoutMS);

//spp����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_ConnectSPP(const char* pcMacAddr,int nCon1TimeoutMS = 5000);
//�Ͽ�spp����
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_DisConnectSPP();
EXTERN_C CRY_BT_ERRORCODE CRY574PRO_API CRYBT_SPPCommand(const char* pcSPPSend,char* pcSPPRecv,int nTimeoutMS,BOOL bHex);


