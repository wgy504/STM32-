#include "includes.h"

#define I2C_Speed              100000 
#define I2C1_SLAVE_ADDRESS7    0xA0 
#define I2C_PageSize           256 

#define SCL_H         GPIOB->BSRR = GPIO_Pin_8 
#define SCL_L         GPIOB->BRR  = GPIO_Pin_8  
    
#define SDA_H         GPIOB->BSRR = GPIO_Pin_9 
#define SDA_L         GPIOB->BRR  = GPIO_Pin_9 

#define SCL_read      GPIOB->IDR  & GPIO_Pin_8 
#define SDA_read      GPIOB->IDR  & GPIO_Pin_9 

u8 ack;
/* Private macro -------------------------------------------------------------*/ 
/* Private variables ---------------------------------------------------------*/ 
vu8 FRAM_ADDRESS; 

/* Private function prototypes -----------------------------------------------*/ 

/**/ 
void I2C_delay(void) 
{ 
   u8 i=150; //这里可以优化速度 ，经测试最低到5还能写入 
   while(i)  
   {  
     i--;  
   }  
} 

bool I2C_Start(void) 
{ 
SDA_H; 
SCL_H; 
I2C_delay(); 
if(!SDA_read)return FALSE; //SDA线为低电平则总线忙,退出 
SDA_L; 
I2C_delay(); 
if(SDA_read) return FALSE; //SDA线为高电平则总线出错,退出 
SDA_L; 
I2C_delay(); 
return TRUE; 
} 

void I2C_Stop(void) 
{ 
SCL_L; 
I2C_delay(); 
SDA_L; 
I2C_delay(); 
SCL_H; 
I2C_delay(); 
SDA_H; 
I2C_delay(); 
} 

void I2C_Ack(void) 
{ 
SCL_L; 
I2C_delay(); 
SDA_L; 
I2C_delay(); 
SCL_H; 
I2C_delay(); 
SCL_L; 
I2C_delay(); 
} 

void I2C_NoAck(void) 
{ 
SCL_L; 
I2C_delay(); 
SDA_H; 
I2C_delay(); 
SCL_H; 
I2C_delay(); 
SCL_L; 
I2C_delay(); 
} 

bool I2C_WaitAck(void)   //返回为:=1有ACK,=0无ACK 
{ 
SCL_L; 
I2C_delay(); 
SDA_H; 
I2C_delay(); 
SCL_H; 
I2C_delay(); 
if(SDA_read) 
{ 
      SCL_L; 
      return 0; 
} 
SCL_L; 
return 1; 
} 

void I2C_SendByte(u8 SendByte) //数据从高位到低位// 
{ 
    u8 i=8; 
    while(i--) 
    { 
        SCL_L; 
        I2C_delay(); 
      if(SendByte&0x80) 
        SDA_H;   
      else  
        SDA_L;    
        SendByte<<=1; 
        I2C_delay(); 
SCL_H; 
        I2C_delay(); 
    } 
    SCL_L; 
} 

u8 I2C_ReceiveByte(void)  //数据从高位到低位// 
{  
    u8 i=8; 
    u8 ReceiveByte=0; 

    SDA_H; 
    while(i--) 
    { 
      ReceiveByte<<=1;       
      SCL_L; 
      I2C_delay(); 
  SCL_H; 
      I2C_delay(); 
      if(SDA_read) 
      { 
        ReceiveByte|=0x01; 
      } 
    } 
    SCL_L; 
    return ReceiveByte; 
} 

bool I2C_FRAM_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite) 
{ 
u8 Addr = 0, count = 0; 

Addr = WriteAddr / I2C_PageSize; 

count = WriteAddr % I2C_PageSize; 

Addr = Addr << 1; 

Addr = Addr & 0x0F;   

FRAM_ADDRESS = I2C1_SLAVE_ADDRESS7 | Addr; 

    if (!I2C_Start()) return FALSE; 
    I2C_SendByte(FRAM_ADDRESS);//设置器件地址+段地址  
    if (!I2C_WaitAck()) 
{ 
I2C_Stop();  
return FALSE; 
} 
    I2C_SendByte(count);   //设置段内地址       
I2C_WaitAck(); 
   
while(NumByteToWrite--) 
{ 
  I2C_SendByte(* pBuffer); 
  I2C_WaitAck(); 
      pBuffer++; 
} 
I2C_Stop(); 
  //注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms) 
  //Systick_Delay_1ms(10); 
return TRUE; 
} 


//读出1串数据          
bool I2C_FRAM_BufferRead(u8* pBuffer, u16 WriteAddr, u16 NumByteToRead) 
{ 
u8 Addr = 0, count = 0; 

    Addr = WriteAddr / I2C_PageSize; 

count = WriteAddr % I2C_PageSize; 

Addr = Addr << 1; 

Addr = Addr & 0x0F;   

FRAM_ADDRESS = I2C1_SLAVE_ADDRESS7 | Addr; 

if (!I2C_Start()) return FALSE; 

    I2C_SendByte(FRAM_ADDRESS);//设置器件地址+段地址  

    if (!I2C_WaitAck())  
{ 
I2C_Stop();  
return FALSE; 
} 

    I2C_SendByte(count);   //设置低起始地址       
    I2C_WaitAck(); 
    I2C_Start(); 
    I2C_SendByte(FRAM_ADDRESS | 0x01); 
    I2C_WaitAck(); 
    while(NumByteToRead) 
    { 
      *pBuffer = I2C_ReceiveByte(); 
      if(NumByteToRead == 1)I2C_NoAck(); 
      else I2C_Ack();  
      pBuffer++; 
      NumByteToRead--; 
    } 
    I2C_Stop(); 
    return TRUE; 
} 




/*******************************************************************
                     起动总线函数               
函数原型: void  Start_I2c();  
功能:       启动I2C总线,即发送I2C起始条件.
  
********************************************************************/
void Start_I2c()
{
	SDA_H;   /*发送起始条件的数据信号*/
	delay_us(1);

	SCL_H;
	delay_us(1);    /*起始条件建立时间大于4.7us,延时*/
	delay_us(1);
	delay_us(1);
	delay_us(1);
	delay_us(1); 
	   
	SDA_L;   /*发送起始信号*/
	delay_us(1);    /* 起始条件锁定时间大于4μs*/
	delay_us(1);
	delay_us(1);
	delay_us(1);
	delay_us(1); 
	      
	SCL_L;   /*钳住I2C总线，准备发送或接收数据 */
	delay_us(1);
	delay_us(1);
}				  

/*******************************************************************
                      结束总线函数               
函数原型: void  Stop_I2c();  
功能:       结束I2C总线,即发送I2C结束条件.
  
********************************************************************/
void Stop_I2c()
{
	SDA_L;  /*发送结束条件的数据信号*/
	delay_us(1);   /*发送结束条件的时钟信号*/

	SCL_H;  /*结束条件建立时间大于4μs*/
	delay_us(1);
	delay_us(1);
	delay_us(1);
	delay_us(1);
	delay_us(1);

	SDA_H;  /*发送I2C总线结束信号*/
	delay_us(1);
	delay_us(1);
	delay_us(1);
	delay_us(1);
}							 

/*******************************************************************
                 字节数据传送函数               
函数原型: void  SendByte(u8 c);
功能:  将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
     此状态位进行操作.(不应答或非应答都使ack=0 假)     
     发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
********************************************************************/
void  SendByte(u8 c)
{
	u8 BitCnt;
	
	for(BitCnt=0;BitCnt<8;BitCnt++)  /*要传送的数据长度为8位*/
	{
		if((c<<BitCnt)&0x80)
			SDA_H;   /*判断发送位*/
		else  
			SDA_L;                
		delay_us(1);

		SCL_H;               /*置时钟线为高，通知被控器开始接收数据位*/
		delay_us(1); 
		delay_us(1);               /*保证时钟高电平周期大于4μs*/
		delay_us(1);
		delay_us(1);
		delay_us(1);         
		SCL_L; 
	}
	
	delay_us(1);
	delay_us(1);
	SDA_H;               /*8位发送完后释放数据线，准备接收应答位*/
	delay_us(1);
	delay_us(1);   
	SCL_H;
	delay_us(1);
	delay_us(1);
	delay_us(1);
	if(SDA_read)
		ack=0;     
	else 
		ack=1;        /*判断是否接收到应答信号*/
	SCL_L;
	delay_us(1);
	delay_us(1);
}					

/*******************************************************************
                 字节数据传送函数               
函数原型: u8  RcvByte();
功能:  用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
     发完后请用应答函数。  
********************************************************************/	
u8  RcvByte()
{
	u8 retc;
	u8 BitCnt;
	
	retc=0; 
	SDA_H;             /*置数据线为输入方式*/
	for(BitCnt=0;BitCnt<8;BitCnt++)
	{
		delay_us(1);           
		SCL_L;       /*置时钟线为低，准备接收数据位*/
		delay_us(1);
		delay_us(1);         /*时钟低电平周期大于4.7μs*/
		delay_us(1);
		delay_us(1);
		delay_us(1);
		SCL_H;       /*置时钟线为高使数据线上数据有效*/
		delay_us(1);
		delay_us(1);
		retc=retc<<1;
		if(SDA_read)
			retc=retc+1; /*读数据位,接收的数据位放入retc中 */
		delay_us(1);
		delay_us(1); 
	}
	SCL_L;    
	delay_us(1);
	delay_us(1);
	return(retc);
}				   

/********************************************************************
                     应答子函数
原型:  void Ack_I2c(bit a);
 
功能:主控器进行应答信号,(可以是应答或非应答信号)
********************************************************************/
void Ack_I2c(u8 a)
{
	if(a==0)
		SDA_L;     /*在此发出应答或非应答信号 */
	else 
		SDA_H;
	delay_us(1);
	delay_us(1);
	delay_us(1);      
	SCL_H;
	delay_us(1);
	delay_us(1);              /*时钟低电平周期大于4μs*/
	delay_us(1);
	delay_us(1);
	delay_us(1);  
	SCL_L;                /*清时钟线，钳住I2C总线以便继续接收*/
	delay_us(1);
	delay_us(1);    
}			  

/*******************************************************************
                    向无子地址器件发送字节数据函数               
函数原型: bit  ISendByte(u8 sla,ucahr c);  
功能:     从启动总线到发送地址，数据，结束总线的全过程,从器件地址sla.
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/
u8 ISendByte(u8 sla,u8 c)
{
	Start_I2c();               /*启动总线*/
	SendByte(sla);            /*发送器件地址*/
	if(ack==0)
		return(0);
	SendByte(c);               /*发送数据*/
	if(ack==0)
		return(0);
	Stop_I2c();                 /*结束总线*/ 
	return(1);
}				  

/*******************************************************************
                    向有子地址器件发送多字节数据函数               
函数原型: bit  ISendStr(u8 sla,u8 suba,ucahr *s,u8 no);  
功能:     从启动总线到发送地址，子地址,数据，结束总线的全过程,从器件
          地址sla，子地址suba，发送内容是s指向的内容，发送no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/
u8 ISendStr(u8 sla,u8 suba,u8 *s,u8 no)
{
	u8 i;
	
	Start_I2c();               /*启动总线*/

	SendByte(sla);            /*发送器件地址*/
	if(ack==0)
		return(0);

	SendByte(suba);            /*发送器件子地址*/
	if(ack==0)
		return(0);
	
	for(i=0;i<no;i++)
	{   
		SendByte(*s);               /*发送数据*/
		if(ack==0)return(0);
		s++;
	}
	 
	Stop_I2c();                 /*结束总线*/
	 
	return(1);
}					  

/*******************************************************************
                    向无子地址器件读字节数据函数               
函数原型: bit  IRcvByte(u8 sla,ucahr *c);  
功能:     从启动总线到发送地址，读数据，结束总线的全过程,从器件地
          址sla，返回值在c.
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/
u8 IRcvByte(u8 sla,u8 *c)
{
	Start_I2c();                /*启动总线*/

	SendByte(sla+1);           /*发送器件地址*/
	if(ack==0)
		return(0);

	*c=RcvByte();               /*读取数据*/
	Ack_I2c(1);               /*发送非就答位*/
	Stop_I2c();                  /*结束总线*/ 
	return(1);
}				   

/*******************************************************************
                    向有子地址器件读取多字节数据函数               
函数原型: bit  ISendStr(u8 sla,u8 suba,ucahr *s,u8 no);  
功能:     从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件
          地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/
u8 IRcvStr(u8 sla,u8 suba,u8 *s,u8 no)
{
	u8 i;
	
	Start_I2c();               /*启动总线*/

	SendByte(sla);            /*发送器件地址*/
	if(ack==0)
		return(0);

	SendByte(suba);            /*发送器件子地址*/
	if(ack==0)
		return(0);
	
	Start_I2c();
	SendByte(sla+1);
	if(ack==0)
		return(0);
	
	for(i=0;i<no-1;i++)
	{   
		*s=RcvByte();               /*发送数据*/
		Ack_I2c(0);                /*发送就答位*/  
		s++;
	} 
	*s=RcvByte();
	Ack_I2c(1);  			/*发送非应位*/
	Stop_I2c();             /*结束总线*/ 
	return(1);
}



