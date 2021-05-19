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
/*
void save_cfg(Connection* connection) {
	char msg[20];
	memset(msg,0,sizeof(msg));
	char header[] = {0xb5,0x62,0x06,0x09,0x0C,0x00};
	int len = sizeof(header);
	memcpy(&msg,&header,len);
	for (int i=len+4;i<len+8;i++) {
		msg[i] = 0xff;
	}

	compute_checksum(msg[], len, &msg[18], &msg[19]);
	write_n_bytes(connection,msg,20);
}
*/
bool check_ack(Connection* connection) {
	bool ok = false;
	bool is_ACK = false;
	int fpos;
	char buf[8];
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
			if (buf[0]!=ACK[0]) {
				fpos = 0;
				continue;
			}
			else {
				ok = true;
			}

			read_n_bytes(connection,&buf[3],1);
			if (buf[1]==ACK[1]) {
				is_ACK = true;
			}

			read_n_bytes(connection,&buf[4],6);
			memcpy(&checksum,&buf[8],2);

			if (!verify_checksum(buf,sizeof(8),checksum)) {
				printf("Warning : Bad checksum\n");
				is_ACK = false;
			}else{
				return true;
			}
		}
	}
	is_ACK = false;
	return is_ACK;
}
/*
int main(int argc, char *argv[])  {

	
	Connection connection = setup_connection(port);

	//while (true) {
		save_cfg(&connection);
		
		if(check_ack(&connection)) {
				printf("ok\n");
		}
	//}
	close_connection(connection);
	return 1;
}
*/
void wrap_config(char* inwards,int in_len,char* msg,int* msg_len){
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

