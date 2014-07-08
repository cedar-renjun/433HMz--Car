#ifndef __BSP_UART_H_
#define __BSP_UART_H_

#ifdef  __cpluscplus
extern "C"
{
#endif

#include "stm32f10x.h"

#define PC_PrintCh(Ch)             UART1_PrintCh(Ch)
#define PC_PrintStr(Str)           UART1_PrintStr(Str)
#define PC_PrintBlock(pBuf, size)  UART1_PrintBlock(pBuf, size)

#define RC_PrintCh(Ch)             UART2_PrintCh(Ch)
#define RC_PrintStr(Str)           UART2_PrintStr(Str)
#define RC_PrintBlock(pBuf, size)  UART2_PrintBlock(pBuf, size)

extern void UART1_Init        (void);
extern void UART1_PrintCh     (uint8_t ch);
extern void UART1_PrintStr    (uint8_t* str);
extern void UART1_PrintBlock  (uint8_t* pBuf, uint8_t size);

extern void UART2_Init        (void);
extern void UART2_PrintCh     (uint8_t ch);
extern void UART2_PrintStr    (uint8_t* str);
extern void UART2_PrintBlock  (uint8_t* pBuf, uint8_t size);

#ifdef  __cpluscplus
}
#endif

#endif  // __BSP_UART_H__
