#include "immintrin.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
using namespace std;
const int pic_width = 1920;
const int pic_height = 1080;
__m256 vec_16 = _mm256_set1_ps(16.0);
__m256 vec_128 = _mm256_set1_ps(128.0);
__m256 vec_1_164 = _mm256_set1_ps(1.164383);
void convert2rgb(__m256 &r,__m256 &g,__m256 &b,__m256 y,__m256 u,__m256 v);
unsigned char* YUV2ARGB(unsigned char* yuv_file,int width,int height,int alpha);
void convert2yuv(__m256 r,__m256 g,__m256 b,__m256 &y,__m256 &u,__m256 &v);
unsigned char* ARGB2YUV(unsigned char* rgb_file,int width,int height);
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
	/*unsigned char* rgb_file=YUV2ARGB((unsigned char*)yuv_file,pic_width,pic_height,0);
	delete [] yuv_file; 
	for(int i = 0;i<100;i++)
		printf("%u ",(unsigned int)rgb_file[i]);
	printf("\n");
	yuv_file=ARGB2YUV((unsigned char*)rgb_file,pic_width,pic_height);*/
	YUV2ARGB2YUV(yuv_1, yuv_2, pic_width, pic_height,0);
	ofstream fout;
	fout.open("trs1.yuv");
	fout.write((char*)yuv_2,char_num);
	fout.close();
	return 0;
}  
void convert2rgb(__m256 &r,__m256 &g,__m256 &b,__m256 y,__m256 u,__m256 v)
{
	/*float result[8];
	_mm256_storeu_ps (result,y);
	printf("y=\n");
	for(int i = 0;i<8;i++)
		printf("%d ",(int)result[i]);
	_mm256_storeu_ps (result,u);
	printf("u=\n");
	for(int i = 0;i<8;i++)
		printf("%d ",(int)result[i]);
	_mm256_storeu_ps (result,v);
	printf("v=\n");
	for(int i = 0;i<8;i++)
		printf("%d ",(int)result[i]);*/

	y = _mm256_sub_ps(y,vec_16);
	v =_mm256_sub_ps(v,vec_128);
	u =_mm256_sub_ps(u,vec_128);
	__m256 vec_tmp =_mm256_set1_ps(1.596027);
	r = _mm256_add_ps(_mm256_mul_ps (y,vec_1_164),_mm256_mul_ps(v,vec_tmp));
	vec_tmp = _mm256_set1_ps(2.017232);
	b = _mm256_add_ps(_mm256_mul_ps(y,vec_1_164),_mm256_mul_ps(u,vec_tmp));
	vec_tmp = _mm256_set1_ps(0.391762);
	g = _mm256_sub_ps(_mm256_mul_ps(y,vec_1_164),_mm256_mul_ps(u,vec_tmp));
	vec_tmp = _mm256_set1_ps(0.812968);
	g = _mm256_sub_ps(g, _mm256_mul_ps(v,vec_tmp));

	r=_mm256_max_ps(r,_mm256_set1_ps(0));
	g=_mm256_max_ps(g,_mm256_set1_ps(0));
	b=_mm256_max_ps(b,_mm256_set1_ps(0));
	r=_mm256_min_ps(r,_mm256_set1_ps(255));
	g=_mm256_min_ps(g,_mm256_set1_ps(255));
	b=_mm256_min_ps(b,_mm256_set1_ps(255));
	//round down(cut off float part)
	r =_mm256_round_ps (r, (_MM_FROUND_TO_NEG_INF |_MM_FROUND_NO_EXC) );
	g =_mm256_round_ps (g, (_MM_FROUND_TO_NEG_INF |_MM_FROUND_NO_EXC) );
	b =_mm256_round_ps (b, (_MM_FROUND_TO_NEG_INF |_MM_FROUND_NO_EXC) );

	/*_mm256_storeu_ps (result,r);
	printf("r=\n");
	for(int i = 0;i<8;i++)
		printf("%d ",(int)result[i]);
	_mm256_storeu_ps (result,g);
	printf("g=\n");
	for(int i = 0;i<8;i++)
		printf("%d ",(int)result[i]);
	_mm256_storeu_ps (result,b);
	printf("b=\n");
	for(int i = 0;i<8;i++)
		printf("%d ",(int)result[i]);
	getchar();*/
	return;
}
void convert2yuv(__m256 r,__m256 g,__m256 b,__m256 &y,__m256 &u,__m256 &v)
{
	__m256 tmp = _mm256_set1_ps(0.256788);
	y = _mm256_mul_ps(tmp,r);
	tmp = _mm256_set1_ps(0.504129);
	y = _mm256_add_ps(y,_mm256_mul_ps(tmp,g));
	tmp = _mm256_set1_ps(0.097906);
	y = _mm256_add_ps(y,_mm256_mul_ps(tmp,b));
	y = _mm256_add_ps(y,_mm256_set1_ps(16.0));

	tmp=_mm256_set1_ps(0.439216);
	u=_mm256_mul_ps(tmp,b);
	u=_mm256_add_ps(u,_mm256_set1_ps(128.0));
	u=_mm256_sub_ps(u,_mm256_mul_ps(g,_mm256_set1_ps(0.290993)));
	u=_mm256_sub_ps(u,_mm256_mul_ps(r,_mm256_set1_ps(0.148223)));

	v=_mm256_mul_ps(r, _mm256_set1_ps(0.439216));
	v=_mm256_sub_ps(v,_mm256_mul_ps(g,_mm256_set1_ps(0.367788)));
	v=_mm256_sub_ps(v,_mm256_mul_ps(b,_mm256_set1_ps(0.071427)));
	v=_mm256_add_ps(v,_mm256_set1_ps(128.0));
}

void convert(unsigned char *yuv_pic,__m256 y,__m256 u,__m256 v, int width,int height,int offset,int i, int k, int alpha)
{
		__m256 r, g, b;
		convert2rgb(r, g, b, y,u,v);
		convert2yuv(r,g,b,y,u,v);

		float result[8];
		 _mm256_storeu_ps (result,y);
		 yuv_pic[i] = result[0];
		 yuv_pic[i+1] = result[1];
		 yuv_pic[i+width] = result[2];
		 yuv_pic[i+width+1] = result[3];
		 yuv_pic[i+2] = result[4];
		 yuv_pic[i+3] = result[5];
		 yuv_pic[i+2+width] = result[6];
		 yuv_pic[i+3+width] = result[7];

		 _mm256_storeu_ps (result,u);
		 yuv_pic[offset+k]=result[0];
		 yuv_pic[offset+k+1]=result[4];

		 _mm256_storeu_ps (result,v);
		 yuv_pic[offset+k+width*height/4]=result[0];
		 yuv_pic[offset+k+width*height/4+1]=result[4];
}
void YUV2ARGB2YUV(unsigned char* data,unsigned char *yuv_pic, int width, int height,int alpha)
{
	int size = width*height;  
	int offset = size;
	unsigned char u, v, y1, y2, y3, y4;
	for(int i=0, k=0; i < size; i+=4, k+=2) 
	{
		__m256 y = _mm256_set_ps((float)data[i],(float)data[i+1],(float)data[i+width],(float)data[i+width+1],(float)data[i+2],(float)data[i+3],(float)data[i+2+width],(float)data[i+3+width]);
	   	__m256 u =_mm256_set_ps((float)data[offset+k],(float)data[offset+k],(float)data[offset+k],(float)data[offset+k],(float)data[offset+k+1],(float)data[offset+k+1],(float)data[offset+k+1],(float)data[offset+k+1]);
	   	__m256 v =_mm256_set_ps((float)data[offset+size/4+k],(float)data[offset+size/4+k],(float)data[offset+size/4+k],(float)data[offset+size/4+k],(float)data[offset+size/4+k+1],(float)data[offset+size/4+k+1],(float)data[offset+size/4+k+1],(float)data[offset+size/4+k+1]);
		convert(yuv_pic, y, u, v, width,height, offset,i,k, alpha);
		if (i!=0 && (i+4)%width==0)  
		 	i+=width;  
	}
}
