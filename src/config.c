#include<stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef __unix__
    
    #include <fcntl.h> // Contains file controls like O_RDWR
    #include <errno.h> // Error integer and strerror() function
    #include <termios.h> // Contains POSIX terminal control definitions
    #include <unistd.h> // write(), read(), close()

#elif defined(_WIN32) || defined(WIN32)
	#include<windows.h>

#endif
#include"connect.h"

const char HEADER[] = { 0xB5, 0x62 };
const char ACK[] = {0x05, 0x01};
const char NAK[] = {0x05, 0x00};
const char CFG_cls = 0x06;
int fpos;

void print_hex(char msg[],int start,int len) {
	int i;
	for (i=start;i<len+start;i++) {
		printf("0x%02hhx ", msg[i]);
	}
	printf("\n");
}


void save_cfg(Connection* connection) {
	char msg[20];
	memset(msg,0,sizeof(msg));
	char header[] = {0xb5,0x62,0x06,0x09,0x0C,0x00};
	int len = sizeof(header);
	memcpy(&msg,&header,len);
	for (int i=len+4;i<len+8;i++) {
		msg[i] = 0xff;
	}

	char CK_A = 0;
	char CK_B = 0;
	for (int i = 2; i < 18; i++) {
		CK_A = CK_A + msg[i];
		CK_B = CK_B + CK_A;
	}
	msg[18] = CK_A;
	msg[19] = CK_B;
	write_n_bytes(connection,msg,20);
}

bool verify_checksum(char msg[],int len,char checksum[]) {
	char CK_A = 0;
	char CK_B = 0;
		for (int i = 0; i < len; i++) {
		CK_A = CK_A + msg[i];
		CK_B = CK_B + CK_A;
	}
	return CK_A==checksum[0] && CK_B==checksum[1];
}

bool check_ack(Connection* connection) {
	bool ok = false;
	bool is_ACK;
	int fpos;
	char buf[8];
	char checksum[2];

	while (ok ==false) {
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
			read_n_bytes(connection,&buf[0],1);
			if (buf[0]!=ACK[0]) {
				fpos = 0;
				continue;
			}
			else {
				ok = true;
			}

			read_n_bytes(connection,&buf[1],1);
			if (buf[1]==ACK[1]) {
				is_ACK = true;
			}

			read_n_bytes(connection,&buf[2],6);
			memcpy(&checksum,&buf[6],2);
			int len = 6;

			if (!verify_checksum(buf,len,checksum)) {
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

int main() {
	Connection connection = setup_connection();
	//while (true) {
		save_cfg(&connection);
		
		if(check_ack(&connection))
				printf("ok\n");;
	//}
	close_connection(connection);
	return 1;
}

