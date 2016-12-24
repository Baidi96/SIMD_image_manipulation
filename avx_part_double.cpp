#include "immintrin.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <ctime>
using namespace std;
const int pic_width = 1920;
const int pic_height = 1080;
void convert2rgb(__m256d &r,__m256d &g,__m256d &b,__m256d y,__m256d u,__m256d v);
void convert2yuv(__m256d r,__m256d g,__m256d b,__m256d &y,__m256d &u,__m256d &v);
void YUV2ARGB2YUV(unsigned char* data, unsigned char *yuv_pic, int width, int height,int alpha);
int main(int argc, char* argv[])  
{  
	int char_num = (pic_width*pic_height*3)>>1;
	unsigned char* yuv_1 = new unsigned char[char_num];//source image
        	unsigned char* yuv_2 = new unsigned char[char_num];//dest image
	ifstream fin;
	fin.open("dem1.yuv");
	fin.read((char*)yuv_1, char_num);
	fin.close();
	clock_t start,end,start_tmp;
	int time = 0;
	ofstream fout;
	fout.open("trsd.yuv");
	
	for(int A=1;A<256;A=A+3)
        	{
	            start_tmp = clock();
	            YUV2ARGB2YUV(yuv_1,yuv_2,1920,1080,A);
	            end = clock();
	            time = (int)((end - start_tmp)/1000);
	            printf("time for loop %d is %d\n",(A-1)/3+1,time);
	            fout.write((char*)yuv_2,char_num);
        	}
//fout.write((char*)yuv_2,char_num);
	fout.close();
	YUV2ARGB2YUV(yuv_1, yuv_2, pic_width, pic_height,0);
	
	return 0;
}  
void convert2rgb(__m256d &r,__m256d &g,__m256d &b,__m256d y,__m256d u,__m256d v)
{
	__m256d vec_16 = _mm256_set1_pd(16.0);
	__m256d vec_128 = _mm256_set1_pd(128.0);
	__m256d vec_1_164 = _mm256_set1_pd(1.164383);

	y = _mm256_sub_pd(y,vec_16);
	v =_mm256_sub_pd(v,vec_128);
	u =_mm256_sub_pd(u,vec_128);

	r = _mm256_add_pd(_mm256_mul_pd (y,vec_1_164),_mm256_mul_pd(v,_mm256_set1_pd(1.596027)));
	b = _mm256_add_pd(_mm256_mul_pd(y,vec_1_164),_mm256_mul_pd(u, _mm256_set1_pd(2.017232)));
	g = _mm256_sub_pd(_mm256_mul_pd(y,vec_1_164),_mm256_mul_pd(u,_mm256_set1_pd(0.391762)));
	g = _mm256_sub_pd(g, _mm256_mul_pd(v,_mm256_set1_pd(0.812968)));

	r=_mm256_max_pd(r,_mm256_set1_pd(0.0));
	g=_mm256_max_pd(g,_mm256_set1_pd(0.0));
	b=_mm256_max_pd(b,_mm256_set1_pd(0.0));
	r=_mm256_min_pd(r,_mm256_set1_pd(255.0));
	g=_mm256_min_pd(g,_mm256_set1_pd(255.0));
	b=_mm256_min_pd(b,_mm256_set1_pd(255.0));

	return;
}
void convert2yuv(__m256d r,__m256d g,__m256d b,__m256d &y,__m256d &u,__m256d &v)
{
	__m256d tmp = _mm256_set1_pd(0.256788);
	y = _mm256_mul_pd(tmp,r);
	tmp = _mm256_set1_pd(0.504129);
	y = _mm256_add_pd(y,_mm256_mul_pd(tmp,g));
	tmp = _mm256_set1_pd(0.097906);
	y = _mm256_add_pd(y,_mm256_mul_pd(tmp,b));
	y = _mm256_add_pd(y,_mm256_set1_pd(16.0));

	tmp=_mm256_set1_pd(0.439216);
	u=_mm256_mul_pd(tmp,b);
	u=_mm256_add_pd(u,_mm256_set1_pd(128.0));
	u=_mm256_sub_pd(u,_mm256_mul_pd(g,_mm256_set1_pd(0.290993)));
	u=_mm256_sub_pd(u,_mm256_mul_pd(r,_mm256_set1_pd(0.148223)));

	v=_mm256_mul_pd(r, _mm256_set1_pd(0.439216));
	v=_mm256_sub_pd(v,_mm256_mul_pd(g,_mm256_set1_pd(0.367788)));
	v=_mm256_sub_pd(v,_mm256_mul_pd(b,_mm256_set1_pd(0.071427)));
	v=_mm256_add_pd(v,_mm256_set1_pd(128.0));
}

void convert(unsigned char *yuv_pic,__m256d y,__m256d u,__m256d v, int width,int height,int offset,int i, int k, int alpha)
{
		__m256d r, g, b;
		convert2rgb(r, g, b, y,u,v);

		/*
		R’=A*R/256
		G’=A*G/256
		b’=A*B/256
		*/
		r=_mm256_mul_pd(_mm256_set1_pd((double)alpha),_mm256_div_pd(r,_mm256_set1_pd(256.0)));
		g=_mm256_mul_pd(_mm256_set1_pd((double)alpha),_mm256_div_pd(g,_mm256_set1_pd(256.0)));
		b=_mm256_mul_pd(_mm256_set1_pd((double)alpha),_mm256_div_pd(b,_mm256_set1_pd(256.0)));

		convert2yuv(r,g,b,y,u,v);

		double result[4];
		 _mm256_storeu_pd (result,y);
		 yuv_pic[i] = (unsigned char)result[0];
		 yuv_pic[i+1] = (unsigned char)result[1];
		 yuv_pic[i+width] = (unsigned char)result[2];
		 yuv_pic[i+width+1] = (unsigned char)result[3];

		 _mm256_storeu_pd (result,u);
		 yuv_pic[offset+k]=(unsigned char)result[0];

		 _mm256_storeu_pd (result,v);
		 yuv_pic[offset+k+width*height/4]=(unsigned char)result[0];
}
void YUV2ARGB2YUV(unsigned char* data,unsigned char *yuv_pic, int width, int height,int alpha)
{
	int size = width*height;  
	int offset = size;
	for(int i=0, k=0; i < size; i+=2, k+=1) 
	{
		__m256d y = _mm256_set_pd((double)data[i],(double)data[i+1],(double)data[i+width],(double)data[i+width+1]);
	   	__m256d u =_mm256_set1_pd((double)data[offset+k]);
	   	__m256d v =_mm256_set1_pd((double)data[offset+size/4+k]);
		convert(yuv_pic, y, u, v, width,height, offset,i,k, alpha);
		if (i!=0 && (i+4)%width==0)  
		 	i+=width;  
	}
}
