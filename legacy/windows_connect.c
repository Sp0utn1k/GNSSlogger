#include<windows.h>
#include<stdio.h>
#include<string.h>
#include"windows_connect.h"

Connection setup_connection(){
    HANDLE hComm;
	
	BOOL Write_Status;
	DCB dcbSerialParams;					// Initializing DCB structure
	COMMTIMEOUTS timeouts = { 0 };
	BOOL  Read_Status;                      // Status of the various operations 
	DWORD dwEventMask;
	
    hComm = CreateFileA("\\\\.\\COM5",                //port name
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
void read_n_bytes(Connection connection, char* msg,int n){
	unsigned long received;
	if(!ReadFile(connection.hComm, msg, n, &received, NULL))
		printf("Error reading from serial port");

}
