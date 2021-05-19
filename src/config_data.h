#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
typedef struct Config_field{
	const char cmd_line_arg[10];
	const char var_type[10];
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

static const Config_field CONFIG_DB[3]={
		{"--bd","Bool", "Use BeiDou geostationary satellites (PRN 1-5)", make_bool_message, {0x14,0x00,0x34,0x10}},
		{"--ubx","Bool", "Information message enable flags for the UBX protocol", make_bool_message, {0x04,0x00,0x92,0x20}},
		{"--nmea","Bool", "Information message enable flags for the NMEA protocol", make_bool_message, {0x09,0x00,0x92,0x20}}
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