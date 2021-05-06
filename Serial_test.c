// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()


// struct termios {
// 	tcflag_t c_iflag;		/* input mode flags */
// 	tcflag_t c_oflag;		/* output mode flags */
// 	tcflag_t c_cflag;		// control mode flags 
// 	tcflag_t c_lflag;		/* local mode flags */
// 	cc_t c_line;			/* line discipline */
// 	cc_t c_cc[NCCS];		/* control characters */
// };

struct NAV_POSLLH {
	unsigned char cls;
	unsigned char id;
	unsigned short len;
	unsigned long iTOW;
	long lon;
	long lat;
	long height;
	long hMSL;
	unsigned long hAcc;
	unsigned long vAcc;
};

void print_hex(char msg[],int start,int len) {
	int i;
	for (i=start;i<len+start;i++) {
		printf("0x%02hhx ", msg[i]);
	}
	printf("\n");
}

void read_n_bytes(int serial_port,char* msg,int n) {
	char buf [1];
	for (int i=0;i<n;i++){
		int n = read(serial_port, buf, sizeof(buf));
		*(msg+i) = buf[0];
	}
}

int main() {

	NAV_POSLLH posllh;

	int serial_port = open("/dev/ttyACM0", O_RDWR);
	if (serial_port < 0) { // Errors check
    	printf("Error %i from open: %s\n", errno, strerror(errno));
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
	}


	// Allocate memory for read buffer, set size according to your needs
	char read_buf [1];
	char msg[256];

	// Read bytes. The behaviour of read() (e.g. does it block?,
	// how long does it block for?) depends on the configuration
	// settings above, specifically VMIN and VTIME

	memset(msg,0,strlen(msg));
	// for (int i=0;i<10;i++){
	// 	int n = read(serial_port, &read_buf, sizeof(read_buf));
	// 	// printf("Received byte : %02hhx\n",read_buf[0]); // HEX format, adds 0 if needed
	// 	msg[i] = read_buf[0];
	// }

	// printf("Message : ");
	// print_hex(msg,10);

	int fpos = 0;

	read_n_bytes(serial_port,&msg[0],2); // read header
	if (!(msg[0] == (char) 0xb5 && msg[1] == (char) 0x62)) {
		printf("Wrong header\n");
		return 0;
	}
	read_n_bytes(serial_port,&msg[0],2);	
	if (!(msg[0] == (char) 0x01 && msg[1] == (char) 0x02)) {
		printf("Not posllh\n");
		return 0;
	}
	read_n_bytes(serial_port,&msg[2],2);	
	read_n_bytes(serial_port,&msg[3],(int) msg[2]);


	printf("Length : %d\n",(int) msg[2]);
	printf("Payload : ");
	print_hex(msg,3,(int) msg[2]);


	for (int i=0;i<4;i++) {
		((unsigned char*)(&posllh))[i+3] = c;
	}

	int TOW ((unsigned char*)(&posllh))[fpos-2] = c;
	for (int i=0;i<3;i++) {

	}
	// n is the number of bytes read. n may be 0 if no bytes were received, and can also be negative to signal an error.
}