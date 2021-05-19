#include<stdio.h>
#include <string.h>
#include <stdbool.h>
#include"utils.h"
void print_hex(char msg[],int start,int len) {
	int i;
	for (i=start;i<len+start;i++) {
		printf("0x%02hhx ", msg[i]);
	}
	printf("\n");
}

bool verify_checksum(char msg[],int len,char checksum[]) {
	char CK_A = 0;
	char CK_B = 0;
	compute_checksum(msg, len, &CK_A, &CK_B);
	return CK_A==checksum[0] && CK_B==checksum[1];
}

void compute_checksum(char msg[],int len, char* CK_A,char* CK_B){
	*CK_B=0;
	*CK_A=0;
	for (int i = 2; i < len; i++) {
		*CK_A = *CK_A + msg[i];
		*CK_B = *CK_B + *CK_A;
	}
	
}

void array_reverse(char* in_arr,int arr_len){
	char buf[arr_len];
	for(int i=0; i<arr_len; i++){
		buf[i]=*(in_arr+arr_len-i);
	}
	memcpy(in_arr,buf,arr_len);
}