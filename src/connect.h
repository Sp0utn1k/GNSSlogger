#include<stdio.h>
#include<string.h>
// #define MOCK 1
#ifdef MOCK
    typedef struct Connection{
        unsigned char mock_buffer[34];
        int position;    
    } Connection;
#elif defined(__unix__)
    #include <fcntl.h> // Contains file controls like O_RDWR
    #include <errno.h> // Error integer and strerror() function
    #include <termios.h> // Contains POSIX terminal control definitions
    #include <unistd.h>
    typedef struct Connection{
        int serial_port;
    } Connection;
#elif defined(_WIN32) || defined(WIN32)
    #include<windows.h>
    typedef struct Connection{
        HANDLE hComm;
    }Connection ;
#endif

Connection setup_connection();
void close_connection(Connection connection);
void read_n_bytes(Connection* connection, char* msg,int n);