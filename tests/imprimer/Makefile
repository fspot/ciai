all : testImp

testImp : mutex.o log.o imprimer.o stubs.o testImp.o 
	g++ -o testImp mutex.o log.o imprimer.o stubs.o testImp.o -lpthread

mutex.o : ../../src/multithreadObjects/mutex.cpp ../../src/multithreadObjects/mutex.h
	g++ -c ../../src/multithreadObjects/mutex.cpp -o mutex.o

log.o : ../../src/log/log.h ../../src/log/log.cpp
	g++ -c ../../src/log/log.cpp -o log.o

imprimer.o : ../../src/imprimer/imprimer.cpp ../../src/imprimer/imprimer.h ../../src/modeles/modeles.h ../../src/mailbox/mailbox.h ../../src/log/log.h ../../src/stubs/stubs.h
	g++ -c ../../src/imprimer/imprimer.cpp -o imprimer.o -I../../src

stubs.o : ../../src/stubs/stubs.cpp ../../src/stubs/stubs.h
	g++ -c ../../src/stubs/stubs.cpp -o stubs.o -I../../src

testImp.o : testTacheImprimer.cpp ../../src/imprimer/imprimer.h ../../src/modeles/modeles.h ../../src/mailbox/mailbox.h ../../src/log/log.h
	g++ -c testTacheImprimer.cpp -o testImp.o -I../../src

clean :
	rm -f testImp *.o *~ log.txt
