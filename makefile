all: GNSSLogger

GNSSLogger: src/the_almighty.o src/connect.o
	gcc -o GNSSLogger src/the_almighty.o src/connect.o

src/the_almighty.o: src/the_almighty.c src/connect.h
	gcc -o src/the_almighty.o -c src/the_almighty.c -W -Wall

src/connect.o: src/connect.c
	gcc -o src/connect.o -c src/connect.c -W -Wall

clean:
	del /s /q *.o

mrproper: clean
	del /s /q GNSSLogger