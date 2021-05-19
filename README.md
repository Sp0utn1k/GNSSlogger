# GNSSlogger

Run the make command into the root directory to compile the program.
Then run ./GNSSLogger. A help section will be printed if no argument is provided.

It is almost necessary to provide the correct serial port in which the u-blox is plugged. In typical UNIX style, serial ports are represented by files within the operating system. These files usually pop-up in /dev/, and begin with the name tty*. 

Common names are:

- /dev/ttyACM0 : ACM stands for the ACM modem on the USB bus. Arduino UNOs (and similar) will appear using this name. The u-blox C100-F9K used on our machine for testing appeared under the name ACM0 or ACM1.
- /dev/ttyPS0 : Xilinx Zynq FPGAs running a Yocto-based Linux build will use this name for the default serial port that Getty connects to.
- /dev/ttyS0 : Standard COM ports will have this name. These are less common these days with newer desktops and laptops not having actual COM ports.
- /dev/ttyUSB0 : Most USB-to-serial cables will show up using a file named like this.
- /dev/pts/0 : A pseudo terminal. These can be generated with socat.

You should only provide the name inside the /dev folder. 
example : ./GNSSLogger -p ttyACM0

This also supports windows, the provided port should then be the used COM port.
