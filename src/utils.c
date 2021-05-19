#include<stdio.h>
#include <string.h>
#include <stdbool.h>

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
		for (int i = 2; i < len; i++) {
		CK_A = CK_A + msg[i];
		CK_B = CK_B + CK_A;
	}
	return CK_A==checksum[0] && CK_B==checksum[1];
}