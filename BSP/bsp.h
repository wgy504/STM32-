#if !defined( _BSP_H )
#define _BSP_H

void RCC_Configuration(void);

void GPIO_Configuration(void);

void NVIC_Configuration(void);

CPU_INT32U  BSP_CPU_ClkFreq (void);

//INT32U  OS_CPU_SysTickClkFreq (void);

void  OS_CPU_SysTickInit(void);

void tp_Config(void) ;

void BSP_Init(void);
void USART1_Config(u32 baud);
void USART2_Config(u32 baud);
void USART3_Config(u32 baud);
void TIM2_Configeration();
void TIM4_Configeration();
void TIM3_Configeration();


#endif
