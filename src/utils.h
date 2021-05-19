#include<stdio.h>
#include <string.h>


static const char HEADER[2] = { 0xB5, 0x62 };
static const char ACK[2] = {0x05, 0x01};
static const char NAK[2] = {0x05, 0x00};
static const char CFG_cls = 0x06;


void print_hex(char msg[],int start,int len);
bool verify_checksum(char msg[],int len,char checksum[]);
