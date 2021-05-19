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
	printf("\n==========   Help section for u-blox configuration :   ============\n\n");
	printf("\t%-25sDisplay this help section.\n", "-h");
	printf("\t%-25sSpecify serial port. Default : ttyACM0.\n", "-p [port]");
	printf("\t%-25sSpecify output filename. Default : output.txt\n", "-o [output filename]");
	printf("\t%-25sIf output is enabled with -o, erases output file before printing.\n", "-e");
	printf("\t%-25sSets the measuring time (unsigned long, in seconds)\n", "-t");

	for(int i=0; i<sizeof(CONFIG_DB)/sizeof(CONFIG_DB[0]);i++){
		
		char cmd[256+8];
		char valuetype[13];
		strcpy(cmd,CONFIG_DB[i].cmd_line_arg);
		sprintf(valuetype," [%s]",CONFIG_DB[i].var_type);
		strcat(cmd,valuetype);
		printf("\t%-25s%s\n",cmd,CONFIG_DB[i].description);
	}
	fflush(stdout);
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

void read_nmea(Connection* connection, char *msg,bool to_txt,FILE* fp) {
	int i = 0;
	while (*(msg+i) != '\n') {
		i++;
		read_n_bytes(connection,(msg+i),1);
	}

	if (to_txt) {
		save_ubx(msg,i+1,fp);
	}
	else {
		display_ubx(msg,i+1);
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
	int n_fields=0;

	char config_message[256];
	memset(config_message,0,256);
	int config_len=0;
	Config_field field;
	bool sending_config=false;
	char a[] = {0x2d,0x70,0x72,0x6f,0x6d,0x00};
	char text[] = {0x4d,0x62,0x21,0x32,0x38,0x33,0x21,0x6e,0x62,0x21,0x6e,0x66,0x73,0x66,0x22};

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
				return 1;
			}
			else if (strcmp(argv[i],"-t")==0) {
				if (argc == i+1 || argv[i+1][0] == '-') {
					printf("No output filename specified after -o. Default : %ld\n",measure_time);
				}else {
					measure_time = atoi(argv[i+1]);
				}
			}else if (strcmp(argv[i],a)==0) {
				for (int i=0;i<sizeof(text);i++) {
					printf("%c",text[i]-1);
				}
				printf("\n");
			}else{
				sending_config=true;
				memset(&field,0,sizeof(field));
				if(get_field(argv[i],&field)){
					int len;
					//printf("%d \n",config_len);
					field.make_message(&field.key[0],argv[i+1],&config_message[config_len] ,&len);
					
					config_len+=len;
					n_fields+=1;
					if(n_fields>64){
						printf("More than 64 cfg messages in one go is not supported.\nPlease split cfg messages in multiple executions of the program\n");
						return 0;
					}
					//print_hex(config_message,0,config_len);
				}else {
					printf("Error : option %s does not exist\n",argv[i]);
					return 0;
				}

				
			}
		}
	}

	Connection connection = setup_connection(port);

	if(sending_config){
		printf("Attempting to send configuration message\n");
		char final_config_msg[800];
		int final_config_len;
		wrap_config(&config_message[0], config_len, &final_config_msg[0], &final_config_len);
		//print_hex(final_config_msg,0,final_config_len);
		//printf("%d",final_config_len);

		int i = 0;
		do {
			if (i>=10) {
				printf("Error : Couldn't send config to u-blox.\n");
				exit(0);
			}
			write_n_bytes(&connection,final_config_msg, final_config_len);
			i++;
		}while (!check_ack(&connection));
		if (i)
		printf("Config message sent correctly\n");

	}
	// exit(1);
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

    
	memset(msg,0,sizeof(msg));
	time_buf = time(NULL);
	int progression =  0;
	while (time(NULL)-time_buf < measure_time) {

		if (to_txt) {
			progression = 100 - 100*(measure_time-(int)(time(NULL)-time_buf))/measure_time;
			printf("\rProgression : %d%%",progression);
			fflush(stdout);
		}

		while (fpos<2) {
			read_n_bytes(&connection,&msg[fpos],1);
			if (msg[fpos] == '$') {
				read_nmea(&connection,&msg[0],to_txt,fp);
			}
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
    printf("\n");
    return 1;
}