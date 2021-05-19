#include<stdio.h>
#include <string.h>
#include <stdbool.h>
#include<stdlib.h>

#ifdef __unix__
    
    #include <fcntl.h> // Contains file controls like O_RDWR
    #include <errno.h> // Error integer and strerror() function
    #include <termios.h> // Contains POSIX terminal control definitions
    #include <unistd.h> // write(), read(), close()

#elif defined(_WIN32) || defined(WIN32)
	#include<windows.h>

#endif

void print_hex(char msg[],int start,int len);
void save_cfg(Connection* connection);
bool verify_checksum(char msg[],int len,char checksum[]);
bool check_ack(Connection* connection);
void wrap_config(char* inwards,int in_len,char* msg,int* msg_len);