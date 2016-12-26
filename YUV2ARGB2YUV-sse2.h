#include <emmintrin.h>
#include <stdio.h>

#define ARG_TYPE_SSE2 __m128d

struct SSE2
{
	// __m128d argument: 2 * double
	static void yuv2rgb(ARG_TYPE_SSE2 y, ARG_TYPE_SSE2 u, ARG_TYPE_SSE2 v, ARG_TYPE_SSE2 &r, ARG_TYPE_SSE2 &g, ARG_TYPE_SSE2 &b)
	{
		/*
		r = 1.164383 * (y - 16) + 1.596027*(v - 128);
		b = 1.164383 * (y - 16) + 2.017232*(u - 128);
		g = 1.164383 * (y - 16) - 0.391762*(u - 128) - 0.812968*(v - 128);
		*/

		ARG_TYPE_SSE2 y1 = _mm_sub_pd(y, _mm_set1_pd(16.0));
		ARG_TYPE_SSE2 u1 = _mm_sub_pd(u, _mm_set1_pd(128.0));
		ARG_TYPE_SSE2 v1 = _mm_sub_pd(v, _mm_set1_pd(128.0));
		r = _mm_mul_pd(y1, _mm_set1_pd(1.164383));
		r = _mm_add_pd(r, _mm_mul_pd(v1, _mm_set1_pd(1.596027)));
		b = _mm_mul_pd(y1, _mm_set1_pd(1.164383));
		b = _mm_add_pd(b, _mm_mul_pd(u1, _mm_set1_pd(2.017232)));
		g = _mm_mul_pd(y1, _mm_set1_pd(1.164383));
		g = _mm_sub_pd(g, _mm_mul_pd(u1, _mm_set1_pd(0.391762)));
		g = _mm_sub_pd(g, _mm_mul_pd(v1, _mm_set1_pd(0.812968)));
		
		/*
		r = r>255 ? 255 : r<0 ? 0 : r;
		g = g>255 ? 255 : g<0 ? 0 : g;
		b = b>255 ? 255 : b<0 ? 0 : b;
		*/
		r = _mm_min_pd(_mm_set1_pd(255.0), _mm_max_pd(_mm_set1_pd(0.0), r));
		g = _mm_min_pd(_mm_set1_pd(255.0), _mm_max_pd(_mm_set1_pd(0.0), g));
		b = _mm_min_pd(_mm_set1_pd(255.0), _mm_max_pd(_mm_set1_pd(0.0), b));
	}

	static void rgb2yuv(ARG_TYPE_SSE2 r, ARG_TYPE_SSE2 g, ARG_TYPE_SSE2 b, ARG_TYPE_SSE2 &y, ARG_TYPE_SSE2 &u, ARG_TYPE_SSE2 &v)
	{
		/*
		y = 0.256788*r + 0.504129*g + 0.097906*b + 16;
		u = -0.148223*r - 0.290993*g + 0.439216*b + 128;
		v = 0.439216*r - 0.367788*g - 0.071427*b + 128;
		*/
		ARG_TYPE_SSE2 y0 = _mm_add_pd(_mm_mul_pd(_mm_set1_pd(0.256788), r), _mm_mul_pd(_mm_set1_pd(0.504129), g));
		ARG_TYPE_SSE2 y1 = _mm_add_pd(_mm_mul_pd(_mm_set1_pd(0.097906), b), _mm_set1_pd(16.0));
		y = _mm_add_pd(y0, y1);
		ARG_TYPE_SSE2 u0 = _mm_add_pd(_mm_mul_pd(_mm_set1_pd(-0.148223), r), _mm_mul_pd(_mm_set1_pd(-0.290993), g));
		ARG_TYPE_SSE2 u1 = _mm_add_pd(_mm_mul_pd(_mm_set1_pd(0.439216), b), _mm_set1_pd(128.0));
		u = _mm_add_pd(u0, u1);
		ARG_TYPE_SSE2 v0 = _mm_add_pd(_mm_mul_pd(_mm_set1_pd(0.439216), r), _mm_mul_pd(_mm_set1_pd(-0.367788), g));
		ARG_TYPE_SSE2 v1 = _mm_add_pd(_mm_mul_pd(_mm_set1_pd(-0.071427), b), _mm_set1_pd(128.0));
		v = _mm_add_pd(v0, v1);
		
		// printf("y = %d, u = %d, v = %d\n", y, u, v);
	}

	static void convert(char *yuv_pic, ARG_TYPE_SSE2 y, ARG_TYPE_SSE2 u, ARG_TYPE_SSE2 v, int width, int height,int offset,int i, int k, int alpha, int cnt) {
		ARG_TYPE_SSE2 r, g, b;
		yuv2rgb(y, u, v, r, g, b);
		
		double fac = alpha / 256.0;
		r = _mm_mul_pd(r, _mm_set1_pd(fac));
		g = _mm_mul_pd(g, _mm_set1_pd(fac));
		b = _mm_mul_pd(b, _mm_set1_pd(fac));

		ARG_TYPE_SSE2 y2, u2, v2;
		rgb2yuv(r, g, b, y2, u2, v2);
		
		yuv_pic[i] = (unsigned char)(((double*)&y2)[0]);
		yuv_pic[i+1] = (unsigned char)(((double*)&y2)[1]);
		//4 y blocks set one u and v
		if(cnt == 4)
		{
		    yuv_pic[offset + k] = (unsigned char)(((double*)&u2)[0]);
		    yuv_pic[offset + k + width*height/4] = (unsigned char)(((double*)&v2)[0]);
		}
	}
	
	static void convert_add(char *yuv_pic, ARG_TYPE_SSE2 y, ARG_TYPE_SSE2 y_, ARG_TYPE_SSE2 u, ARG_TYPE_SSE2 u_, ARG_TYPE_SSE2 v, ARG_TYPE_SSE2 v_, int width, int height,int offset,int i, int k, int alpha, int cnt) {
		ARG_TYPE_SSE2 r,g,b,r_,g_,b_;
		yuv2rgb(y, u, v, r, g, b);
		yuv2rgb(y_, u_, v_, r_, g_, b_);
		/*
		r=(alpha*r + (256-alpha)*r_)/256;
		g=(alpha*g + (256-alpha)*g_)/256;
		b=(alpha*b + (256-alpha)*b_)/256;
		*/
		
		double fac1 = alpha / 256.0, fac2 = (256 - alpha) / 256.0;
		ARG_TYPE_SSE2 r0 = _mm_mul_pd(_mm_set1_pd(fac1), r);
		ARG_TYPE_SSE2 r1 = _mm_mul_pd(_mm_set1_pd(fac2), r_);
		r = _mm_add_pd(r0, r1);
		ARG_TYPE_SSE2 g0 = _mm_mul_pd(_mm_set1_pd(fac1), g);
		ARG_TYPE_SSE2 g1 = _mm_mul_pd(_mm_set1_pd(fac2), g_);
		g = _mm_add_pd(g0, g1);
		ARG_TYPE_SSE2 b0 = _mm_mul_pd(_mm_set1_pd(fac1), b);
		ARG_TYPE_SSE2 b1 = _mm_mul_pd(_mm_set1_pd(fac2), b_);
		b = _mm_add_pd(b0, b1);
		
		ARG_TYPE_SSE2 y2, u2, v2;
		rgb2yuv(r, g, b, y2, u2, v2);
		
		yuv_pic[i] = (unsigned char)(((double*)&y2)[0]);
		yuv_pic[i+1] = (unsigned char)(((double*)&y2)[1]);
		//4 y blocks set one u and v
		if(cnt == 4)
		{
		    yuv_pic[offset + k] = (unsigned char)(((double*)&u2)[0]);
		    yuv_pic[offset + k + width*height/4] = (unsigned char)(((double*)&v2)[0]);
		}
	}

	static void YUV2ARGB2YUV(char* data, char *yuv_pic, int width, int height,int alpha) {
		int size = width*height;  
		int offset = size;
		unsigned char u0, v0, y01, y02, y11, y12;
		ARG_TYPE_SSE2 y0, y1, u, v;
		for(int i=0, k=0; i < size; i+=2, k+=1) {
		    //printf("%d\n",i);
		    y01 = data[i];
		    y02 = data[i+1];
		    y11 = data[width+i];
		    y12 = data[width+i+1];
	   
		    u0 = data[offset+k];
		    v0 = data[offset+size/4+k];
		    
		    y0 = _mm_set_pd((double)y01, (double)y02);
		    y1 = _mm_set_pd((double)y11, (double)y12);
		    u = _mm_set1_pd((double)u0);
		    v = _mm_set1_pd((double)v0);
		    /*
		    convert(yuv_pic, y1, u, v, width,height, offset,i,k, alpha,1);
		    convert(yuv_pic, y2, u, v, width,height, offset,i+1,k, alpha,2);
		    convert(yuv_pic, y3, u, v, width,height, offset,width+i,k, alpha,3);
		    convert(yuv_pic, y4, u, v, width,height, offset,width+i+1,k, alpha,4);
		    */
		    
		    convert(yuv_pic, y0, u, v, width, height, offset, i, k, alpha, 2);
		    convert(yuv_pic, y1, u, v, width, height, offset, width+i+1, k, alpha, 4);
		    
		    if (i!=0 && (i+2)%width==0)  
		        i+=width;  
		}
	}  

	
	static void YUV2ARGB2YUV_add(char* data, char* data2,char *yuv_pic, int width, int height,int alpha) {
		int size = width*height;
		int offset = size;
		unsigned char u0, v0, u0_, v0_, y01, y02, y11, y12, y01_, y02_, y11_, y12_ ;
		ARG_TYPE_SSE2 u, u_, v, v_, y0, y0_, y1, y1_;
		for(int i=0, k=0; i < size; i+=2, k+=1) {
		    //printf("%d\n",i);
		    y01 = data[i];
		    y02 = data[i+1];
		    y11 = data[width+i];
		    y12 = data[width+i+1];
		    
		    u0 = data[offset+k];
		    v0 = data[offset+size/4+k];
		    
		    y01_ = data2[i];
		    y02_ = data2[i+1];
		    y11_ = data2[width+i];
		    y12_ = data2[width+i+1];
		    
		    u0_ = data2[offset+k];
		    v0_ = data2[offset+size/4+k];
		    
		    y0 = _mm_set_pd((double)y01, (double)y02);
		    y1 = _mm_set_pd((double)y11, (double)y12);
		    u = _mm_set1_pd((double)u0);
		    v = _mm_set1_pd((double)v0);
		    y0_ = _mm_set_pd((double)y01_, (double)y02_);
		    y1_ = _mm_set_pd((double)y11_, (double)y12_);
		    u_ = _mm_set1_pd((double)u0_);
		    v_ = _mm_set1_pd((double)v0_);
		    
		    /*
		    convert_add(yuv_pic, y1, y1_, u, u_, v, v_, width,height, offset,i,k, alpha,1);
		    convert_add(yuv_pic, y2, y2_, u, u_, v, v_, width,height, offset,i+1,k, alpha,2);
		    convert_add(yuv_pic, y3, y3_, u, u_, v, v_, width,height, offset,width+i,k, alpha,3);
		    convert_add(yuv_pic, y4, y4_, u, u_, v, v_, width,height, offset,width+i+1,k, alpha,4);
		    */
		    
		    convert_add(yuv_pic, y0, y0_, u, u_, v, v_, width, height, offset, i, k, alpha, 2);
		    convert_add(yuv_pic, y1, y1_, u, u_, v, v_, width, height, offset, width+i, k, alpha, 4);
		    
		    if (i!=0 && (i+2)%width==0)
		        i+=width;
		}
	}
	
};

