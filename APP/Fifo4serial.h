#ifndef _FIFO4SERIAL_H_
#define _FIFO4SERIAL_H_


#define QUEUE_EMPTY 0
#define QUEUE_FULL 1
#define QUEUE_OK 2
#define QUEUE_BUFFER 128

typedef struct {
	char base[QUEUE_BUFFER] ;
	unsigned int bufferCount ;
	unsigned int front ;
	unsigned int rear ;
} Fifo4Serial ;
extern Fifo4Serial USART1Send,USART1Recieve ;
extern Fifo4Serial USART2Send,USART2Recieve ;
extern Fifo4Serial USART3Send,USART3Recieve ;
extern void QueueInit(Fifo4Serial *Q) ;
extern char QueueIn(Fifo4Serial *Q,char dat) ;
extern char QueueOut(Fifo4Serial *Q,char *dat) ;

#endif
