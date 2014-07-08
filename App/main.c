#include <includes.h>

// Start Task
static  OS_TCB   AppTaskStartTCB; 
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  void  AppTaskStart  (void *p_arg);

// Can Task
OS_TCB   AppTaskCanTCB;
static  CPU_STK  AppTaskCanStk[APP_TASK_CAN_STK_SIZE];
static  void  AppTaskCan  (void *p_arg);

// RF Task
OS_TCB   AppTaskRFTCB;
static  CPU_STK  AppTaskRFStk[APP_TASK_RF_STK_SIZE];
static  void  AppTaskRF  (void *p_arg);


// CAN Msg Queue
OS_MEM     CAN_Msg;
CPU_INT08U CAN_MsgBuf[CAN_BUF_ELEM_CNT][CAN_BUF_ELEM_SIZE];


// Intel Function
static  uint16_t CAN_CmdParser(void *  p_msg);
static  uint16_t RF_CmdParser (uint16_t cmd);

// Global Info
static CAR_INFO       g_CarInfo;
static CAR_STATUS     g_CarStatus;
static EQUIMENT_INFO  g_CarEquiment;
static ATTACKED_INFO  g_CarAttacked;

#pragma pack(1)
typedef struct
{
    uint8_t   len;
    uint8_t   addr;
    uint16_t  cmd_id;
    uint8_t   data[32];
}RF_Cmd;

// Global Data Lock
OS_MUTEX    GLOBAL_DATA_PROTECT;

int  main (void)
{
    OS_ERR  err;
    
    CPU_IntDis();                                               /* Disable all interrupts.                              */
    
    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR )AppTaskStart, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  )5,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}

//static void RF_CmdPacketSend(uint8_t );

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;

   (void)p_arg;

    BSP_Init();                                                   /* Initialize BSP functions                         */
    CPU_Init();                                                   /* Initialize the uC/CPU services                   */

    cpu_clk_freq = BSP_CPU_ClkFreq();                             /* Determine SysTick reference freq.                */                                                                        
    cnts         = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;  /* Determine nbr SysTick increments                 */
    OS_CPU_SysTickInit(cnts);                                     /* Init uC/OS periodic time src (SysTick).          */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                                 /* Compute CPU capacity with no task running        */
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    // CAN Msg Buffer
    OSMemCreate((OS_MEM      *) &CAN_Msg,
                (CPU_CHAR    *) "CAN Message Queue",
                (void        *) &CAN_MsgBuf[0][0],
                (OS_MEM_QTY   ) CAN_BUF_ELEM_CNT,
                (OS_MEM_SIZE  ) CAN_BUF_ELEM_SIZE,
                (OS_ERR      *) &err);
#ifdef DEBUG    
    if(OS_ERR_NONE != err)
    {
        while(1);
    }
#endif

    // Create Global Data Protect Lock    
    OSMutexCreate((OS_MUTEX  *) &GLOBAL_DATA_PROTECT,
                  (CPU_CHAR  *) "Global Data Mutex Lock",
                  (OS_ERR    *)&err);
#ifdef DEBUG    
    if(OS_ERR_NONE != err)
    {
        while(1);
    }
#endif

    // Create Can Task    
    OSTaskCreate((OS_TCB     *)&AppTaskCanTCB,
                 (CPU_CHAR   *)"App Task Can",
                 (OS_TASK_PTR )AppTaskCan, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_CAN_PRIO,
                 (CPU_STK    *)&AppTaskCanStk[0],
                 (CPU_STK_SIZE)APP_TASK_CAN_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_CAN_STK_SIZE,
                 (OS_MSG_QTY  )CAN_BUF_ELEM_CNT,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);    

    // Create RF Task    
    OSTaskCreate((OS_TCB     *)&AppTaskRFTCB,
                 (CPU_CHAR   *)"App Task RF",
                 (OS_TASK_PTR )AppTaskRF, 
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_RF_PRIO,
                 (CPU_STK    *)&AppTaskRFStk[0],
                 (CPU_STK_SIZE)APP_TASK_RF_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_RF_STK_SIZE,
                 (OS_MSG_QTY  )RF_BUF_ELEM_CNT,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
    
    while (DEF_TRUE)
    {
        LED_Triggle(0);
        
        OSTimeDlyHMSM( 0, 0, 0, 200,
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err); 
    }
}

static  void  AppTaskCan (void *p_arg)
{
    OS_ERR       err;
    CPU_TS       ts;
    void         *p_msg;
    OS_MSG_SIZE  msg_size;
    uint16_t     Cmd;
    
    // Clear Global Data Struct
    memset((void*)&g_CarInfo,     0, sizeof(CAR_INFO));
    memset((void*)&g_CarStatus,   0, sizeof(CAR_STATUS));
    memset((void*)&g_CarEquiment, 0, sizeof(EQUIMENT_INFO));
    memset((void*)&g_CarAttacked, 0, sizeof(ATTACKED_INFO));
    
    BSP_CAN_Init();
    
    while (DEF_TRUE)
    {
        //LED_Triggle(0);
        p_msg = OSTaskQPend((OS_TICK      )0,
                            (OS_OPT       )OS_OPT_PEND_BLOCKING,
                            (OS_MSG_SIZE *)&msg_size,
                            (CPU_TS      *)&ts,
                            (OS_ERR      *)&err);

#ifdef DEBUG
        if(OS_ERR_NONE != err)
        {
            while(1);
        }
#endif

        OSMutexPend((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                    (OS_TICK     )0,
                    (OS_OPT      )OS_OPT_PEND_BLOCKING,
                    (CPU_TS     *)&ts,
                    (OS_ERR     *)&err);

        // Parser Msg
        Cmd = CAN_CmdParser(p_msg);

        OSMutexPost((OS_MUTEX   *)&GLOBAL_DATA_PROTECT,
                    (OS_OPT      )OS_OPT_POST_NONE,
                    (OS_ERR     *)&err);        


        // Release Msg Memory
        OSMemPut((OS_MEM  *)&CAN_Msg,
                 (void    *)p_msg,
                 (OS_ERR  *)&err);
#ifdef DEBUG
        if(OS_ERR_NONE != err)
        {
            while(1);
        }
#endif
        
#if 1        
        if(Cmd != (uint16_t)0)
        {
            // Send Message to Wireless Process
            OSTaskQPost((OS_TCB     *)&AppTaskRFTCB,
                        (void       *)Cmd,
                        (OS_MSG_SIZE )2,
                        (OS_OPT      )OS_OPT_POST_FIFO,
                        (OS_ERR     *)&err);
#ifdef DEBUG
            if(OS_ERR_NONE != err)
            {
                while(1);
            }
#endif
        }
#endif

    }
}

static  void  AppTaskRF (void *p_arg)
{
    OS_ERR       err;
    CPU_TS       ts;
    void         *p_msg;
    OS_MSG_SIZE  msg_size;

    while(1)
    {
        p_msg = OSTaskQPend(0,
                            OS_OPT_PEND_BLOCKING,
                            &msg_size,
                            &ts,
                            &err);
        LED_Triggle(0);       
        
#ifdef DEBUG
        if(OS_ERR_NONE != err)
        {
            while(1);
        }
#endif
        RF_CmdParser((uint16_t)p_msg);
    }
}

static  uint16_t CAN_CmdParser(void * p_msg)
{
    CanRxMsg *     CanMsg = (CanRxMsg *)p_msg;
    CAN_CMD_T*     CanCmd;
    uint16_t       DataUpdate = 0;

#ifdef DEBUG
    if(NULL == p_msg)
    {
        while(1);
    }
#endif

    CanCmd = (CAN_CMD_T *)&(CanMsg->Data[0]);

    //PC_PrintStr("\r\n");
    //PC_PrintBlock((void*)CanCmd, sizeof(CAN_CMD_T));
    //PC_PrintCh(CanCmd->seq);
    //if(CanCmd->seq <= 4)
    //{
    //    PC_PrintStr("\r\n");
    //}

    switch(CanCmd->cmd)
    {
        case CAN_CMD_BASIC:
            {
                //PC_PrintCh(CanCmd->seq);
                if(CanCmd->seq == (uint8_t)0)
                {
                    uint8_t offset = 16;
                    memcpy((void*)((uint8_t*)&g_CarInfo+offset), (void*)CanCmd->data, 4);
                    DataUpdate = CMD_PROPERTY;
                }
                else if((CanCmd->seq) <= (uint8_t)4)
                {
                    uint8_t offset = (4 - CanCmd->seq)*4;
                    memcpy((void*)((uint8_t*)&g_CarInfo+offset), (void*)CanCmd->data, 4);
                }
                break;
            }
        case CAN_CMD_FIGHT:
            {
                memcpy((void*)&g_CarStatus, (void*)CanCmd->data, 4);
                DataUpdate = CMD_STATUS;
                break;
            }
        case CAN_CMD_POSITION:
            {
                memcpy((void*)&(g_CarStatus.position), (void*)CanCmd->data, 2);
                DataUpdate = CMD_STATUS;                
                break;
            }
        case CAN_CMD_SPEED:
            {
                memcpy((void*)&(g_CarStatus.speed), (void*)CanCmd->data, 3);
                DataUpdate = CMD_STATUS;
                break;
            }     
        case CAN_CMD_EQUIMENT:
            {
                memcpy((void*)&(g_CarEquiment), (void*)CanCmd->data, 3);
                DataUpdate = CMD_EQUIMENT;                
                break;
            }
        case CAN_CMD_ATTACKED:
            {
                memcpy((void*)&(g_CarAttacked), (void*)CanCmd->data, 4);
                DataUpdate = CMD_ATTACKED;                
                break;
            }             
        default:
            {
                break;
            }
    }
    
    return (DataUpdate);
}

static  uint16_t RF_CmdParser (uint16_t cmd)
{
    RF_Cmd CmdPacket;
    
    switch(cmd)
    {
        case CMD_PROPERTY:
            {
                CmdPacket.len    = (uint8_t )(3 + sizeof(CAR_INFO));
                CmdPacket.addr   = (uint8_t )CC1101_DRV_ADDR;
                CmdPacket.cmd_id = (uint16_t)CMD_PROPERTY;
                memcpy((void*)(&(CmdPacket.data[0])), (void*)&g_CarInfo, sizeof(CAR_INFO));
                CC1101_PacketSend((uint8_t*)&CmdPacket, CmdPacket.len + 1);
                
                break;
            }
        case CMD_STATUS:
            {
                CmdPacket.len    = (uint8_t )(3 + sizeof(CAR_STATUS));
                CmdPacket.addr   = (uint8_t )CC1101_DRV_ADDR;
                CmdPacket.cmd_id = (uint16_t)CMD_STATUS;
                memcpy((void*)(&(CmdPacket.data[0])), (void*)&g_CarStatus, sizeof(CAR_STATUS));
                CC1101_PacketSend((uint8_t*)&CmdPacket, CmdPacket.len + 1);                
                break;
            }
        case CMD_ATTACKED:
            {
                break;
            }
        case CMD_EQUIMENT:
            {
                break;
            }
        default:
            {
                break;
            }
    }
}