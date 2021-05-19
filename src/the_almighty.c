#include<stdio.h>
#include <string.h>
#include <stdbool.h>
#include<stdlib.h>
#include <time.h>

// #define MOCK 1

#ifdef __unix__
    
    #include <fcntl.h> // Contains file controls like O_RDWR
    #include <errno.h> // Error integer and strerror() function
    #include <termios.h> // Contains POSIX terminal control definitions
    #include <unistd.h> // write(), read(), close()

#elif defined(_WIN32) || defined(WIN32)
	#include<windows.h>

#endif

#include"utils.h"
#include"connect.h"
#include"config.h"
#include"config_data.h"

void display_help() {
	printf("Help section for u-blox configuration : \n\n");
	printf("%-20s", "\t-h"); printf("Display this help section.\n");
	printf("%-20s", "\t-p [port]");printf("Specify serial port. Default : ttyACM0.\n");
	printf("%-20s", "\t-o [output filename]");printf("Specify output filename. Default : output.txt\n");
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
		fprintf(fp,"%c",msg[i]);
	}
}

int main(int argc, char *argv[]){

	char checksum[2];
	char port[20] = "ttyACM0"; // Default serial port
	char outputfile[256] = "output.ubx";
	char msg[256];
	int fpos = 0;
	unsigned short len;
	bool to_txt = false;
	bool erase = false;
	unsigned long measure_time = 10;
	time_t time_buf;

	char config_message[256];
	memset(config_message,0,256);
	int config_len=0;
	Config_field field;

	for (int i=1;i<argc;i++) {
		
		if(argv[i][0]=='-'){
			if (strcmp(argv[i],"-p")==0) {
				if(argc == i+1 || argv[i+1][0] == '-') {
					printf("Error : No serial port specified after -p.\n");
					return 0;
				}else {
					strcpy(port,argv[i+1]);
				}
			} else if (strcmp(argv[i],"-o")==0) {
				to_txt = true;
				if (argc == i+1 || argv[i+1][0] == '-') {
					printf("No output filename specified after -o. Default : %s\n",outputfile);
				}
				else {
					strcpy(outputfile,argv[i+1]);
				}
			}else if (strcmp(argv[i],"-e")==0) {
				erase = true;
			}else if (strcmp(argv[i],"-h")==0) {
				display_help();
			}
			else if (strcmp(argv[i],"-t")==0) {
				if (argc == i+1 || argv[i+1][0] == '-') {
					printf("No output filename specified after -o. Default : %ld\n",measure_time);
				}else {
					measure_time = atoi(argv[i+1]);
				}
			}else{
				memset(&field,0,sizeof(field));
				if(get_field(argv[i],&field)){
					int len;
					//printf("%d \n",config_len);
					field.make_message(&field.key[0],argv[i+1],&config_message[config_len] ,&len);
					
					config_len+=len;
					//print_hex(config_message,0,config_len);
				}else {
					printf("Error : option %s does not exist\n",argv[i]);
					return 0;
				}

				
			}
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
	    	fprintf(fp,"\n");
	    }
	    else {
	    	fp = fopen(outputfile, "a");
	    }
	    // fprintf(fp, "\n========= START OF MEASURE =========\n");
	}

	

    Connection connection = setup_connection(port);
	memset(msg,0,sizeof(msg));
	time_buf = time(NULL);
	while (time(NULL)-time_buf < measure_time) {
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
    // fprintf(fp, "========= END OF MEASURE =========\n");
    if (to_txt) {
    	fclose(fp);
    }
    return 1;
}