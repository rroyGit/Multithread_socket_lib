#
#Target
all: driver

driver: fileLib.o socketLib.o threadLib.o driver.o
	gcc fileLib.o socketLib.o threadLib.o driver.o -lpthread -o driver

driver.o: driver.c
	gcc -c driver.c

fileLib.o: fileLib.c
	gcc -c fileLib.c
	
socketLib.o: socketLib.c
	gcc -c socketLib.c
	
threadLib.o: threadLib.c 
	gcc -c threadLib.c
	
clean:
	rm -f *.o driver