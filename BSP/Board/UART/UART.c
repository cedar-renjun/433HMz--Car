
#include "includes.h"

extern FIFO_S_t* PC_Tx;

void UART1_Init        (void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    
    /* Enable USART Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    /* Configure USARTy Rx as input floating */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure USARTy Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    
    /* Configure USARTy */
    USART_Init(USART1, &USART_InitStructure);
    
    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);  
        
    USART_Cmd(USART1, ENABLE);
    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

void UART1_PrintCh     (uint8_t ch)
{
#if 0
    OS_ERR         err;
    CPU_TS         ts;
    
    OSMutexPend((OS_MUTEX   *)&PC_UART_DEVICE,
            (OS_TICK     )0,
            (OS_OPT      )OS_OPT_PEND_BLOCKING,
            (CPU_TS     *)&ts,
            (OS_ERR     *)&err);
    
    /* Loop until USARTy DR register is empty */ 
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

    /* Send one byte from USARTy to USARTz */
    USART_SendData(USART1, (uint8_t)ch);
    
    OSMutexPost((OS_MUTEX   *)&PC_UART_DEVICE,
                (OS_OPT      )OS_OPT_POST_NONE,
                (OS_ERR     *)&err);
#else
    /* Loop until USARTy DR register is empty */ 
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

    /* Send one byte from USARTy to USARTz */
    USART_SendData(USART1, (uint8_t)ch);
#endif
}

void UART1_PrintStr    (uint8_t* str)
{
#if 1
    uint8_t i = 0;

    while(str[i] != '\0')
    {
        UART1_PrintCh(str[i++]);
    }
    
#else

    uint8_t i = 0;
    OS_ERR         err;
    CPU_TS         ts;

    OSMutexPend((OS_MUTEX   *)&PC_UART_DEVICE,
            (OS_TICK     )0,
            (OS_OPT      )OS_OPT_PEND_BLOCKING,
            (CPU_TS     *)&ts,
            (OS_ERR     *)&err);
#if 0    
    while(str[i] != '\0')
    {
        while(FIFO_S_IsFull(PC_Tx));
        FIFO_S_Put(PC_Tx, str[i]);
    }
#else
    while(str[i] != '\0')
    {
        UART1_PrintCh(str[i++]);
    }
#endif    
    
    OSMutexPost((OS_MUTEX   *)&PC_UART_DEVICE,
                (OS_OPT      )OS_OPT_POST_NONE,
                (OS_ERR     *)&err);
    
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
#endif
}

void UART1_PrintBlock  (uint8_t* pBuf, uint8_t size)
{
#if 1
    uint8_t i = 0;
    
    for(i = 0; i < size; i++)
    {
        UART1_PrintCh(pBuf[i]);
    }
#else
    uint8_t i = 0;
    OS_ERR         err;
    CPU_TS         ts;

    OSMutexPend((OS_MUTEX   *)&PC_UART_DEVICE,
                (OS_TICK     )0,
                (OS_OPT      )OS_OPT_PEND_BLOCKING,
                (CPU_TS     *)&ts,
                (OS_ERR     *)&err);
    
    for(i = 0; i < size; i++)
    {
#if 0
        while(FIFO_S_IsFull(PC_Tx));
        FIFO_S_Put(PC_Tx, pBuf[i]);
#else
        UART1_PrintCh(pBuf[i]);
#endif
    }
    
    OSMutexPost((OS_MUTEX   *)&PC_UART_DEVICE,
                (OS_OPT      )OS_OPT_POST_NONE,
                (OS_ERR     *)&err);
    
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
#endif
}

void UART2_Init        (void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    
    /* Enable USART Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    /* Configure USARTy Rx as input floating */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    /* Configure USARTy Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure USARTy */
    USART_Init(USART2, &USART_InitStructure);
    
    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);  
        
    USART_Cmd(USART2, ENABLE);
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void UART2_PrintCh     (uint8_t ch)
{
    /* Loop until USARTy DR register is empty */
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);

    /* Send one byte from USARTy to USARTz */
    USART_SendData(USART2, (uint8_t)ch);
}

void UART2_PrintStr    (uint8_t* str)
{
    uint8_t i = 0;

    while(str[i] != '\0')
    {
        UART2_PrintCh(str[i++]);
    }
}

void UART2_PrintBlock  (uint8_t* pBuf, uint8_t size)
{
    uint8_t i = 0;
    
    for(i = 0; i < size; i++)
    {
        UART2_PrintCh(pBuf[i]);
    }
}


