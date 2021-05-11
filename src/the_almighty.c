#include<stdio.h>
#include <string.h>
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

char cls;
char id;
int len;
unsigned long TOW;
long lon;
long lat;
long height;
long hMSL;
unsigned long hAcc;
unsigned long vAcc;

const char HEADER[] = { 0xB5, 0x62 };

void print_hex(char msg[],int start,int len) {
	int i;
	for (i=start;i<len+start;i++) {
		printf("0x%02hhx ", msg[i]);
	}
	printf("\n");
}

void char2posllh(char msg[], NAV_POSLLH* posllh, int len) {
	for (int i=0;i<len;i++) {
		((unsigned char*)(posllh))[i] = msg[i];
	}
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

int main(){

    Connection connection = setup_connection();
    
    NAV_POSLLH posllh;
	char read_buf [1];
	char msg[256];

	memset(msg,0,strlen(msg));

	// for (int i=0;i<10;i++){
	// 	int n = read(serial_port, &read_buf, sizeof(read_buf));
	// 	// printf("Received byte : %02hhx\n",read_buf[0]); // HEX format, adds 0 if needed
	// 	msg[i] = read_buf[0];
	// }

	int fpos = 0;
	
	for (int i=0;i<10;i++) {

		while (fpos<2) {
			read_n_bytes(&connection,&msg[fpos],1); // read header
			if (msg[fpos] == HEADER[fpos]) {
				fpos++;
			}
		}
		read_n_bytes(&connection,&msg[0],2);

		if (!(msg[0] == (char) 0x01 && msg[1] == (char) 0x02)) {
			printf("Not posllh\n");
			continue;
		}

		read_n_bytes(&connection,&msg[2],2);
		cls = msg[0];
		id = msg[1];
		len = (int) to_ushort(msg,2);
		// printf("length : %d\n",len);

		read_n_bytes(&connection,&msg[4],len);
		// printf("Payload : ");
		// print_hex(msg,0,4+len);

		TOW = to_ulong(msg,4);
		lon = to_long(msg,8);
		
		// printf("Class : %d\n",cls);
		// printf("ID : %d\n",id);
		// printf("length : %d\n",len);
		printf("TOW : %lu\n",TOW);
		// printf("Longitutde: %ld\n",lon);
		fpos = 0;
	}
    close_connection(connection);
    return 1;
}
