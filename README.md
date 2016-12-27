# lab4.1

#show pictures
final lab~hhh
please first sudo apt-get install ffmpeg
you can use "ffplay -f rawvideo -video_size 640x360 test_input_640x360_bak.yuv" to show yuv file
or just download yuvplayer.

#q1
q1 is for lab4.1.
To run it: ./q1
Then input 1 to run image morphing, or input 2 to run image addition.
It runs in the order of no-simd, mmx, sse2, avx. After a certain run the time of millisecond will be seen.
