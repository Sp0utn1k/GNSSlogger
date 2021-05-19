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
	for(int i; i<sizeof(CONFIG_DB)/sizeof(CONFIG_DB[0]);i++){
		char cmd[256+8];
		char valuetype[13];
		strcpy(cmd,CONFIG_DB[i].cmd_line_arg);
		sprintf(valuetype," [%s]",CONFIG_DB[i].var_type);
		strcat(cmd,valuetype);
		printf("\t%-25s%s\n",cmd,CONFIG_DB[i].description);
	}
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

	char checksum[2];
	char port[20] = "ttyACM0"; // Default serial port
	char outputfile[256] = "output.txt";
	char msg[256];
	int fpos = 0;
	unsigned short len;
	bool to_txt = false;
	bool erase = false;

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
			}else{
				memset(&field,0,sizeof(field));
				if(get_field(argv[i],&field)){
					int len;
					//printf("%d \n",config_len);
					field.make_message(&field.key[0],argv[i+1],&config_message[config_len] ,&len);
					
					config_len+=len;
					//print_hex(config_message,0,config_len);
				}

				
			}
		}
	}

	exit(1);

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