CC=cc
CFLAGS=-Wall
LDFLAGS=-lpthread
EXE_DETECT=detect
OBJ_DETECT=filecount.o sequence.o

$(EXE_DETECT): detect.c $(OBJ_DETECT) 
	$(CC) $(CFLAGS) -o $(EXE_DETECT) $< $(OBJ_DETECT) $(LDFLAGS)

%.o: %.c %.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.o $(EXE_DETECT) 
