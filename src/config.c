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

void save_cfg(Connection* connection) {
	char msg[20];
	memset(msg,0,strlen(msg));
	char header[] = {0xb5,0x62,0x06,0x09,0x12,0x00};
	memcpy(&msg,&header,strlen(msg));
	int len = strlen(header);
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
	write(connection->serial_port, msg, 20);

	bool ok = false;
	int fpos;
	char buf[10];

	while (ok ==false) {
		fpos = 0;
		while (fpos<2) {
			read_n_bytes(connection,&buf[fpos],1);
			if (buf[fpos] == HEADER[fpos]) {
				fpos++;
			}
			else {
				fpos = 0;
				printf("%02hhx\n",buf[fpos]);
			}
		}

		if (fpos==2) {
			read_n_bytes(connection,&buf[fpos],1);
			printf("%02hhx\n",buf[2]);
			fpos++;
			if (buf[2]!=ACK[0]) {
				printf("Not ACK msg\n");
			}
		}

		if (fpos==3) {
			read_n_bytes(connection,&buf[fpos],1);
			fpos++;
			ok = true;
			if (buf[3]==ACK[1]) {
				printf("ACK\n");
			}
			else if (buf[3]==NAK[1]) {
				printf("NAK\n");
			}
			else {
				printf("Unexpected ACK msg\n");
			}
		}
	}
}

int main() {
	Connection connection = setup_connection();
	while (true) {
		save_cfg(&connection);
		printf("\n");
	}
	
	close_connection(connection);
	return 1;
}

