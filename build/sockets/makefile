CC = g++
CIBLE = exe
CFLAGS = -g
LIBS = -lpthread
DIR = ../../src/sockets/
CHEMINSHEAD = 
CHEMINSLIBS = 
OBJETS = parser.o server.o network.o

all : $(CIBLE)

$(CIBLE) : $(OBJETS)
	@echo "Linking..."
	$(CC) -o $(CIBLE) $(CFLAGS) $(CHEMINSLIBS) $(OBJETS) $(LIBS)
	@echo "DONE !"

parser.o : $(DIR)parser.c $(DIR)parser.h $(DIR)messages.h
	@echo "Compiling $@..."
	$(CC) -c $(CFLAGS) $(CHEMINSHEAD) $<

server.o : $(DIR)server.cpp
	@echo "Compiling $@..."
	$(CC) -c $(CFLAGS) $(CHEMINSHEAD) $<

network.o : $(DIR)network.c $(DIR)network.h $(DIR)messages.h
	@echo "Compiling $@..."
	$(CC) -c $(CFLAGS) $(CHEMINSHEAD) $<

clean :
	rm $(CIBLE) $(OBJETS)
