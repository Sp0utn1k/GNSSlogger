all: GNSSLogger

GNSSLogger: src/config.c src/config.h src/config_data.h src/config_data.c src/connect.c src/connect.h src/the_almighty.c src/utils.c src/utils.h
	gcc -o GNSSLogger src/*.c

clean:
	del /s /q *.o

mrproper: clean
	del /s /q GNSSLogger