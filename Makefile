CC=g++

all: q1
	rm -rf *.o *~

q1:
	$(CC) -o $@ -O2 -mavx main.cpp YUV2ARGB2YUV.h YUV2ARGB2YUV-sse2.h YUV2ARGB2YUV-avx.h

.PHONY: clean

clean:
	rm -rf q1* *.o *~
