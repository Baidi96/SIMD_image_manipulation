#include "immintrin.h"
#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;
const int pic_width = 1920;
const int pic_height = 1080;
const int tmps = 10;
__m256 vec_16 = _mm256_set1_ps(16);
__m256 vec_128 = _mm256_set1_ps(128);
__m256 vec_1_164 = _mm256_set1_ps(1.164383);
int main(int argc, char* argv[])  
{  
        float op1[8] = {1.1, 2.0, 3.0, 4.0,5.0,6.0,7.0,8.0};  
        float op2[8] = {1.3, 2.0, 3.0, 4.0,5.0,6.0,7.0,8.0};  
        float result[8];  
      //__m256d _mm256_load_pd (double const * mem_addr)
        __m256  a;  
        __m256  b;  
        __m256  c;  
        // Load
        a = _mm256_loadu_ps (op1) ;
        b = _mm256_loadu_ps (op2) ;
        // Calculate  
        c = _mm256_mul_ps (a, b);
        //c =_mm256_round_ps (c, (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC));
        // Store  
        _mm256_storeu_ps (result,c);
      for(int i =0;i<8;i++)
      	cout<<result[i]<<' ';
      for(int i =0;i<8;i++)
      		if(result[i]>0)result[i]=-1;
      		else if(result[i]<0)result[i]=2;
      	c = _mm256_loadu_ps (result) ;
      	 _mm256_storeu_ps (result,c);
      for(int i =0;i<8;i++)
      	cout<<result[i]<<' ';
        return 0;  
}  
void convert2rgb(__m256 &r,__m256 &g,__m256 &b,__m256 y,__m256 u,__m256 v)
{
	y = _mm256_sub_ps(y,vec_16);
	v =_mm256_sub_ps(v,vec_128);
	u =_mm256_sub_ps(u,vec_128);
	__m256 vec_tmp =_mm256_set1_ps(1.596027);
	r = _mm256_add_ps(_mm256_mul_ps (y,vec_1_164),_mm256_mul_ps(v,vec_tmp));
	vec_tmp = _mm256_set1_ps(2.017232);
	b = _mm256_add_ps(_mm256_mul_ps(y,vec_1_164),_mm256_mul_ps(u,vec_tmp));
	vec_tmp = _mm256_set1_ps(0.391762);
	g = _mm256_sub_ps(_mm256_mul_ps(vec_1_164,y),_mm256_mul_ps(u,vec_tmp));
	vec_tmp = _mm256_set1_ps(0.812968);
	g = _mm256_sub_ps(g, _mm256_mul_ps(v,vec_tmp));
	//round to nearest
	r =_mm256_round_ps (r, (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC));
	g =_mm256_round_ps (g, (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC));
	b =_mm256_round_ps (b, (_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC));
	return;
}
//1920*1080
char* YUV2ARGB(char* yuv_file,int width,int height,int alpha)
{
	char* rgb_file = new char[width*height*4];
	__m256  y,u,v;
        	int tmp = 0;
        	char* ystart;
	for(int i = 0 ;i<height;i+=2)
	for(int j = 0;j<width;j+=4)
	{
		ystart=yuv_file+i*width+j;
		//Y1 Y2 Y7 Y8 Y3 Y4 Y9 Y10
		y = _mm256_set_ps ((float)ystart[0],(float)*(ystart+1),(float)*(ystart+width), (float)*(ystart+1+width), (float)*(ystart+2), (float)*(ystart+2+width), (float)*(ystart+3), (float)*(ystart+3+width));
		float u1=(float)*(yuv_file+width*height+tmp);
		float u2=(float)*(yuv_file+width*height+tmp +1);
		float v1=(float)*(yuv_file+width*height+(width*height/4)+tmp);
		float v2=(float)*(yuv_file+width*height+(width*height/4)+tmp+1);
		u = _mm256_set_ps (u1,u1,u1,u1,u2,u2,u2,u2);
		v = _mm256_set_ps (v1,v1,v1,v1,v2,v2,v2,v2);
		__m256  r,g,b;
        		convert2rgb(r,g,b,y,u,v);

        		rgb_file[tmp*4] =rgb_file[(tmp+1)*4] =rgb_file[(tmp+width)*4] =rgb_file[(tmp+width+1)*4] =rgb_file[(tmp+2)*4] =rgb_file[(tmp+2+width)*4] =rgb_file[(tmp+3)*4] =rgb_file[(tmp+3+width)*4] = (unsigned char)alpha;
        		//check overflow and fill to rgb file
		 float result[8];
		 _mm256_storeu_ps (result,r);
	      	for(int k =0;k<8;k++)
	      	{
	      		if(result[k]>255)result[k]=255;
	      		else if(result[k]<0)result[k]=0;
	      		rgb_file[tmp*4+1] =rgb_file[(tmp+1)*4+1] =rgb_file[(tmp+width)*4+1] =rgb_file[(tmp+width+1)*4+1] =rgb_file[(tmp+2)*4+1] =rgb_file[(tmp+2+width)*4+1] =rgb_file[(tmp+3)*4+1] =rgb_file[(tmp+3+width)*4+1] = (unsigned char)result[k];
	      	}
	      	
	      	_mm256_storeu_ps (result,g);
	      	for(int k =0;k<8;k++)
	      	{
	      		if(result[k]>255)result[k]=255;
	      		else if(result[k]<0)result[k]=0;
	      		rgb_file[tmp*4+2] =rgb_file[(tmp+1)*4+2] =rgb_file[(tmp+width)*4+2] =rgb_file[(tmp+width+1)*4+2] =rgb_file[(tmp+2)*4+2] =rgb_file[(tmp+2+width)*4+2] =rgb_file[(tmp+3)*4+2] =rgb_file[(tmp+3+width)*4+2] = (unsigned char)result[k];
	      	}

	      	_mm256_storeu_ps (result,b);
	      	for(int k =0;k<8;k++)
	      	{
	      		if(result[k]>255)result[k]=255;
	      		else if(result[k]<0)result[k]=0;
	      		rgb_file[tmp*4+3] =rgb_file[(tmp+1)*4+3] =rgb_file[(tmp+width)*4+3] =rgb_file[(tmp+width+1)*4+3] =rgb_file[(tmp+2)*4+3] =rgb_file[(tmp+2+width)*4+3] =rgb_file[(tmp+3)*4+3] =rgb_file[(tmp+3+width)*4+3] = (unsigned char)result[k];
	      	}
		tmp+=2;
	}
	return rgb_file;
}
void convert2yuv(__m256 r,__m256 g,__m256 b,__m256 &y,__m256 &u,__m256 &v)
{
	y = _mm256_setzero_ps();
	__m256 tmp = _mm256_set1_ps(0.256788);
	y = _mm256_add_ps(y,_mm256_mul_ps(tmp,r));
	tmp = _mm256_set1_ps(0.504129);
	y = _mm256_add_ps(y,_mm256_mul_ps(tmp,g));
	tmp = _mm256_set1_ps(0.097906);
	y = _mm256_add_ps(y,_mm256_mul_ps(tmp,b));
	y = _mm256_add_ps(y,_mm256_set1_ps(16));

	u=_mm256_setzero_ps();
	tmp=_mm256_set1_ps(0.439216);
	u=_mm256_add_ps(u,_mm256_mul_ps(tmp,b));
	u=_mm256_add_ps(u,_mm256_set1_ps(128));
	u=_mm256_sub_ps(u,_mm256_mul_ps(g,_mm256_set1_ps(0.290993)));
	u=_mm256_sub_ps(u,_mm256_mul_ps(r,_mm256_set1_ps(0.148223)));
	v=_mm256_mul_ps(r, _mm256_set1_ps(0.439216));
	v=_mm256_sub_ps(v,_mm256_mul_ps(g,_mm256_set1_ps(0.367788)));
	v=_mm256_sub_ps(v,_mm256_mul_ps(b,_mm256_set1_ps(0.071427)));
	v=_mm256_add_ps(v,_mm256_set1_ps(128));
}
char* ARGB2YUV(char* rgb_file,int width,int height)
{
	char* yuv_file = new char[width*height*3];
        	__m256  r,g,b;
        	int tmp = 0;
        	char* ystart;
        	char* ustart = yuv_file+pic_height*pic_width;
	for(int i = 0 ;i<pic_height;i+=2)
	for(int j = 0;j<pic_width;j+=4)
	{
		ystart = yuv_file+i*width;
		r = _mm256_set_ps((float)rgb_file[tmp*4+1],(float)rgb_file[(tmp+1)*4+1],(float)rgb_file[(tmp+width)*4+1],(float)rgb_file[(tmp+width+1)*4+1],(float)rgb_file[(tmp+2)*4+1],(float)rgb_file[(tmp+2+width)*4+1],(float)rgb_file[(tmp+3)*4+1],(float)rgb_file[(tmp+3+width)*4+1]);
		g = _mm256_set_ps((float)rgb_file[tmp*4+2],(float)rgb_file[(tmp+1)*4+2],(float)rgb_file[(tmp+width)*4+2],(float)rgb_file[(tmp+width+1)*4+2],(float)rgb_file[(tmp+2)*4+2],(float)rgb_file[(tmp+2+width)*4+2],(float)rgb_file[(tmp+3)*4+2],(float)rgb_file[(tmp+3+width)*4+2]);
		b = _mm256_set_ps((float)rgb_file[tmp*4+3],(float)rgb_file[(tmp+1)*4+3],(float)rgb_file[(tmp+width)*4+3],(float)rgb_file[(tmp+width+1)*4+3],(float)rgb_file[(tmp+2)*4+3],(float)rgb_file[(tmp+2+width)*4+3],(float)rgb_file[(tmp+3)*4+3],(float)rgb_file[(tmp+3+width)*4+3]);
        		__m256  y,u,v;  
        		convert2yuv(r,g,b,y,u,v);
        		//store the res
        		float result[8];
		 _mm256_storeu_ps (result,y);
		 ystart[0]=(char)result[0];
		 ystart[1]=(char)result[1];
		 ystart[width]=(char)result[2];
		 ystart[1+width]=(char)result[3];
		 ystart[2]=(char)result[4];
		 ystart[3]=(char)result[5];
		 ystart[2+width]=(char)result[6];
		 ystart[3+width]=(char)result[7];

		 _mm256_storeu_ps (result,u);
		 cout<<"all u result"<<endl;
		 for(int k=0;k<8;k++)
		 	cout<<(char)result[k]<<endl;
		 ustart[0]=(char)result[0];
		 ustart[1]=(char)result[4];

		 char* vstart = ustart+(width*height)/4;
		 _mm256_storeu_ps (result,v);
		 cout<<"all v result"<<endl;
		 for(int k=0;k<8;k++)
		 	cout<<(char)result[k]<<endl;
		 vstart[0]=(char)result[0];
		 vstart[1]=(char)result[4];
		tmp+=2;
		ustart+=2;
	}
}