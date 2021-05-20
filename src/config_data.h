#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <ctype.h>
#include<stdint.h>

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
		{"--nmea","RATE", "Output rate of all nmea messages on port USB (8bits) in Hz", make_nmea_message, {0x00,0x00,0x00,0x00}},
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


