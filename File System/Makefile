CFLAGS = -g -c -o

make :
	gcc -g -c FileSys.c
	gcc -g -c Driver.c
	gcc -g -c EasyTest.c
	gcc -g -c Driver.h
	gcc -g -c FileSysAPI.h
	gcc -g -o filesys FileSys.o Driver.o EasyTest.o

clean :
	$(RM) *~ , ~# , *.o , *.gch , filesys , CSCI460_data
