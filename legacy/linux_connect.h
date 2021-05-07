// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

typedef struct Connection{
	int serial_port;
}Connection;

Connection setup_connection();
void close_connection(Connection connection);
void read_n_bytes(Connection connection, char* msg,int n);