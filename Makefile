CC=g++

all: q1
	rm -rf *.o *~

q1:
	$(CC) -o $@ main.cpp YUV2ARGB2YUV.h YUV2ARGB2YUV-sse2.h

avx:
	$(CC) -o $@ -mavx avx_part.cpp 

.PHONY: clean

clean:
	rm -rf q1* *.o *~
