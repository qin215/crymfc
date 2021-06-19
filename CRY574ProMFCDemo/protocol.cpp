#include "stdafx.h"
#include <stddef.h>
#include <stdio.h>
#include <windows.h>
#include "protocol.h"

#pragma pack(push)
#pragma pack(1)
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


UINT32 parse_race_cmd_rsp(const uint8_t *pdata, int data_len)
{
	onewire_frame_t *pFrame = (onewire_frame_t *)pdata;
	BOOL valid = FALSE;
	// 05 5A 05 00 06 0E 00 0B 03

	uint16_t len = pFrame->len;
	len += 4;			// added length & header & type
	if (len <= data_len)
	{
		valid = TRUE;
	}

	if (!valid)
	{
		printf("qin spp format error!");
		return 0;
	}

	return pFrame->param[0];
}