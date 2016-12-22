#include <emmintrin.h>
#include <stdio.h>

#define ARG_TYPE_SSE2 int

struct SSE2
{
	static void yuv2rgb(ARG_TYPE_SSE2 y, ARG_TYPE_SSE2 u, ARG_TYPE_SSE2 v, ARG_TYPE_SSE2 &r, ARG_TYPE_SSE2 &g, ARG_TYPE_SSE2 &b)
	{
	
		r = (298 * (y - 16) + 409 * (v - 128)) / 256;
		b = (298 * (y - 16) + 520 * (u - 128)) / 256;
		g = (298 * (y - 16) - 100 * (u - 128) - 208 * (v - 128)) / 256;
		/*
		__m128i t0i = _mm_set_epi16(-16, -128, -16, -128, -16, -128, -128, 0);
		__m128i t1i = _mm_set_epi16(298, 409, 298, 520, 298, 100, 208, 0);
		
		__m128i t0v = _mm_set_epi16(y, v, y, u, y, u, v, 0);
		
		__m128i h0 = _mm_adds_epi16(t0i, t0v);
		__m128i h1 = _mm_mulhi_epu16(h0, t1i);
		short dst[8];
		_mm_store_si128((__m128i*)dst, h1);
		r = dst[0] + dst[1];
		b = dst[2] + dst[3];
		g = dst[4] - dst[5] - dst[6];
		*/
		r = r>255 ? 255 : r<0 ? 0 : r;
		g = g>255 ? 255 : g<0 ? 0 : g;
		b = b>255 ? 255 : b<0 ? 0 : b;
		// printf("r = %d, g = %d, b = %d\n", r, g, b);
	}

	static void rgb2yuv(ARG_TYPE_SSE2 r, ARG_TYPE_SSE2 g, ARG_TYPE_SSE2 b, ARG_TYPE_SSE2 &y, ARG_TYPE_SSE2 &u, ARG_TYPE_SSE2 &v)
	{
		y = (66 * r + 129 * g + 25 * b) / 256 + 16;
		u = (-38 * r - 74 * g + 112 * b) / 256 + 128;
		v = (112 * r - 94 * g - 18 * b) / 256 + 128;
		/*
		__m128i t0i = _mm_set_epi16(66, 129, 25, 38, 74, 112, 112, 94);
		__m128i t0v = _mm_set_epi16(r, g, b, r, g, b, r, g);

		__m128i h1 = _mm_mulhi_epu16(t0i, t0v);
		short dst[9];
		_mm_store_si128((__m128i*)dst, h1);
		dst[8] = 18 * b / 256;
		
		y = dst[0] + dst[1] + dst[2] + 16;
		u = -dst[3] - dst[4] + dst[5] + 128;
		v = dst[6] - dst[7] - dst[8] + 128;
		*/
		// printf("y = %d, u = %d, v = %d\n", y, u, v);
	}

	static void convert(char *yuv_pic, int y, int u, int v, int width, int height,int offset,int i, int k, int alpha, int cnt) {
		ARG_TYPE_SSE2 r, g, b;
		yuv2rgb(y, u, v, r, g, b);
		
		r=(alpha*r)/256;
		g=(alpha*g)/256;
		b=(alpha*b)/256;

		ARG_TYPE_SSE2 y2, u2, v2;
		rgb2yuv(r, g, b, y2, u2, v2);
		
		yuv_pic[i] = y2;
		//4 y blocks set one u and v
		if(cnt == 4) {
		    yuv_pic[offset + k] = u2;
		    yuv_pic[offset + k + width*height/4] = v2;
		}
	}
	/*
	static void convert_add(char *yuv_pic,unsigned char y,unsigned char y_,unsigned char u,unsigned char u_,unsigned char v,unsigned char v_, int width, int height,int offset,int i, int k, int alpha, int cnt) {
		int r,g,b,r_,g_,b_;
		yuv2rgb(y, u, v, r, g, b);
		yuv2rgb(y_, u_, v_, r_, g_, b_);
		
		r=(alpha*r + (256-alpha)*r_)/256;
		g=(alpha*g + (256-alpha)*g_)/256;
		b=(alpha*b + (256-alpha)*b_)/256;
		
		int y2, u2, v2;
		rgb2yuv(r, g, b, y2, u2, v2);
		
		yuv_pic[i] = y2;
		//4 y blocks set one u and v
		if(cnt == 4){
		    yuv_pic[offset + k] = u2;
		    yuv_pic[offset + k + width*height/4] = v2;
		}
	}*/

	static void YUV2ARGB2YUV(char* data, char *yuv_pic, int width, int height,int alpha) {
		int size = width*height;  
		int offset = size;
		short u, v, y1, y2, y3, y4;
		for(int i=0, k=0; i < size; i+=2, k+=1) {
		    //printf("%d\n",i);
		    y1 = data[i];
		    y2 = data[i+1];
		    y3 = data[width+i];
		    y4 = data[width+i+1];
	   
		    u = data[offset+k];
		    v = data[offset+size/4+k];

		    convert(yuv_pic, y1, u, v, width,height, offset,i,k, alpha,1);
		    convert(yuv_pic, y2, u, v, width,height, offset,i+1,k, alpha,2);
		    convert(yuv_pic, y3, u, v, width,height, offset,width+i,k, alpha,3);
		    convert(yuv_pic, y4, u, v, width,height, offset,width+i+1,k, alpha,4);
	   
		    if (i!=0 && (i+2)%width==0)  
		        i+=width;  
		}
	}  

	/*
	static void YUV2ARGB2YUV_add(char* data, char* data2,char *yuv_pic, int width, int height,int alpha) {
		int size = width*height;
		int offset = size;
		unsigned char u, v, u_, v_, y1, y2, y3, y4, y1_, y2_, y3_, y4_ ;
		for(int i=0, k=0; i < size; i+=2, k+=1) {
		    //printf("%d\n",i);
		    y1 = data[i];
		    y2 = data[i+1];
		    y3 = data[width+i];
		    y4 = data[width+i+1];
		    
		    u = data[offset+k];
		    v = data[offset+size/4+k];
		    
		    y1_ = data2[i];
		    y2_ = data2[i+1];
		    y3_ = data2[width+i];
		    y4_ = data2[width+i+1];
		    
		    u_ = data2[offset+k];
		    v_ = data2[offset+size/4+k];
		    
		    convert_add(yuv_pic, y1, y1_, u, u_, v, v_, width,height, offset,i,k, alpha,1);
		    convert_add(yuv_pic, y2, y2_, u, u_, v, v_, width,height, offset,i+1,k, alpha,2);
		    convert_add(yuv_pic, y3, y3_, u, u_, v, v_, width,height, offset,width+i,k, alpha,3);
		    convert_add(yuv_pic, y4, y4_, u, u_, v, v_, width,height, offset,width+i+1,k, alpha,4);
		    
		    if (i!=0 && (i+2)%width==0)
		        i+=width;
		}
	}
	*/
};

