#include "can.h"

#ifdef USE_CAN_REC_Q
//如果使用 CAN接收队列

#define CAN_RX_BUFFER_SIZE 20
static CanRxMsg g_CAN_Rx_message[CAN_RX_BUFFER_SIZE];
static volatile unsigned char g_CAN_Rx_Provider;//can buffer 生产者
static volatile unsigned char g_CAN_Rx_Consumer;//can buffer 消费者

void CAN_RX0_CALLBACK(void);

#endif

void BSP_CAN_Init(void)
{
	CAN_InitTypeDef        can;
	CAN_FilterInitTypeDef  can_filter;		
	GPIO_InitTypeDef 	   gpio;
	NVIC_InitTypeDef   	   nvic;

    //can rcc config
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,  ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_AFIO , ENABLE);	

	//can gpio config
	gpio.GPIO_Pin = GPIO_Pin_11;
	gpio.GPIO_Mode = GPIO_Mode_IPU;       
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);	//CAN_RX 								 
	
	gpio.GPIO_Pin = GPIO_Pin_12;	   
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);    //CAN_TX
	
	//can nvic config	
	nvic.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;  
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	nvic.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;  
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

    //can mode config
	CAN_DeInit(CAN1);

	can.CAN_TTCM = DISABLE;
	can.CAN_ABOM = DISABLE;
	can.CAN_AWUM = DISABLE;
	can.CAN_NART = DISABLE;	//使能自动重传  
	can.CAN_RFLM = DISABLE;																
	can.CAN_TXFP = ENABLE;		
 	can.CAN_Mode = CAN_Mode_Normal;
	can.CAN_SJW = CAN_SJW_1tq;
	can.CAN_BS1 = CAN_BS1_5tq;
	can.CAN_BS2 = CAN_BS2_3tq;
	can.CAN_Prescaler = 4;     //CAN BaudRate 36/(1+5+3)/4=1Mbps
	CAN_Init(CAN1, &can);
	
    //can filter config
    can_filter.CAN_FilterNumber = 0;
	can_filter.CAN_FilterMode = CAN_FilterMode_IdList; //标识符列表模式
	can_filter.CAN_FilterScale = CAN_FilterScale_32bit;//两个16位过滤器
	can_filter.CAN_FilterIdHigh = (MODULE_ID)<<5;
	can_filter.CAN_FilterIdLow = 0;
    can_filter.CAN_FilterMaskIdHigh = 0;
    can_filter.CAN_FilterMaskIdLow = 0;
    can_filter.CAN_FilterFIFOAssignment = 0;
	can_filter.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&can_filter);


	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	//NOTE CAN发送中断
	CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);    

#ifdef USE_CAN_REC_Q
    g_CAN_Rx_Provider = 0;
    g_CAN_Rx_Consumer = 0;
#endif
}


void USB_HP_CAN1_TX_IRQHandler(void) //CAN TX
{//发送成功进入中断
    if (CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET) 
    {
	   CAN_ClearITPendingBit(CAN1,CAN_IT_TME);     
    }
}


#ifdef USE_CAN_REC_Q
/*******************************************************************************
* Function Name  : read_CAN_buffer
* Description    : 读取CAN缓冲区内的数据
* Input          : None
* Output         : None
* Return         : 当前有用数据的指针，如果队列里面没有数据则返回0
*******************************************************************************/
CanRxMsg* read_CAN_buffer(void)
{
    if(g_CAN_Rx_Provider != g_CAN_Rx_Consumer)
    {
        g_CAN_Rx_Consumer ++;
        if(g_CAN_Rx_Consumer >= CAN_RX_BUFFER_SIZE)
            g_CAN_Rx_Consumer = 0;  
        return &g_CAN_Rx_message[g_CAN_Rx_Consumer];
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
* Function Name  : CAN_RX0_CALLBACK
* Description    : CAN0接收的回调函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_RX0_CALLBACK(void)
{
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
	{
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
        g_CAN_Rx_Provider++;
        if(g_CAN_Rx_Provider >= CAN_RX_BUFFER_SIZE)
            g_CAN_Rx_Provider = 0;
        CAN_Receive(CAN1, CAN_FIFO0, &g_CAN_Rx_message[g_CAN_Rx_Provider]);
    }
}
#endif

