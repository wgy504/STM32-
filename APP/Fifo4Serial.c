#include "Fifo4serial.h" 

/**********************************************
函数功能：创建队列
输入：无
输出：无
作者：小鹰fighting
创建日期：2012年3月25日
最后修改时间：2012年3月25日
备注：声明结构体
**********************************************/
Fifo4Serial USART1Send,USART1Recieve ;
Fifo4Serial USART2Send,USART2Recieve ;
Fifo4Serial USART3Send,USART3Recieve ;

/**********************************************
函数功能：队列初始化
输入：无
输出：无
作者：小鹰fighting
创建日期：2012年3月25日
最后修改时间：2012年3月25日
备注：无
**********************************************/
void QueueInit(Fifo4Serial *Q)
{
	Q->front = 0 ;
	Q->rear = 0 ;
	Q->bufferCount = 0 ;
}
/**********************************************
函数功能：入列
输入：无
输出：无
作者：小鹰fighting
创建日期：2012年3月25日
最后修改时间：2012年3月25日
备注：采用循环队列
**********************************************/
char QueueIn(Fifo4Serial *Q,char dat)
{
	if(((Q->rear) % QUEUE_BUFFER == Q->front) && (Q->bufferCount == QUEUE_BUFFER ))
		return QUEUE_FULL ;
	Q->base[Q->rear] = dat ;
	Q->rear = (Q->rear + 1) % QUEUE_BUFFER ;
	Q->bufferCount++ ;
	return(QUEUE_OK) ;
}
/**********************************************
函数功能：出列
输入：无
输出：无
作者：小鹰fighting
创建日期：2012年3月25日
最后修改时间：2012年3月25日
备注：无
**********************************************/
char QueueOut(Fifo4Serial *Q,char *dat)
{
 	if((Q->front == Q->rear) && (Q->bufferCount == 0)) 
		return(QUEUE_EMPTY) ;
	else {
		*dat = Q->base[Q->front] ;
		Q->front = (Q->front + 1) % QUEUE_BUFFER ;
		Q->bufferCount-- ;
		return(QUEUE_OK) ;
	}
}

