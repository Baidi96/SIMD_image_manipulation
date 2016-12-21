CC=g++

all: q1
	rm -rf *.o *~

q1:
	$(CC) -msse2 -o $@ main.cpp YUV2ARGB2YUV.h

.PHONY: clean

clean:
	rm -rf q1 *.o *~
