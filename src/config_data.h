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
	memcpy(out_buff,key,4);
	uint16_t dec_value=atoi(value);
	array_reverse((char*)&dec_value,2);
	memcpy(out_buff+4,&dec_value,2);
	*out_len=6;
}

static const Config_field CONFIG_DB[8]={
		{"--beidou","Bool", "BeiDou Enable", make_bool_message, {0x22,0x00,0x31,0x10}},
		{"--qzss","Bool","Apply QZSS SLAS DGNSS corrections",make_bool_message,{0x05,0x00,0x37,0x10}},
		{"--gps","Bool","GPS enable",make_bool_message, {0x1f,0x10,0x31,0x10}},
		
		{"--rate_meas","Period in ms","Nominal time between GNSS measurements",make_U2_message,{0x01,0x00,0x21,0x30}},
		{"--rate_nav","","Ratio of number of measurements to number of navigation solutions",make_U2_message,{0x02,0x00,0x21,0x30}},

		{"--ubx","Bool", "Flag to indicate if UBX should be an output protocol on USB", make_bool_message, {0x01,0x00,0x78,0x10}},
		{"--nmea","Bool", "Flag to indicate if NMEA should be an output protocol on USB", make_bool_message, {0x02,0x00,0x77,0x10}},
		{"--rtmc","Bool","Flag to indicate if RTCM3X should be an output protocol on USB",make_bool_message,{0x04,0x00,0x78,0x10}}
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


/*

struct Config_db{
	Config_field fields[10];
};

static const struct Config_db CONFIG_DB={
	.fields={
		{"bd","Use BeiDou geostationary satellites (PRN 1-5)",make_bool_message,{0x10,0x34,0x00,0x14}},
		{"ubx","Information message enable flags for the UBX protocol",make_bool_message,{0x20,0x92,0x00,0x04}},
		{"nmea","Information message enable flags for the NMEA protocol",make_bool_message,{0x20,0x92,0x00,0x09}}
	}
};
*/