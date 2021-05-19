#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"connect.h"
// #define MOCK 1
#ifdef MOCK
    void read_n_bytes(Connection* connection_ptr,char* msg,int n){
        
        memcpy(msg,&(connection_ptr->mock_buffer[connection_ptr->position]),n);
        printf("%02hhx\n", *msg );
        connection_ptr->position+= n;
        
    }
    void close_connection(Connection connection){
        
    }
     Connection setup_connection() {
        Connection connection;
        unsigned char buf[34]= {181, 98, 1, 2, 28, 0, 10, 97, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
       
        memcpy(&connection.mock_buffer[0],&buf[0],34 );

        connection.position=0;
        return connection;
     }

#elif defined(__unix__)
    #include <fcntl.h> // Contains file controls like O_RDWR
    #include <errno.h> // Error integer and strerror() function
    #include <termios.h> // Contains POSIX terminal control definitions
    #include <unistd.h> // write(), read(), close()

    void read_n_bytes(Connection* connection,char* msg,int n) {

        char buf [1];
        for (int i=0;i<n;i++){
            read(connection->serial_port, buf, sizeof(buf));
            *(msg+i) = buf[0];
        }
    }

    void write_n_bytes(Connection* connection,char msg[],int len) {
    	write(connection->serial_port, msg, len);
    }

    void close_connection(Connection connection){
        close(connection.serial_port);
    }

    Connection setup_connection(char port[]) {
        Connection connection;
        char portname[20] = "/dev/";
        strcat(portname,port);

        int serial_port = open(portname, O_RDWR);
        if (serial_port < 0) { // Errors check
            printf("Error %i from open: %s\n", errno, strerror(errno));
            exit(0);
        }

        // Create new termios struct, we call it 'tty' for convention
        // No need for "= {0}" at the end as we'll immediately write the existing
        // config to this struct
        struct termios tty;

        // Read in existing settings, and handle any error
        // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
        // must have been initialized with a call to tcgetattr() overwise behaviour
        // is undefined
        if(tcgetattr(serial_port, &tty) != 0) {
            printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
            exit(0);
        }

        tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
        tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the statements below
        tty.c_cflag |= CS8; // 8 bits per byte (most common)
        tty.c_lflag &= ~ICANON; // Disable canonical mode --> bytewise input (hopefully)
        tty.c_lflag &= ~ECHO; // Disable echo
        tty.c_lflag &= ~ECHOE; // Disable erasure
        tty.c_lflag &= ~ECHONL; // Disable new-line echo
        tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
        tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
        tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
        tty.c_cc[VTIME] = 0; // No timeout=
        tty.c_cc[VMIN] = 1; // Wait until 1 bytes is received

        // Set in/out baud rate to be 9600
        cfsetispeed(&tty, B9600);
        cfsetospeed(&tty, B9600);

        if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
            printf("Error %i from tcsetattr: %s \n", errno, strerror(errno));
            exit(0);
        }
        connection.serial_port= serial_port;
        return connection;
    }
#elif defined(_WIN32) || defined(WIN32)
    #include<windows.h>

    Connection setup_connection(char port[]){
    HANDLE hComm;
	
	BOOL Write_Status;
	DCB dcbSerialParams;					// Initializing DCB structure
	COMMTIMEOUTS timeouts = { 0 };
	BOOL  Read_Status;                      // Status of the various operations 
	DWORD dwEventMask;
	
    char portname[20] = "\\\\.\\";
    strcat(portname,port);
    hComm = CreateFileA(portname,                //port name
                        GENERIC_READ | GENERIC_WRITE, //Read/Write
                        0,                            // No Sharing
                        NULL,                         // No Security
                        OPEN_EXISTING,// Open existing port only
                        0,            // Non Overlapped I/O
                        NULL);        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE)
        printf("Error in opening serial port");

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    Write_Status = GetCommState(hComm, &dcbSerialParams);     //retreives  the current settings

    if (Write_Status == FALSE) {
        printf("\n   Error! in GetCommState()");
        CloseHandle(hComm);
        Connection connection;
		connection.hComm=hComm;
		return connection;
    }


	dcbSerialParams.BaudRate = CBR_9600;      // Setting BaudRate = 9600
	dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
											  //dcbSerialParams.
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;      // Setting Parity = None

	Write_Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB

	if (Write_Status == FALSE)
	{
		printf("\n   Error! in Setting DCB Structure");
		CloseHandle(hComm);
		Connection connection;
		connection.hComm=hComm;
		return connection;
	}
	else
	{
		printf("\n   Setting DCB Structure Successful\n");
		printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
		printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
		printf("\n       StopBits = %d", dcbSerialParams.StopBits);
		printf("\n       Parity   = %d", dcbSerialParams.Parity);
		printf("\n\n");
	}

	// Set COM port timeout settings
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (SetCommTimeouts(hComm, &timeouts) == 0)
	{
		printf("Error setting timeouts\n");
		CloseHandle(hComm);
		Connection connection;
		connection.hComm=hComm;
		return connection;
	}
	Read_Status = SetCommMask(hComm, EV_RXCHAR);
	Read_Status = WaitCommEvent(hComm, &dwEventMask, NULL);
	Connection connection;
	connection.hComm=hComm;

    return connection;
    }

    void close_connection(Connection connection){

        CloseHandle(connection.hComm);//Closing the Serial Port
    }
    void read_n_bytes(Connection* connection, char* msg,int n){
        unsigned long received;
        if(!ReadFile(connection->hComm, msg, n, &received, NULL))
            printf("Error reading from serial port");

    }
#endif