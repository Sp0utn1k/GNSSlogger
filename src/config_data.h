#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <ctype.h>
#include<stdint.h>
#include"utils.h"
typedef struct Config_field{
	const char cmd_line_arg[20];
	const char var_type[20];
	const char description[256];
	const void (*make_message)(const char*,char*, char*, int*);
	const char key[4];
}Config_field;

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


static const Config_field CONFIG_DB[10]={
		{"--beidou","BOOL", "BeiDou Enable", make_bool_message, {0x22,0x00,0x31,0x10}},
		{"--qzss","BOOL","Apply QZSS SLAS DGNSS corrections",make_bool_message,{0x05,0x00,0x37,0x10}},
		{"--gps","BOOL","GPS enable",make_bool_message, {0x1f,0x10,0x31,0x10}},
		{"--gal","BOOL","Galileo enable",make_bool_message, {0x21,0x00,0x31,0X10}},
		
		{"--rate_meas","PERIOD","Nominal time between GNSS measurements (16bits) in ms",make_U2_message,{0x01,0x00,0x21,0x30}},
		{"--rate_nav","RATIO","Ratio of number of measurements to number of navigation solutions (16bits)",make_U2_message,{0x02,0x00,0x21,0x30}},

		{"--ubx","BOOL", "Flag to indicate if UBX should be an output protocol on USB", make_bool_message, {0x01,0x00,0x78,0x10}},
		{"--nmea","BOOL", "Flag to indicate if NMEA should be an output protocol on USB", make_bool_message, {0x02,0x00,0x77,0x10}},
		{"--rtmc","BOOL","Flag to indicate if RTCM3X should be an output protocol on USB",make_bool_message,{0x04,0x00,0x78,0x10}},
		{"--posllh","RATE","Output rate of the UBX-NAV-POSLLH message on port USB (8bits) in Hz",make_U1_message,{0x2c,0x00,0x91,0x20}}
	};


bool get_field(char* arg,Config_field* out_field){

	for(int i=0; i<sizeof(CONFIG_DB)/sizeof(CONFIG_DB[0]);i++){
		
		if(strcmp(arg,CONFIG_DB[i].cmd_line_arg)==0){
			memcpy(out_field,&CONFIG_DB[i],sizeof(CONFIG_DB[i]));
			return 1;
		}

	}
	return 0;
}

