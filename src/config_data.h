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

void make_bool_message(const char* key,char* value, char* out_buff, int* out_len);
void make_U2_message(const char* key,char*value, char* out_buff, int* out_len);
void make_U1_message(const char* key,char*value, char* out_buff, int* out_len);
void make_nmea_message(const char* key,char*value, char* out_buff, int* out_len);
bool get_field(char* arg,Config_field* out_field);



static const Config_field CONFIG_DB[]={
		{"--beidou","BOOL", "BeiDou Enable", make_bool_message, {0x22,0x00,0x31,0x10}},
		{"--qzss","BOOL","Apply QZSS SLAS DGNSS corrections",make_bool_message,{0x05,0x00,0x37,0x10}},
		{"--gps","BOOL","GPS enable",make_bool_message, {0x1f,0x00,0x31,0x10}},
		{"--gal","BOOL","Galileo enable",make_bool_message, {0x21,0x00,0x31,0X10}},
		{"--glonass","BOOL","GLONASS enable",make_bool_message,{0x25,0x00,0x31,0X10}},
		
		{"--rate_meas","PERIOD","Nominal time between GNSS measurements (16bits) in ms",make_U2_message,{0x01,0x00,0x21,0x30}},
		{"--rate_nav","RATIO","Ratio of number of measurements to number of navigation solutions (16bits)",make_U2_message,{0x02,0x00,0x21,0x30}},

		{"--ubx","BOOL", "Flag to indicate if UBX should be an output protocol on USB", make_bool_message, {0x01,0x00,0x78,0x10}},
		{"--nmea","BOOL", "Output rate of nmea messages on port USB (8bits) in Hz", make_nmea_message, {0x00,0x00,0x00,0x00}},
		//{"--nmea","BOOL", "Flag to indicate if NMEA should be an output protocol on USB", make_bool_message, {0x02,0x00,0x77,0x10}},
		{"--rtmc","BOOL","Flag to indicate if RTCM3X should be an output protocol on USB",make_bool_message,{0x04,0x00,0x78,0x10}},
		{"--posllh","RATE","Output rate of the UBX-NAV-POSLLH message on port USB (8bits) in Hz",make_U1_message,{0x2c,0x00,0x91,0x20}},
		{"--nDTM","RATE","Output rate of the NMEA-GX-DTM message on port USB (8bits) in Hz",make_U1_message,{0xa9,0x00,0x91,0x20}},
		{"--nGBS","RATE","Output rate of the NMEA-GX-GBS message on port USB (8bits) in Hz",make_U1_message,{0xe0,0x00,0x91,0x20}},
		{"--nGGA","RATE","Output rate of the NMEA-GX-GGA message on port USB (8bits) in Hz",make_U1_message,{0xbd,0x00,0x91,0x20}},
		{"--nGLL","RATE","Output rate of the NMEA-GX-GLL message on port USB (8bits) in Hz",make_U1_message,{0xcc,0x00,0x91,0x20}},
		{"--nGSA","RATE","Output rate of the NMEA-GX-GSA message on port USB (8bits) in Hz",make_U1_message,{0xc2,0x00,0x91,0x20}},
		{"--nGSV","RATE","Output rate of the NMEA-GX-GSV message on port USB (8bits) in Hz",make_U1_message,{0xc7,0x00,0x91,0x20}},
		{"--nVTG","RATE","Output rate of the NMEA-GX-VTG message on port USB (8bits) in Hz",make_U1_message,{0xb3,0x00,0x91,0x20}},
		{"--nZDA","RATE","Output rate of the NMEA-GX-ZDA message on port USB (8bits) in Hz",make_U1_message,{0xdb,0x00,0x91,0x20}}
		
	};


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

