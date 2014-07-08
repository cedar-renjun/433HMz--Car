#ifndef __CAN_H__
#define __CAN_H__

#include "stm32f10x.h"

#define MODULE_ID 0X40
#define USE_CAN_REC_Q

/* Exported functions --------------------------------------------------------*/
extern void BSP_CAN_Init(void);

#ifdef USE_CAN_REC_Q
    CanRxMsg* read_CAN_buffer(void);
#endif

#endif 


