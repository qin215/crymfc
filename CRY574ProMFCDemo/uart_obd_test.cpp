#include "stdafx.h"

#include "ServerCmd.h"
#include "data_buff.h"
#include "uart_cmd.h"
#include "mywin.h"
#include "qcloud_iot_export_log.h"

//#ifdef __OBD_TEST__

static int total_tx;
static int total_rx;
static int uart_total_rx;

#define MAX_UART_LOG_DATA_SIZE	(5 * 1024 * 1024)

static int uart_total_rx_array[MAX_PORT_NUM];

#define UART_LOG_FILE	"uart_tx"
#define NET_LOG_FILE	"net_rx"
#define UART_RX_FILE	"uart_rx"

#define LOG_DIR			"log\\"


void obd_test_send_uart()
{
	static Boolean cs_inited = FALSE;
	static CRITICAL_SECTION cs; 
	
	buf_t *b;
	
	if (!cs_inited)
	{
		InitializeCriticalSection(&cs);
		cs_inited = TRUE;
	}

	EnterCriticalSection(&cs);

	b = get_buf(&uart_tx_queue);
	if (!b)
	{
		printf("null buff\n");
		LeaveCriticalSection(&cs);
		return;
	}

	uart_send_buf(b, NULL);
	remove_buf(&uart_tx_queue);
	free_buffer(b);
	
	LeaveCriticalSection(&cs);

	printf("total send = %d\r\n", total_tx);
}

static buf_t * obd_build_random_pack(buf_t *b)
{
	int i;
	int num;
	char *p = b->pbuff;
	static char ch_array[] = "abcdefghijklmnopqrstuvwxyzABCDEFHIJKLMNOPQRSTUVWXYZ0123456789";
	int str_len = strlen(ch_array);

	srand((unsigned int)time(NULL));
	num = rand() % 3000;

	if (num <= 2)
	{
		num = 2;
	}
	
	for (i = 0; i < num - 2; i++)
	{
		int index;

		index = rand() % str_len;
		*p++ = ch_array[index];
	}

	*p++ = '\r';
	*p++= '\n';

	b->len = num;
	
	return b;
}
