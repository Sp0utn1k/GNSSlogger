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
	char CK_A;
	char CK_B;
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
	printf("chksum: %02hhx %02hhx",*CK_A,*CK_B);
}