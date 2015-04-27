#include "..\App\includes.h"
#include "..\App\Globals.h"

extern u16 USART3_RX_STA;							//接收数据状态

/*
*********************************************************************************************************


*/

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

////////////////////////////////////////////////////////
void RCC_Configuration(void)
{	
  SystemInit(); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			   //复用功能使能
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE); 			//使能串口2时钟 
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE); 			//使能串口3时钟 
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 , ENABLE);	 		//使能串口1时钟
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//配置系统钟函数RCC_Configuration()使能TIM2钟：
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//配置系统钟函数RCC_Configuration()使能TIM2钟：
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//配置系统钟函数RCC_Configuration()使能TIM4钟：
}

//关闭调试接口，作GPIO使用
void UnableJTAG(void)
{
   
   RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
   AFIO->MAPR &= ~(7UL<<24); // clear used bit
   AFIO->MAPR |= (4UL<<24); // set used bits
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : PB5: LED1 (mini and V3)
                   PD6：LED2 (only V3)
				   PD3：LED3 (only V3)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				                 //LED1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_3;		 			//LED2, LED3
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  //LCD SPI配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

  //外设开关引脚配置
  //  |GPRS_SW  PA8|SK_SW  PE2|SMM_SW PE3|QD_SW  PC7|QDDY_SW PC6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				                 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //|SK_SW|SMM_SW|KB_H1|KB_H2|KB_H3|KB_H4|					 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;				                 
  GPIO_Init(GPIOE, &GPIO_InitStructure);					 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;				                 
  GPIO_Init(GPIOC, &GPIO_InitStructure);					 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	//SPEAKER			                 
  GPIO_Init(GPIOD, &GPIO_InitStructure);		

  //键盘输入V1-V4配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;				                 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  //键盘输入A1-A4配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;				                 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

// PCA9555的I2C引脚
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);     									 
}


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  #if defined (VECT_TAB_RAM)
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#elif defined(VECT_TAB_FLASH_IAP)
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif 

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			    //设置串口1中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     	//抢占优先级 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				//子优先级为0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			     	//设置串口2中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     	//抢占优先级 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//子优先级为1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能
  NVIC_Init(&NVIC_InitStructure);
 
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			     	//设置串口3中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     	//抢占优先级 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;				//子优先级为2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

void BSP_Init(void)
{
  /* System Clocks Configuration --72M*/
  RCC_Configuration();   

  GPIO_Configuration();
  
  /* NVIC configuration */
  /*嵌套向量中断控制器 */ 
  NVIC_Configuration();
  
  USART1_Config(9600);     	   //初始化串口1     
  USART2_Config(9600);            //初始化串口2    
  USART3_Config(9600);            //初始化串口3    

  tp_Config();					   //SPI1 触摸电路初始化	    
  
  TIM2_Configeration();	//100ms中断
  TIM3_Configeration();
  TIM4_Configeration();
}

/****************************************************************************
* 名    称：void tp_Config(void)
* 功    能：
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void tp_Config(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure; 
  SPI_InitTypeDef   SPI_InitStructure; 

  /* SPI1 时钟使能 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE); 
 
  /* SPI1 SCK(PA5)、MISO(PA6)、MOSI(PA7) 设置 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//口线速度50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	        //复用模式
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* 由于SPI1总线上挂接了4个外设，所以在使用触摸屏时，需要禁止其余3个SPI1 外设， 才能正常工作 */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           		//SPI1 SST25VF016B片选 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//口线速度50MHZ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//推挽输出模式
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);							//SPI1 SST25VF016B片选置高 
  
   /* SPI1总线 配置 */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //全双工  
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   //主模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   //8位
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;						   //时钟极性 空闲状态时，SCK保持低电平
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //时钟相位 数据采样从第一个时钟边沿开始
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //软件产生NSS
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;  //波特率控制 SYSCLK/128
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //数据高位在前
  SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC多项式寄存器初始值为7 
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* SPI1 使能 */  
  SPI_Cmd(SPI1,ENABLE);  
}


/****************************************************************************
* 名    称：void USART1_Config(u32 baud)
* 功    能：串口1配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART1_Config(u32 baud){
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 		//USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 		//A端口 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//复用开漏输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 		//A端口

  USART_InitStructure.USART_BaudRate = baud;						//速率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
  USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
  
  /*配置USART1 */
  USART_Init(USART1,&USART_InitStructure);							//配置串口参数函数   
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                    //使能接收中断
  //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);						//使能发送缓冲空中断   
   /* 使能USART1 */
  //USART_Cmd(USART1, ENABLE);	
}

/****************************************************************************
* 名    称：void USART2_Config(u32 baud)
* 功    能：串口2配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART2_Config(u32 baud){
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         		 	    //USART2 TX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		        //复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		        //A端口 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	         	 	        //USART2 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	        //复用开漏输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);	

  USART_InitStructure.USART_BaudRate = baud;						//速率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
  USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式

  /* Configure USART2 */
  USART_Init(USART2, &USART_InitStructure);							//配置串口参数函数   

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                    //使能接收中断
   /* 使能USART2 */
  USART_Cmd(USART2, ENABLE);	
}
/****************************************************************************
* 名    称：void USART3_Config(u32 baud)
* 功    能：串口3配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART3_Config(u32 baud){
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         		 		//USART3 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);		    		 		//B端口 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	         	 		//USART3 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//复用开漏输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);		         	 		//A端口 

  USART_InitStructure.USART_BaudRate = baud;						//速率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
  USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
  /*配置USART3 */
  USART_Init(USART3,&USART_InitStructure);							//配置串口参数函数   
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                    //使能接收中断
  //USART_ITConfig(USART3, USART_IT_TXE, ENABLE);						//使能发送缓冲空中断   
   /* 使能USART1 */
  USART_Cmd(USART3, ENABLE);	
}


//每100ms产生定时器中断
//定时频率=72 000 000/(7200-1)/10000
//8MHz晶振：定时频率=72 000 000 / (TIM_TimeBaseStructure.TIM_Prescaler + 1 ) / (TIM_TimeBaseStructure.TIM_Period)
void TIM2_Configeration()
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
  TIM_DeInit(TIM2);//复位TIM2定器
  TIM_TimeBaseStructure.TIM_Period = 1000;   //设置自动装载寄存器       
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;   //分频计数    
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //选择向上计数
  //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM2, ENABLE);
  TIM_ClearITPendingBit(TIM2, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4|TIM_IT_Update); 
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, DISABLE);   //是能定时器
}
void TIM3_Configeration()
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
  TIM_DeInit(TIM3);//复位TIM2定器
  TIM_TimeBaseStructure.TIM_Period = 1000;   //设置自动装载寄存器       
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;   //分频计数    
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //选择向上计数
  //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  TIM_ClearITPendingBit(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4|TIM_IT_Update); 
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM3, DISABLE);   //是能定时器
}
void TIM4_Configeration()
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
  TIM_DeInit(TIM4);//复位TIM2定器
  TIM_TimeBaseStructure.TIM_Period = 1000;   //设置自动装载寄存器       
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;   //分频计数    
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //选择向上计数
  //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM4, ENABLE);
  TIM_ClearITPendingBit(TIM4, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4|TIM_IT_Update); 
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, DISABLE);   //是能定时器
}




////SYSTICK 配置， 10ms一次systick中断，产生ucos 调度节拍， 1KHz
void  OS_CPU_SysTickInit(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;


    RCC_GetClocksFreq(&rcc_clocks);

    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;

	SysTick_Config(cnts);

  
}


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

