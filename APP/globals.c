#include "includes.h"
#include "globals.h"



u8 USART3_RX_BUF[USART3_MAX_RECV_LEN];		//接收缓冲,最大USART2_MAX_RECV_LEN字节
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN];		//发送缓冲,最大USART2_MAX_SEND_LEN字节
u16 USART3_RX_STA=0;							//接收数据状态


