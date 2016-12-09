make :
	gcc -g -c lfuPageReplacer.c
	gcc -g -c lruPageReplacer.c
	gcc -g -c MemSim.c
	gcc -g -c MemSim.h
	gcc -g -c Assg3.h
	gcc -g -o lfu lfuPageReplacer.o MemSim.o
	gcc -g -o lru lruPageReplacer.o MemSim.o

clean :
	$(RM) *.o , *~ , *.gch , *# , lfu , lru
