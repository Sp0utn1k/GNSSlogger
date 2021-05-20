#include<stdio.h>
#include <string.h>


void array_reverse(char* in_arr,int arr_len);



void print_hex(char msg[],int start,int len);
bool verify_checksum(char msg[],int len,char checksum[]);
void compute_checksum(char msg[],int len, char* CK_A,char* CK_B);
