make :
	gcc -g -c -std=c99 ule_scheduler.c
	gcc -g -c -std=c99 4bsd_scheduler.c
	gcc -g -c SchedSim.c
	gcc -g -c Dispatcher.h
	gcc -g -c SchedSim.h
	gcc -g -o ule ule_scheduler.o SchedSim.o -lm
	gcc -g -o 4bsd 4bsd_scheduler.o SchedSim.o -lm

clean :
	$(RM) *.o , *~ , ule , 4bsd , *.gch
