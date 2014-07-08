#ifndef __LED_H_
#define __LED_H_

#ifdef  __cpluscplus
extern "C"
{
#endif

#include "stm32f10x.h"

extern void LED_Init   (void);
extern void LED_AllOn  (void);
extern void LED_AllOff (void);
extern void LED_On     (uint32_t id);
extern void LED_Off    (uint32_t id);
extern void LED_Triggle(uint32_t id);

#ifdef  __cpluscplus
}
#endif

#endif  // __LED_H__
