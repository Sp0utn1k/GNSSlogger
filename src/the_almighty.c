#include<stdio.h>
#include <string.h>
#include <stdbool.h>
#include<stdlib.h>
// #define MOCK 1

#ifdef __unix__
    
    #include <fcntl.h> // Contains file controls like O_RDWR
    #include <errno.h> // Error integer and strerror() function
    #include <termios.h> // Contains POSIX terminal control definitions
    #include <unistd.h> // write(), read(), close()

#elif defined(_WIN32) || defined(WIN32)
	#include<windows.h>

#endif
#include"connect.h"

char checksum[2];
char port[20];
char msg[256];
char payload[256];
memset(msg,0,strlen(msg));
memset(payload,0,strlen(msg));
int fpos = 0;
unsigned short len;

const char HEADER[] = { 0xB5, 0x62 };

void print_hex(char msg[],int start,int len) {
	int i;
	for (i=start;i<len+start;i++) {
		printf("0x%02hhx ", msg[i]);
	}
	printf("\n");
}

unsigned long to_ulong(char bytes[],int start) {
	unsigned long temp;
	temp = (unsigned long) bytes[start];
	temp |= ((unsigned long) bytes[1+start]) << 8;
	temp |= ((unsigned long) bytes[2+start]) << 16;
	temp |= ((unsigned long) bytes[3+start]) << 24;
	return temp;
}

long to_long(char bytes[],int start) {
	long temp;
	temp= (long) bytes[3+start];
	temp |= ((long) bytes[2+start]) << 8;
	temp |= ((long) bytes[1+start]) << 16;
	temp |= ((long) bytes[0+start]) << 24;
	return temp;
}

unsigned short to_ushort(char bytes[],int start) {
	unsigned short temp;
	temp= (unsigned short) bytes[start];
	temp |= ((unsigned short) bytes[1+start]) << 8;
	return temp;
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

int main(int argc, char *argv[]){

    Connection connection = setup_connection(port);
	
	while (1) {

		while (fpos<2) {
			read_n_bytes(&connection,&msg[fpos],1); // read header
			if (msg[fpos] == HEADER[fpos]) {
				fpos++;
			}
			else {
				// printf("Wrong header at pos : %d\n",fpos);
				fpos = 0;
			}
		}

		read_n_bytes(&connection,&msg[0],4);
		memcpy(len,&msg[2],2);
		read_n_bytes(&connection,&msg[4],len+2);
		memcpy(checksum,&msg[len+4],2);

		int length = len+4;

		if (!verify_checksum(msg,length,checksum)) {
			printf("Bad ckecsum \n");
			continue;
		}

		
	}
    close_connection(connection);
    return 1;
}