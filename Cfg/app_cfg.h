/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                              (c) Copyright 2009; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : JJL
*                 EHS
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

#include <includes.h>

#define  APP_TASK_START_PRIO                             2
#define  APP_TASK_START_STK_SIZE                         256

#define  APP_TASK_CAN_PRIO                               3
#define  APP_TASK_CAN_STK_SIZE                           256

#define  APP_TASK_RF_PRIO                                4
#define  APP_TASK_RF_STK_SIZE                            256
#define  RF_BUF_ELEM_CNT                                 20

#define CAN_BUF_ELEM_CNT                                 20
#define CAN_BUF_ELEM_SIZE                                sizeof(CanRxMsg)

#endif
