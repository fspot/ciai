CC = g++
CIBLE = a.exe
CFLAGS = -g
LIBS = -lpthread
DIR = ../../src/sockets/
MBDIR = ../../src/mailbox/
H_MODELES = ../../src/modeles/modeles.h
CHEMINSHEAD = 
CHEMINSLIBS = 
OBJETS = parser.o server.o network.o netsend.o

all : $(CIBLE)

$(CIBLE) : $(OBJETS)
	@echo "Linking..."
	$(CC) -o $(CIBLE) $(CFLAGS) $(CHEMINSLIBS) $(OBJETS) $(LIBS)
	@echo "DONE !"

parser.o : $(DIR)parser.cpp $(DIR)parser.h $(H_MODELES)
	@echo "Compiling $@..."
	$(CC) -c $(CFLAGS) $(CHEMINSHEAD) $<

server.o : $(DIR)server.cpp $(MBDIR)mailbox.h
	@echo "Compiling $@..."
	$(CC) -c $(CFLAGS) $(CHEMINSHEAD) $<

network.o : $(DIR)network.cpp $(DIR)network.h $(H_MODELES) $(MBDIR)mailbox.h
	@echo "Compiling $@..."
	$(CC) -c $(CFLAGS) $(CHEMINSHEAD) $<

netsend.o : $(DIR)netsend.cpp $(DIR)netsend.h $(MBDIR)mailbox.h
	@echo "Compiling $@..."
	$(CC) -c $(CFLAGS) $(CHEMINSHEAD) $<


clean :
	rm $(CIBLE) $(OBJETS)
