#include<stdio.h>
#include <string.h>
#include <stdbool.h>
#include<stdlib.h>
#include<stdint.h>
#include<time.h>

#ifdef __unix__
    
    #include <fcntl.h> // Contains file controls like O_RDWR
    #include <errno.h> // Error integer and strerror() function
    #include <termios.h> // Contains POSIX terminal control definitions
    #include <unistd.h> // write(), read(), close()

#elif defined(_WIN32) || defined(WIN32)
	#include<windows.h>

#endif

#include"connect.h"
#include"utils.h"

const char ACK[2] = {0x05, 0x01};
const char NAK[2] = {0x05, 0x00};
const char CFG_cls = 0x06;


bool check_ack(Connection* connection) {
	/*
	Reads messages until an ack or a nack is received
	*/
	const char HEADER[2] = { 0xB5, 0x62 };
	bool ok = false;
	bool is_ACK = false;
	int fpos;
	char buf[10];
	char checksum[2];
	time_t time_buf = time(NULL);
	unsigned long timeout = 1;


	while (ok ==false && (time(NULL)-time_buf)<timeout) {
		fpos = 0;
		while (fpos<2) {
			read_n_bytes(connection,&buf[fpos],1);
			if (buf[fpos] == HEADER[fpos]) {
				fpos++;
			}
			else {
				fpos = 0;
			}
		}

		if (fpos==2) {
			read_n_bytes(connection,&buf[2],1);
			if (buf[2]!=ACK[0]) {
				fpos = 0;
				continue;
			}
			else {
				ok = true;
			}

			read_n_bytes(connection,&buf[3],1);
			if (buf[3]==ACK[1]) {
				is_ACK = true;
			}else if (buf[3]==NAK[1]) {
				printf("NAK received");
			}

			read_n_bytes(connection,&buf[4],6);
			memcpy(&checksum,&buf[8],2);
			

			if (!verify_checksum(buf,8,checksum)) {
				printf("Warning : Bad checksum\n");
				is_ACK = false;
			}else{
				return is_ACK;
			}
		}
	}
	is_ACK = false;
	return is_ACK;
}

void wrap_config(char* inwards,int in_len,char* msg,int* msg_len){
	// Builds a UBX-CFG-VALSET message from its payload (keys and values)
	char header[4]={0xb5, 0x62, 0x06, 0x8a};
	memcpy(msg,&header,4);
	uint16_t field_len=4+in_len;
	memset(msg+4,field_len%256,1);
	memset(msg+5,field_len/256,1);
	char middle[4] ={0,0x01,0,0};
	memcpy(msg+6,&middle,4);
	memcpy(msg+10,inwards,in_len);

	compute_checksum(msg,  in_len+10, msg+in_len+10, msg+in_len+11);
	*msg_len=in_len+12;
}

