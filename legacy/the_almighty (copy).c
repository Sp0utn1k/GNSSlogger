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

typedef struct NAV_POSLLH {
	unsigned char cls;
	unsigned char id;
	unsigned short len;
	unsigned long iTOW;
	long lon;
	long lat;
	long height;
	long hMSL;
	unsigned long hAcc;
	unsigned long vAcc;
} NAV_POSLLH;

char checksum[2];
char port[20];

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
    
    NAV_POSLLH posllh;
	char read_buf [1];
	char msg[256];

	memset(msg,0,strlen(msg));


	int fpos;
	
	while (1) {

		fpos = 0;
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
		read_n_bytes(&connection,&msg[0],1);
		memcpy(&posllh.cls,&msg[0],1);

		if (posllh.cls != (char) 0x01) {
			printf("Wrong class, excpecting posllh\n");
			continue;
		}

		read_n_bytes(&connection,&msg[1],1);
		memcpy(&posllh.id,&msg[1],1);

		if (posllh.id != (char) 0x02) {
			printf("Wrong id, excpecting posllh\n");
			continue;
		}

		read_n_bytes(&connection,&msg[2],2);
		memcpy(&posllh.len,&msg[2],2);

		read_n_bytes(&connection,&msg[4],posllh.len);
		read_n_bytes(&connection,&msg[posllh.len+4],2);
		// printf("Payload : ");
		// print_hex(msg,0,4+len);

		memcpy(&posllh.iTOW,&msg[4],4);
		memcpy(&posllh.lon,&msg[8],4);
		memcpy(&posllh.lat,&msg[12],4);
		memcpy(&posllh.height,&msg[16],4);
		memcpy(&posllh.hMSL,&msg[20],4);
		memcpy(&posllh.hAcc,&msg[24],4);
		memcpy(&posllh.vAcc,&msg[28],4);
		memcpy(&checksum,&msg[32],2);

		int length = posllh.len+4;

		if (!verify_checksum(msg,length,checksum)) {
			printf("Bad ckecsum \n");
			continue;
		}
		
		printf("length : %d\n",posllh.len);
		printf("TOW : %lu\n",posllh.iTOW);
		printf("Longitutde: %ld\n",posllh.lon);
		printf("Latitude: %ld\n",posllh.lat);
		printf("\n");
	}
    close_connection(connection);
    return 1;
}