
#include "LED.h"

void LED_Init   (void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio.GPIO_Pin   = GPIO_Pin_8; 
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    LED_AllOff();
}

void LED_AllOn  (void)
{
    GPIOA->BRR = GPIO_Pin_8;
}

void LED_AllOff (void)
{
    GPIOA->BSRR = GPIO_Pin_8;
}

void LED_On(uint32_t id)
{
    switch(id)
    {
        case 0:
        {
            GPIOA->BRR = GPIO_Pin_8;
            break;
        }

        default:
        {
            break;
        }
    }
}

void LED_Off(uint32_t id)
{
    switch(id)
    {
        case 0:
        {
            GPIOA->BSRR = GPIO_Pin_8;
            break;
        }

        default:
        {
            break;
        }
    }
}

void LED_Triggle(uint32_t id)
{
    switch(id)
    {
        case 0:
        {
            GPIOA->ODR ^= GPIO_Pin_8;
            break;
        }

        default:
        {
            break;
        }
    }
}

