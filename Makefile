HEADERS := bitmap.h

all: build

build: bitmap

main.o: main.cpp $(HEADERS)
	g++ -o $@ -c $<

bitmap.o: bitmap.cpp $(HEADERS)
	g++ -o $@ -c $<

bitmap: main.o bitmap.o 
	g++ -o $@ $+ 

clean:
	rm -f bitmap *.o
