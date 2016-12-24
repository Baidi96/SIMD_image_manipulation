#include "immintrin.h"

struct AVX
{
	static void convert2rgb(__m256d &r,__m256d &g,__m256d &b,__m256d y,__m256d u,__m256d v)
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
	static void convert2yuv(__m256d r,__m256d g,__m256d b,__m256d &y,__m256d &u,__m256d &v)
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

	static void convert(unsigned char *yuv_pic,__m256d y,__m256d u,__m256d v, int width,int height,int offset,int i, int k, int alpha)
	{
			__m256d r, g, b;
			convert2rgb(r, g, b, y,u,v);
			//r=_mm256_mul_pd(_mm256_set1_pd((double)alpha),_mm256_div_pd(r,_mm256_set1_pd(256.0)));
			//g=_mm256_mul_pd(_mm256_set1_pd((double)alpha),_mm256_div_pd(g,_mm256_set1_pd(256.0)));
			//b=_mm256_mul_pd(_mm256_set1_pd((double)alpha),_mm256_div_pd(b,_mm256_set1_pd(256.0)));
			r=_mm256_mul_pd(_mm256_set1_pd((double)alpha/256.0),r);
			g=_mm256_mul_pd(_mm256_set1_pd((double)alpha/256.0),g);
			b=_mm256_mul_pd(_mm256_set1_pd((double)alpha/256.0),b);

			convert2yuv(r,g,b,y,u,v);

			double result[4];
			 _mm256_storeu_pd (result,y);
			 yuv_pic[i] = (unsigned char)result[0];
			 yuv_pic[i+1] = (unsigned char)result[1];
			 yuv_pic[i+width] = (unsigned char)result[2];
			 yuv_pic[i+width+1] = (unsigned char)result[3];

			 //yuv_pic[i] = (unsigned char)(((double*)&y2)[0]);
			 //_mm256_storeu_pd (result,u);
			 //yuv_pic[offset+k]=(unsigned char)result[0];
			 yuv_pic[offset+k] = (unsigned char)(((double*)&u)[0]);

			 //_mm256_storeu_pd (result,v);
			 //yuv_pic[offset+k+width*height/4]=(unsigned char)result[0];
			  yuv_pic[offset+k+width*height/4] = (unsigned char)(((double*)&v)[0]);
	}
	static void convert_add(unsigned char* yuv_pic,unsigned char* data,unsigned char* data2,__m256d y1,__m256d u1,__m256d v1,__m256d y2,__m256d u2,__m256d v2 ,int width,int height,int offset,int i, int k, int alpha)
	{
			__m256d r, g, b;
			convert2rgb(r, g, b, y1,u1,v1);
			/*r=_mm256_mul_pd(_mm256_set1_pd((double)alpha),r);
			g=_mm256_mul_pd(_mm256_set1_pd((double)alpha),g);
			b=_mm256_mul_pd(_mm256_set1_pd((double)alpha),b);*/

			__m256d tr, tg, tb;
			convert2rgb(tr, tg, tb, y2,u2,v2);

			r=_mm256_sub_pd(r,tr);
			g=_mm256_sub_pd(g,tg);
			b=_mm256_sub_pd(b,tb);

			r=_mm256_mul_pd(r,_mm256_set1_pd(((double)alpha/256.0)));
			g=_mm256_mul_pd(g,_mm256_set1_pd(((double)alpha)/256.0));
			b=_mm256_mul_pd(b,_mm256_set1_pd(((double)alpha)/256.0));

			r=_mm256_add_pd(r,tr);
			g=_mm256_add_pd(g,tg);
			b=_mm256_add_pd(b,tb);
			/*
			tr=_mm256_mul_pd(_mm256_set1_pd((double)(256.0-alpha)),tr);
			tg=_mm256_mul_pd(_mm256_set1_pd((double)(256.0-alpha)),tg);
			tb=_mm256_mul_pd(_mm256_set1_pd((double)(256.0-alpha)),tb);

			r=_mm256_add_pd(r,tr);
			g=_mm256_add_pd(g,tg);
			b=_mm256_add_pd(b,tb);

			r=_mm256_div_pd(r,_mm256_set1_pd(256.0));
			g=_mm256_div_pd(g,_mm256_set1_pd(256.0));
			b=_mm256_div_pd(b,_mm256_set1_pd(256.0));*/

			convert2yuv(r,g,b,y1,u1,v1);

			double result[4];
			 _mm256_storeu_pd (result,y1);
			 yuv_pic[i] = (unsigned char)result[0];
			 yuv_pic[i+1] = (unsigned char)result[1];
			 yuv_pic[i+width] = (unsigned char)result[2];
			 yuv_pic[i+width+1] = (unsigned char)result[3];

			/* _mm256_storeu_pd (result,u1);
			 yuv_pic[offset+k]=(unsigned char)result[0];

			 _mm256_storeu_pd (result,v1);
			 yuv_pic[offset+k+width*height/4]=(unsigned char)result[0];*/
			  yuv_pic[offset+k] = (unsigned char)(((double*)&u1)[0]);
			   yuv_pic[offset+k+width*height/4] = (unsigned char)(((double*)&v1)[0]);
	}
	static void YUV2ARGB2YUV(unsigned char* data,unsigned char *yuv_pic, int width, int height,int alpha)
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
	static void YUV2ARGB2YUV_add(unsigned char* data,unsigned char* data2,unsigned char *yuv_pic, int width, int height,int alpha)
	{
		int size = width*height;  
		int offset = size;
		for(int i=0, k=0; i < size; i+=2, k+=1) 
		{
			__m256d y1 = _mm256_set_pd((double)data[i],(double)data[i+1],(double)data[i+width],(double)data[i+width+1]);
		   	__m256d u1 =_mm256_set1_pd((double)data[offset+k]);
		   	__m256d v1 =_mm256_set1_pd((double)data[offset+size/4+k]);

		   	__m256d y2 = _mm256_set_pd((double)data2[i],(double)data2[i+1],(double)data2[i+width],(double)data2[i+width+1]);
		   	__m256d u2 =_mm256_set1_pd((double)data2[offset+k]);
		   	__m256d v2 =_mm256_set1_pd((double)data2[offset+size/4+k]);
			convert_add((unsigned char*)yuv_pic,(unsigned char*)data,(unsigned char*)data2, y1, u1, v1,y2,u2,v2, width,height, offset,i,k, alpha);
			if (i!=0 && (i+4)%width==0)  
			 	i+=width;  
		}
	}
};