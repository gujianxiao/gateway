#ifndef DEFINE_H
#define DEFINE_H

#include <inttypes.h>
#include <stdlib.h>


#define T_BUF_LEN           15
#define USB_PATH_PORT   "/dev/ttyUSB0"
#define EXPIRE_TIME         10
#define MY_NOTE             1//是否打印调试信息：0-不打印; 1-打印
#define DEBUG                 if(MY_NOTE)
#define NAME_PREFIX             "wsn"
#define NAME_PREFIX_LEN     3

#define PTR_MAX 128
#define TOPO_MSG_LEN 20
#define SHORTEST_DATA_FRAME_LEN 25
//modifyBy cb
//#include<timer.h>
//用以进程间通信保证时间同步；
//Timer lifeTime[10000];
//int lifeTimeCount=0; 
//#define longestRequestTime 10000;



uint16_t top;
uint16_t bottom;

/*
typedef struct node_name {
    uint16_t x;
    uint16_t y;
}node_name;
*/

typedef struct BTNode{
    int child_num;
    struct BTNode *ptr[PTR_MAX];
    uint16_t nodeID;
    struct BTNode *parent;
}BTNode;

extern int fdusb;

#pragma pack(1)
typedef enum Type{
    Light,
    Temp,
    Humidity,
 }Type;

 typedef enum messageType{
    BRO=1,
    IN=2,             //interest
    DATA=3,       //content
    MAPPING=4,     //topology responding type
    TOPOLOGY=5,     //topology asking type
}messageType;


 /**
 * 经纬度结构
 */
typedef struct point{
    uint16_t x; //经度
    uint16_t y; //维度
}point;

typedef struct node_mapping {
    short expire;
    point coordinate;
}node_mapping;

node_mapping nodeID_mapping_table[256];

/**
 * 范围结构，用来当作路由能力，进行前缀匹配
 * 范围用矩形表示
 */
typedef struct location{
    point leftDown; //矩形左下角节点
    point rightUp; //矩形右上角节点
}location;

/**
 * name的结构
 * dataType:
 * Temp,Light,Energy
 */ 
typedef struct name{
    location ability;
    uint16_t dataType;
	uint16_t begintime;
	uint16_t endtime;
	
}interest_name;//10B

typedef struct message{
    uint16_t msgType;
    interest_name msgName;
    uint16_t data;
 }Msg;

/*
typedef struct topology_message{
    uint16_t msgType;
    char data[TOPO_MSG_LEN];
 }topo_msg;
*/

typedef struct topology_message{
    uint16_t num;
    uint16_t data[10];
}topo_msg;

topo_msg* queue[PTR_MAX];

/*
typedef struct node_info{
    point coordinate;
    uint16_t nodeID;
}node_info;
*/

typedef struct node_info{
    location coordinate;
    uint16_t nodeID;
}node_info;

typedef struct tinyosndw_payload{
    uint16_t dst_addr;
    uint16_t src_addr;
    uint8_t msg_len;
    uint8_t groupID;
    uint8_t handlerID;
    Msg     content;
}tinyosndw_payload;//21B

typedef struct tinyosndw_head{
    uint8_t F;
    uint8_t P;
    uint8_t S;
    uint8_t D;
}tinyosndw_head;//4B

typedef struct tinyosndw_end{
    uint16_t CRC;
    uint8_t F;
}tinyosndw_end;//3B

typedef struct tinyosndw_packet{
    tinyosndw_head head;
    tinyosndw_payload payload;
    tinyosndw_end end;
}tinyosndw_packet;
#pragma pack()

/*数据类型，可扩展*/
enum ndw_data_type
{
    NDW_DATA_TYPE_REQ = 1,  //请求数据包
    NDW_DATA_TYPE_RSP = 2,  //回应数据包
    NDW_DATA_TYPE_ACK = 4,  //for test
    NDW_NEIB_TYPE_REQ = 4,  //
    NDW_NEIB_TYPE_RSP = 8,  //
    NDW_PATH_TYPE_REQ = 16, //
    NDW_PATH_TYPE_RSP = 32, //
};
#endif