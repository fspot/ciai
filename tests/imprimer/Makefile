all : testImp

testImp : mutex.o testImp.o
	g++ -o testImp mutex.o testImp.o -lpthread

mutex.o : ../../src/multithreadObjects/mutex.cpp ../../src/multithreadObjects/mutex.h
	g++ -c ../../src/multithreadObjects/mutex.cpp

testImp.o : testTacheImprimer.cpp ../../src/imprimer/imprimer.h ../../src/modeles/modeles.h ../../src/mailbox/mailbox.h
	g++ -c testTacheImprimer.cpp -o testImp.o

clean :
	rm -f testImp *.o *~