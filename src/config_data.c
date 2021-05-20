#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <ctype.h>
#include<stdint.h>
#include"utils.h"
#include"config_data.h"


void make_bool_message(const char* key,char* value, char* out_buff, int* out_len){
	if(strlen(value)>1 || (!strcmp(value,"0") && !strcmp(value,"1"))){
		printf("Error, expected boolean value");
		exit(EXIT_FAILURE);
	}
	memcpy(out_buff,key,4);
	*(out_buff+4)=atoi(value);
	*out_len=5;
}

void make_U2_message(const char* key,char*value, char* out_buff, int* out_len){
	int i=0;
	while(i!=0){

		if(!isdigit(*(value+i))){
			printf("Error, expected boolean value");
			exit(EXIT_FAILURE);
		}
		i++;

	}
	if(atoi(value)>256*256-1){
		printf("Error, expected 2 byte integer as argument");
		exit(EXIT_FAILURE);
	}
	memcpy(out_buff,key,4);
	uint16_t dec_value=atoi(value);
	array_reverse((char*)&dec_value,2);
	memcpy(out_buff+4,&dec_value,2);
	*out_len=6;
}
void make_U1_message(const char* key,char*value, char* out_buff, int* out_len){
	int i=0;
	while(i!=0){

		if(!isdigit(*(value+i))){
			printf("Error, expected boolean value");
			exit(EXIT_FAILURE);
		}
		i++;

	}
	if(atoi(value)>256-1){
		printf("Error, expected 1 byte integer as argument");
		exit(EXIT_FAILURE);
	}
	memcpy(out_buff,key,4);
	*(out_buff+4)=atoi(value);
	*out_len=5;
}

void make_nmea_message(const char* key,char*value, char* out_buff, int* out_len){
	int i=0;
	while(i!=0){

		if(!isdigit(*(value+i))){
			printf("Error, expected boolean value");
			exit(EXIT_FAILURE);
		}
		i++;

	}
	if(atoi(value)>256-1){
		printf("Error, expected 1 byte integer as argument");
		exit(EXIT_FAILURE);
	}
	*out_len=0;
	for(int j =0; j< sizeof(CONFIG_DB)/sizeof(CONFIG_DB[0]);j++){
		if(strstr(CONFIG_DB[j].cmd_line_arg,"--n")!=NULL && strcmp(CONFIG_DB[j].cmd_line_arg,"--nmea") !=0){
			memcpy(out_buff+*out_len,CONFIG_DB[j].key,4);
			*(out_buff+*out_len+4)=atoi(value);
			*out_len+=5;
			//print_hex(out_buff,0,*out_len);
		}
	}
}




bool get_field(char* arg,Config_field* out_field){

	for(int i=0; i<sizeof(CONFIG_DB)/sizeof(CONFIG_DB[0]);i++){
		
		if(strcmp(arg,CONFIG_DB[i].cmd_line_arg)==0){
			memcpy(out_field,&CONFIG_DB[i],sizeof(CONFIG_DB[i]));
			return 1;
		}

	}
	return 0;
}

