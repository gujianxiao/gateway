#ifndef NDW_H
#define NDW_H

/*
Main function of NDW.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <string.h>

//#include "ndnpoke.h"
#include "define.h"

extern BTNode *topo_head;

int g_count=0;
//uint16_t co2nodereq = 0;
int co2res = 0;
int armmark = 0;
int nodemark = 0;
int neimark = 0;
int pathmark = 0;
FILE *fp;

/**
 * Cal the CRC Byte
 */
uint16_t crcByte(uint16_t crc, uint8_t b) {
  crc = (uint8_t)(crc >> 8) | (crc << 8);
  crc ^= b;
  crc ^= (uint8_t)(crc & 0xff) >> 4;
  crc ^= crc << 12;
  crc ^= (crc & 0xff) << 5;
  return crc;
}

/**
 * Cal the CRC Val
 */
uint16_t crccal(unsigned char* buf, int len)
{
	uint16_t res = 0;
	int i;

	for ( i = 0; i < len; ++i)
	{
		res = crcByte(res, buf[i]);
	}

	return res;
}

/**
 * Print the MAC addr in HEX
 */
void printhex_macaddr(void *hex, int len, char *tag)
{
	int i;
	unsigned char *p = (unsigned char *)hex;

	if(len < 1)
		return;
  
	for(i = 0; i < len - 1; i++)
	{
		if(*p < 0x10)
			printf("0%x%s", *p++, tag);
		else
			printf("%2x%s", *p++, tag);
	}

	if(*p < 0x10)
		printf("0%x", *p++);
	else
		printf("%2x", *p++);
}

void topo_tree_traversal(BTNode **node, int *count, char *buf, char *temp)
{
	int i;
	sprintf(temp, "%d %d\n", (*node)->nodeID, (*node)->parent->nodeID);
	strcat(buf, temp);
	(*count)++;
	for(i=0; i<(*node)->child_num; i++)
	{
		topo_tree_traversal(&((*node)->ptr[i]), count, buf, temp);
	}
}

/**
 * write the interest into USB
 */
void usb_write(interest_name *interest, uint16_t type)
{
	DEBUG printf("enter the function usb_write~~\n");
	int nread;

	tinyosndw_packet *packet = (tinyosndw_packet*)malloc(sizeof(tinyosndw_packet));
	memset((char*)packet, 0, sizeof(tinyosndw_packet));
	packet->head.F = 0x7e;
	packet->head.P = 0x44;
	packet->head.S = 0x26;
	packet->head.D = 0x00;

	packet->payload.dst_addr = 0x01;
	packet->payload.dst_addr = packet->payload.dst_addr<<8;
	packet->payload.src_addr = 0x00;
	packet->payload.src_addr = packet->payload.src_addr<<8 ;
	packet->payload.msg_len = 0x0E;
	packet->payload.groupID = 0x22;
	packet->payload.handlerID = 0x03;

	packet->payload.content.msgType = type;
	memcpy(&(packet->payload.content.msgName), interest, sizeof(interest_name));

	//packet->payload.content.data[0] = 0x01;
	//packet->payload.content.data[2] = 0xff;
	//packet->payload.content.data[3] = 0xff;

	packet->end.CRC = crccal((char*)packet+1, sizeof(tinyosndw_packet)-4);
	
	packet->end.F = 0x7e;
	printhex_macaddr((char*)packet, sizeof(tinyosndw_packet), " ");
	uint8_t loopflag=1;
	uint8_t ARQ=0;
	while(loopflag)
	{
		nread = write(fdusb, (char*)packet, sizeof(tinyosndw_packet));
		if(nread == -1)
		{
			if(ARQ<5)
			{
				printf("write failed!!\n");
				ARQ++;
			}
			else
			{
				printf("failed to write the interest into the USB port!\n");
				loopflag=0;
			}
		}
		else
		{
			loopflag=0;
			DEBUG printf("write usb successed!!-----NO:[%d]\n", g_count++);
		}
	}	
	free(packet);		
}


/**
 * entrence of ndw
 */
int request_from_backbone(char *interest)
{
	//modifyBy cb;
// lifeTime[lifTimeCount++].start();当有interest时开始计时；

	DEBUG printf("process the interest from backbone\n");
	printf("interest = %s\n", interest);
	
	if(strncmp(interest, "ints", 4) == 0)
	{
		char arg[5][20] = {0};
		int i=0, j=0, count=0;
		char* interest_msg = strstr(interest, "/");
		for(i=0, j=0; interest_msg[i]!='\0'; i++)
		{
			if(interest_msg[i]!=',' && interest_msg[i]!='/' && interest_msg[i]!='(')
			{
				while(interest_msg[i]!=',' && interest_msg[i]!='/' && interest_msg[i]!=')' && interest_msg[i]!='\0')
					arg[count][j++] = interest_msg[i++];
				j=0;
				count++;
			}
		}
		interest_name *name = (interest_name*)malloc(sizeof(interest_name));
		name->ability.leftUp.x = atoi(arg[0]);
		name->ability.leftUp.y = atoi(arg[1]);
		name->ability.rightDown.x = atoi(arg[2]);
		name->ability.rightDown.y = atoi(arg[3]);
		if(!strcmp(arg[4], "light"))
			name->dataType = Light;
		if(!strcmp(arg[4], "temp"))
			name->dataType = Temp;
		if(!strcmp(arg[4], "humidity"))
			name->dataType = Humidity;
	    	DEBUG printf("location is(%d, %d),(%d, %d)\n", name->ability.leftUp.x, name->ability.rightDown.y, 
	                                                                            name->ability.rightDown.x, name->ability.rightDown.y);
	    	DEBUG printf("type is %s(value=%d)\n", arg[4], name->dataType);
			}
			//modifyBy cb
			/* interest_name *searchNodeByLocation(interest_name name, node_mapping *table, int lenth)
{
	int count = 0;
	nodeInfo destTable[];//目标节点表
	interest_name destName[];//目标name表
	for (int i = 0, i < lenth, i++)//遍历获取符合地理范围的节点
	{
		if ((name.ablility.leftDown.x <= table[i].coordinate.x <= name.ablility.rightUp.x) && (name.ablity.leftDown.y <= table[i].coordinate.y <= name.ablility.rightUp.y)
		{
			destTable[count++] = table[i];
		}
	}
	for (i = 0; i < count, i++){//遍历生成目标name信息
		destName[i] = name;
		destName[i].ablility.leftDown.x = destTable[i].coordinate.x;
		destName[i].ablility.leftDown.y= destTable[i].coordinate.y;
	}
return destName;//返回目标节点表}
	destName=searchNodeByLocation(name,nodeID_mapping_table,10);
	int allsize=(int)(sizeof(destName)/sizeof(destName[0]));
	while(allsize){
		usb_write(destName[count], IN);
	}
	free(destName);
}
*/
		usb_write(name, IN);
		free(name);
	}
	else if(strncmp(interest, "location", 8) == 0)
	{
		char name_buf[256]={0};
    		char content_buf[1024]={0};
    		char temp_buf[100]={0};
    		strcpy(name_buf, "ndn:/wsn/");
    		strcat(name_buf, interest);
    		int i;
    		for(i=0; i<256; i++)
    		{
    			if(nodeID_mapping_table[i].expire>=0)
    			{
    				sprintf(temp_buf, "%d %d,%d %d\n", i, nodeID_mapping_table[i].coordinate.x, nodeID_mapping_table[i].coordinate.y, nodeID_mapping_table[i].expire);
    				strcat(content_buf, temp_buf);
    			}
    		}
    		pack_data_content(name_buf, content_buf);
	}
	else if(strncmp(interest, "topo", 4) == 0)
	{
		char name_buf[256]={0};
    		char content_buf[1024]={0};
    		char temp_buf[1024]={0};
    		char temp[20] = {0};
    		strcpy(name_buf, "ndn:/wsn/");
    		strcat(name_buf, interest);
    		int count=0;
    		if(topo_head->child_num>0)
    		{
    			int i;
    			for(i=0; i<topo_head->child_num; i++)
    				topo_tree_traversal(&(topo_head->ptr[0]), &count, temp_buf, temp);
    		}
    		sprintf(temp, "0 %d\n", count);
    		strcat(content_buf, temp);
    		strcat(content_buf, temp_buf);
    		printf("topo response:%s\n", content_buf);
    		pack_data_content(name_buf, content_buf);
	}
	return 0;
}

#endif