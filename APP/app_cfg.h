/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-LK1 Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.10
* Programmer(s) : BH3NVN
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                               2

#define Task_main_PRIO    5
#define Task_calendar_PRIO    3
//#define Task_screen_PRIO    4
#define Task_savedata_PRIO    29
#define Task_guizi_PRIO   20      // 柜子状态刷新任务优先级
#define Task_keyboard_PRIO   12// 键盘扫描任务
#define Task_icread_PRIO   10// IC读卡任务
#define Task_gprs_PRIO   11// IC读卡任务
#define Task_barcode_PRIO   9// 条码读任务


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                         128

#define Task_main_STK_SIZE     512
#define Task_calendar_STK_SIZE     128
//#define Task_Screen_STK_SIZE     512
#define Task_savedata_STK_SIZE     128
#define Task_guizi_STK_SIZE     128
#define Task_keyboard_STK_SIZE     128
#define Task_icread_STK_SIZE     128
#define Task_gprs_STK_SIZE     512
#define Task_barcode_STK_SIZE     256

/*
*********************************************************************************************************
*                                                  LIB
*********************************************************************************************************
*/

#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED

#endif
