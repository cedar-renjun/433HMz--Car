#include <stdint.h>

/************************************* Support Command List **************************************/
#define CMD_PROPERTY         0x1000
#define CMD_STATUS           0x1001
#define CMD_ATTACKED         0x1002
#define CMD_EQUIMENT         0x1003
#define CMD_CONTROL          0x2000
#define CMD_PING             0x3000

/**************************************** Parameters List ****************************************/

// Protocal Version
#define PROTOCAL_HEADER      0x5A
#define PROTOCAL_VERSION     0x01

// Car ID list
#define CAR_ID_A_0           0x01
#define CAR_ID_A_1           0x02
#define CAR_ID_A_2           0x03
#define CAR_ID_A_3           0x04
#define CAR_ID_A_4           0x05
#define CAR_ID_A_5           0x06
#define CAR_ID_A_6           0x07
#define CAR_ID_A_7           0x08

#define CAR_ID_B_0           0x21
#define CAR_ID_B_1           0x22
#define CAR_ID_B_2           0x23
#define CAR_ID_B_3           0x24
#define CAR_ID_B_4           0x25
#define CAR_ID_B_5           0x26
#define CAR_ID_B_6           0x27
#define CAR_ID_B_7           0x28

// Mouse Key Press Flag
#define KEY_NO_PRESS         0x00
#define KEY_PRESS            0x01

// Weapon Type
#define WEAPON_GUN_BB        0x01
#define WEAPON_GUN_GAS       0x02
#define WEAPON_GUN_RUB       0x03
#define WEAPON_CANNON        0x04

// Ping error value
#define ERR_NONE             0x0000
#define ERR_RF               0x0001
#define ERR_CAR              0x0002

#pragma pack(1)
typedef struct
{
    uint8_t     flag;                              // 0x5A ,数据头标志
    uint8_t     length;                            // 协议中数据段（SEQ_NUM+PAYLOAD)长度
    uint8_t     version;                           // 协议版本，方便后续扩展(当前协议版本暂定为0x01)
    uint8_t     crc8;                              // 头校验，对Header前3字节计算CRC8
}HEADER;

#pragma pack(1)
typedef struct
{
    uint16_t    value;                             // 序号，以此判断数据是否有丢包，设备收到数据，
                                                   // 应该在ACK中原样返回此字段
}SEQ_NUM;

#pragma pack(1)
typedef struct
{
    uint16_t    crc16;                             //CRC16 ，对整个协议Frame进行校验
}CHECKSUM;

/*--------------------------------------- Property Frame ----------------------------------------*/

#pragma pack(1)
typedef struct
{
    uint8_t     name[16];                          // 战车名称，ASCII字符，0x00结尾
    uint8_t     car_id;                            // 战车ID编号
    uint8_t     weapon;                            // 武器类型
    uint16_t    blood;                             // 总血量
}CAR_INFO;

#pragma pack(1)
typedef struct
{
    HEADER    header;
    SEQ_NUM   seq_number;
    uint16_t  cmd_id;
    CAR_INFO  data;
    CHECKSUM  checksum; 
}CAR_INFO_T;

/*---------------------------------------- status Frame -----------------------------------------*/

#pragma pack(1)
typedef struct
{
    uint8_t     power;                             // 当前电量（0-100）
    uint8_t     bullet;                            // 当前弹药（0-100）
    uint16_t    blood_cur;                         // 当前血量
    struct
    {
        int8_t  x;                                 // 位置X
        int8_t  y;                                 // 位置Y
    }position;
    struct
    {
        int8_t  x;                                 // 平动x轴速度 (-128 - 127)
        int8_t  y;                                 // 平动y轴速度 (-128 - 127)
        int8_t  r;                                 // 旋转速度 (-128 - 127)
    }speed;
    uint8_t     linkquality;                       // 信道质量

}CAR_STATUS;

#pragma pack(1)
typedef struct
{
    HEADER      header;
    SEQ_NUM     seq_number;
    uint16_t    cmd_id;
    CAR_STATUS  data;
    CHECKSUM    checksum; 
}CAR_STATUS_T;

/*----------------------------------------- Control Frame ---------------------------------------*/

#pragma pack(1)
typedef struct
{
    int16_t  mouse_x;                               // 鼠标坐标X
    int16_t  mouse_y;                               // 鼠标坐标Y
    int16_t  mouse_z;                               // 鼠标坐标Z
    uint8_t  left_key_press_flag;                   // 鼠标左键是否按下
    uint8_t  right_key_press_flag;                  // 鼠标右键是否按下
    uint16_t keyboard_value;                        // 当前键盘按键
}CONTROL_INFO;

#pragma pack(1)
typedef struct
{
    uint16_t      header;
    CONTROL_INFO  data;    
    uint8_t       checksum;
}CONTROL_INFO_T;

/*------------------------------------------- Ping Frame ----------------------------------------*/

#pragma pack(1)
typedef struct
{    
    uint16_t  status;
}PING;

#pragma pack(1)
typedef struct
{
    HEADER    header;
    SEQ_NUM   seq_number;
    uint16_t  cmd_id;
    PING      data;
    CHECKSUM  checksum; 
}PING_T;

/*----------------------------------------- equiment Frame --------------------------------------*/

#pragma pack(1)
typedef struct
{
    uint8_t     type;                              // 额外装备类型
    int16_t     blood;                             // （增/减）血量信息
}EQUIMENT_INFO;

#pragma pack(1)
typedef struct
{
    HEADER        header;
    SEQ_NUM       seq_number;
    uint16_t      cmd_id;
    EQUIMENT_INFO data;
    CHECKSUM      checksum;
}EQUIMENT_INFO_T;

/*------------------------------------------ Attack Frame ---------------------------------------*/

#pragma pack(1)
typedef struct
{
    uint8_t     weapon;                            // 攻击武器类型
    uint8_t     point;                             // 打击点
    uint16_t    blood;                             // 失血量
}ATTACKED_INFO;

#pragma pack(1)
typedef struct
{
    HEADER        header;
    SEQ_NUM       seq_number;
    uint16_t      cmd_id;
    ATTACKED_INFO data;
    CHECKSUM      checksum;
}ATTACKED_INFO_T;

/**********************************************************************************/
#define CAN_CMD_BASIC           (uint8_t)0x01
#define CAN_CMD_FIGHT           (uint8_t)0x02
#define CAN_CMD_POSITION        (uint8_t)0x03
#define CAN_CMD_SPEED           (uint8_t)0x04
#define CAN_CMD_ATTACKED        (uint8_t)0x05
#define CAN_CMD_EQUIMENT        (uint8_t)0x06

#pragma pack(1)
typedef struct
{
    uint8_t sender;
    uint8_t cmd;
    uint8_t seq;
    uint8_t dirty_flag;
    uint8_t data[4];
}CAN_CMD_T;