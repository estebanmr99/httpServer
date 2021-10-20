SOURCEC = main.c IPFinder.c Server.c HTTPRequest.c Queue.c

TARGETC = server

.PHONY: all

all: c

c:
	$(CC) $(SOURCEC) -o $(TARGETC) -lpthread -lm

clean:
	-rm -f *.o
	#-rm -f *.txt
	-rm -f $(TARGETC)