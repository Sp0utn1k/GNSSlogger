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
char port[20] = "ttyACM0"; // Default serial port
char outputfile[256] = "output.txt";
char msg[256];
int fpos = 0;
unsigned short len;
char prev_arg;
bool to_txt = false;
bool erase = false;

const char HEADER[] = { 0xB5, 0x62 };

void print_hex(char msg[],int start,int len) {
	int i;
	for (i=start;i<len+start;i++) {
		printf("0x%02hhx ", msg[i]);
	}
	printf("\n");
}

bool verify_checksum(char msg[],int len,char checksum[]) {
	char CK_A = 0;
	char CK_B = 0;
		for (int i = 2; i < len; i++) {
		CK_A = CK_A + msg[i];
		CK_B = CK_B + CK_A;
	}
	return CK_A==checksum[0] && CK_B==checksum[1];
}

void display_help() {
	printf("Help section for u-blox configuration : \n\n");
	printf("%-20s", "\t-h"); printf("Display this help section.\n");
	printf("%-20s", "\t-p [port]");printf("Specify serial port. Default : ttyACM0.\n");
	printf("%-20s", "\t-o [output filename]");printf("Specify output filename. Default : %s.\n",outputfile);
	printf("%-20s", "\t-e"); printf("If output is enabled with -o, erases output file before printing.\n");
}

void display_ubx(char msg[],int len) {
	for (int i=0;i<len;i++) {
		printf("%02hhx ",msg[i]);
		if ((i+1) % 16 == 0 && i < len-1) {
			printf("\n");
		}
	}
	printf("\n\n");
}

void save_ubx(char msg[],int len,FILE * fp) {
	for (int i=0;i<len;i++) {
		fprintf(fp,"%02hhx ",msg[i]);
		if ((i+1) % 16 == 0 && i < len-1) {
			fprintf(fp,"\n");
		}
	}
	fprintf(fp,"\n\n");
}
int main(int argc, char *argv[]){

	for (int i=1;i<argc;i++) {
		
		if (prev_arg == 'p') {
			prev_arg = '\0';
			strcpy(port,argv[i]);
		}

		if (prev_arg == 'o') {
			prev_arg = '\0';
			strcpy(outputfile,argv[i]);
		}

		if (strcmp(argv[i],"-h")==0) {
			display_help();
		}

		if (strcmp(argv[i],"-p") == 0) {
			if (argc == i+1 || argv[i+1][0] == '-') {
				printf("Error : No serial port specified after -p.\n");
				return 0;
			}
			prev_arg = 'p';
		}

		if (strcmp(argv[i],"-o") == 0) {
			to_txt = true;
			if (argc == i+1 || argv[i+1][0] == '-') {
				printf("No output filename specified after -o. Default : %s\n",outputfile);
			}
			else {
				prev_arg = 'o';
			}
		}

		if (strcmp(argv[i],"-e")==0) {
			erase = true;
		}
	}

	FILE * fp;
	if (to_txt) {
		if (erase) {
			fp = fopen(outputfile, "w");
		}
		else if ((fp = fopen(outputfile, "r")))
	    {
	    	fp = fopen(outputfile, "a");
	    	fprintf(fp,"\n\n");
	    }
	    else {
	    	fp = fopen(outputfile, "a");
	    }
	}
	

    Connection connection = setup_connection(port);
	memset(msg,0,sizeof(msg));
	while (1) {
		while (fpos<2) {
			read_n_bytes(&connection,&msg[fpos],1);
			if (msg[fpos] == HEADER[fpos]) {
				fpos++;
			}
			else {
				fpos = 0;
			}
		}
		read_n_bytes(&connection,&msg[2],4);
		len = (unsigned short) msg[4];
		len |= ((unsigned short) msg[5]) << 8;
		read_n_bytes(&connection,&msg[6],len+2);
		memcpy(checksum,&msg[len+6],2);
		if (!verify_checksum(msg,len+6,checksum)) {
			printf("Bad checksum \n");
			continue;
		}
		if (to_txt) {
			save_ubx(msg,len+8,fp);
		}
		else {
			display_ubx(msg,len+8);
		}
		fpos = 0;
	}
    close_connection(connection);
    fclose(fp);
    return 1;
}