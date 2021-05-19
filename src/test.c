#include<stdio.h>
#include<stdlib.h>
#include "config_data.h"

void print_hex(char msg[],int start,int len) {
	int i;
	for (i=start;i<len+start;i++) {
		printf("0x%02hhx ", msg[i]);
	}
	printf("\n");
}

int main(int argc,char* argv[]){

	Config_field field= get_field(argv[1]);
	printf("%s        %s\n",field.cmd_line_arg,field.description);
	
	unsigned char out_buff[10];
	int len;
	
	field.make_message(&field.key[0],argv[2],&out_buff[0],&len);
	print_hex(out_buff,0,len);
}