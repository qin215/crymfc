#ifndef _GPIB_H_
#define _GPIB_H_

#ifdef WIN32
#include <tchar.h>
#include "windows.h"
#include <BaseTsd.h>
#else
#define TCHAR char
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	#define CHANNEL_ONE		1
	#define CHANNEL_TWO		2

	BOOL GPIBOpenDevice();
	void SetupChannelVoltage(double voltage, int channel);
	void EnableChannelVoltage(BOOL on, int channel);
	void GPIBCloseDevice();

#ifdef __cplusplus
}
#endif
#endif