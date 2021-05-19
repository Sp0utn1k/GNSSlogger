#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char port[20] = "ttyACM0";
char prev_arg = '\0';


void display_help() {
	printf("Help section for u-blox configuration : \n\n");
	printf("%-20s", "\t-h"); printf("Display this help section.\n");
	printf("%-20s", "\t-p [port]");printf("Specify serial port. Default : ttyACM0.\n");
}

int main(int argc, char *argv[])  {

	if (argc == 1) {
		display_help();
	}

	for (int i=1;i<argc;i++) {
		
		if (prev_arg == 'p') {
			printf("%s\n",argv[i]);
			prev_arg = '\0';
			strcpy(port,argv[i]);
		}

		if (strcmp(argv[i],"-h")==0) {
			display_help();
		}

		if (strcmp(argv[i],"-p") == 0) {
			if (argc == i+1) {
				printf("Error : No serial port specified after -p.\n");
				return 0;
			}
			if (argv[i+1][0] == '-') {
				printf("Error : No serial port specified after -p.\n");
				return 0;
			}
			printf("Serial port : ");
			prev_arg = 'p';
		}


	}
	return 1;
}