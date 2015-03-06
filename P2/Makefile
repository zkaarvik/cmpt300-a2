CFLAGS=
CC=gcc $(CFLAGS)
OBJECTS=sync.o main.o

mylocks: $(OBJECTS) 
	$(CC) $(OBJECTS) -lpthread -o mylocks
debug: $(OBJECTS) 
	$(CC) $(OBJECTS) -g -lpthread -o mylocks
clean:
	rm -f mylocks $(OBJECTS) 
run: mylocks
	./mylocks
