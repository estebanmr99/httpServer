SOURCEC = main.c IPFinder.c Server.c HTTPRequest.c

TARGETC = server

.PHONY: all

all: c

c:
	$(CC) $(SOURCEC) -o $(TARGETC) 

clean:
	-rm -f *.o
	#-rm -f *.txt
	-rm -f $(TARGETC)