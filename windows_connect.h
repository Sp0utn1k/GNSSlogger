#include<windows.h>
#include<stdio.h>
#include<string.h>
typedef struct Connection{
	HANDLE hComm;
}Connection;

Connection setup_connection();
void close_connection(Connection connection);
void read_n_bytes(Connection connection, char* msg,int n);
