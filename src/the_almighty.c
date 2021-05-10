#include<stdio.h>
#include <string.h>
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

	read_n_bytes(connection,&msg[0],2); // read header
	if (!(msg[0] == (char) 0xb5 && msg[1] == (char) 0x62)) {
		printf("Wrong header\n");
		print_hex(msg,0,2);
		return 0;
	}
	read_n_bytes(connection,&msg[0],4);	
	if (!(msg[0] == (char) 0x01 && msg[1] == (char) 0x02)) {
		printf("Not posllh\n");
		return 0;
	}
	char2posllh(msg,&posllh,4);
	read_n_bytes(connection,&msg[4],posllh.len);
	printf("Payload : ");

	char2posllh(msg, &posllh,4+posllh.len);	
	print_hex((char*) &posllh,0,4+posllh.len);

	printf("Class : %d\n",posllh.cls);
	printf("ID : %d\n",posllh.id);
	printf("length : %d\n",posllh.len);
	printf("TOW : %lu\n",posllh.iTOW);
	printf("Longitutde: %ld\n",posllh.lon);

    close_connection(connection);
    return 0;
}
