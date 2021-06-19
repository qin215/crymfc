#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#define uint8_t unsigned char
#define uint16_t unsigned short


//#define BOOL unsigned char

//#define TRUE 1
//#define FALSE 0

UINT32 parse_race_cmd_rsp(const uint8_t *pdata, int data_len);

#ifdef __cplusplus
}
#endif
#endif