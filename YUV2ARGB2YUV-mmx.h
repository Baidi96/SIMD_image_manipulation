#include <mmintrin.h>
#include <stdio.h>

#define ARG_TYPE_MMX __m64
#define _MM_mul(x,y) _mm_add_pi32(_mm_mullo_pi16(x,y),_mm_mulhi_pi16(x,y))

static const short yuv2b[3] = { 0.164383 * (1 << 16),  0.017232 * (1 << 16),0 * (1 << 16) };
static const short yuv2g[3] = { 0.164383 * (1 << 16), -0.391762 * (1 << 16), -0.312968 * (1 << 16) };
static const short yuv2r[3] = { 0.164383 * (1 << 16),0 * (1 << 16),  0.096027 * (1 << 16) };
static const short rgb2y[3] = { short(0.256788 * (1 << 16)),  short(0.004129 * (1 << 16)),  short(0.097906 * (1 << 16)) };
static const short rgb2v[3] = { short(0.439216 * (1 << 16)),  short(-0.367788 * (1 << 16)), short(-0.071427 * (1 << 16)) };
static const short rgb2u[3] = { short(-0.148223 * (1 << 16)), short(-0.290993 * (1 << 16)), short(0.439216 * (1 << 16)) };
static const ARG_TYPE_MMX off128 = _mm_set_pi16(128, 128, 128, 128);
static const ARG_TYPE_MMX off16 = _mm_set_pi16(16, 16, 16, 16);
const ARG_TYPE_MMX Y_R = _mm_set_pi16(yuv2r[0], yuv2r[0], yuv2r[0], yuv2r[0]);
const ARG_TYPE_MMX U_R = _mm_set_pi16(yuv2r[1], yuv2r[1], yuv2r[1], yuv2r[1]);
const ARG_TYPE_MMX V_R = _mm_set_pi16(yuv2r[2], yuv2r[2], yuv2r[2], yuv2r[2]);
const ARG_TYPE_MMX Y_G = _mm_set_pi16(yuv2g[0], yuv2g[0], yuv2g[0], yuv2g[0]);
const ARG_TYPE_MMX U_G = _mm_set_pi16(yuv2g[1], yuv2g[1], yuv2g[1], yuv2g[1]);
const ARG_TYPE_MMX V_G = _mm_set_pi16(yuv2g[2], yuv2g[2], yuv2g[2], yuv2g[2]);
const ARG_TYPE_MMX Y_B = _mm_set_pi16(yuv2b[0], yuv2b[0], yuv2b[0], yuv2b[0]);
const ARG_TYPE_MMX U_B = _mm_set_pi16(yuv2b[1], yuv2b[1], yuv2b[1], yuv2b[1]);
const ARG_TYPE_MMX V_B = _mm_set_pi16(yuv2b[2], yuv2b[2], yuv2b[2], yuv2b[2]);

const ARG_TYPE_MMX R_Y = _mm_set_pi16(rgb2y[0], rgb2y[0], rgb2y[0], rgb2y[0]);
const ARG_TYPE_MMX G_Y = _mm_set_pi16(rgb2y[1], rgb2y[1], rgb2y[1], rgb2y[1]);
const ARG_TYPE_MMX B_Y = _mm_set_pi16(rgb2y[2], rgb2y[2], rgb2y[2], rgb2y[2]);
const ARG_TYPE_MMX R_U = _mm_set_pi16(rgb2u[0], rgb2u[0], rgb2u[0], rgb2u[0]);
const ARG_TYPE_MMX G_U = _mm_set_pi16(rgb2u[1], rgb2u[1], rgb2u[1], rgb2u[1]);
const ARG_TYPE_MMX B_U = _mm_set_pi16(rgb2u[2], rgb2u[2], rgb2u[2], rgb2u[2]);
const ARG_TYPE_MMX R_V = _mm_set_pi16(rgb2v[0], rgb2v[0], rgb2v[0], rgb2v[0]);
const ARG_TYPE_MMX G_V = _mm_set_pi16(rgb2v[1], rgb2v[1], rgb2v[1], rgb2v[1]);
const ARG_TYPE_MMX B_V = _mm_set_pi16(rgb2v[2], rgb2v[2], rgb2v[2], rgb2v[2]);
struct MMX
{
	static void yuv2rgb(ARG_TYPE_MMX y, ARG_TYPE_MMX u, ARG_TYPE_MMX v, ARG_TYPE_MMX &r, ARG_TYPE_MMX &g, ARG_TYPE_MMX &b)
	{
		/*
		r = 1.164383 * (y - 16) + 1.596027*(v - 128);
		b = 1.164383 * (y - 16) + 2.017232*(u - 128);
		g = 1.164383 * (y - 16) - 0.391762*(u - 128) - 0.812968*(v - 128);
		*/

		ARG_TYPE_MMX y1 = _m_psubw(y, off16);
		ARG_TYPE_MMX u1 = _m_psubw(u, off128);
		ARG_TYPE_MMX v1 = _m_psubw(v, off128);
        
		//r = _MM_mul(y1, _mm_set1_pi32(1.164383));
        ARG_TYPE_MMX tmp1 = _m_pmulhw(y1, Y_R);
        r = _m_paddsw(y1,tmp1);
        tmp1 = _m_pmulhw(u1, U_R);
        r = _m_paddsw(r,tmp1);
        tmp1 = _m_psllwi(u1, 1);
        r = _m_paddsw(r,tmp1);
        
		//b = _MM_mul(y1, _mm_set1_pi32(1.164383));
        ARG_TYPE_MMX tmp2 = _m_pmulhw(y1, Y_G);
        g = _m_paddsw(y1,tmp2);
        tmp2 = _m_pmulhw(u1, U_G);
        g = _m_paddsw(g,tmp2);
        tmp2 = _m_pmulhw(v1, V_G);
        g = _m_paddsw(g, tmp2);
        tmp2 = _m_psrawi(v1, 1);
        g = _m_psubsw(g, tmp2);
        
		//g = _MM_mul(y1, _mm_set1_pi32(1.164383));
        ARG_TYPE_MMX tmp3 = _m_pmulhw(y1, Y_B);
        b = _m_paddsw(tmp3,y1);
        tmp3 = _m_pmulhw(v1, V_B);
        b = _m_paddsw(b, tmp3);
        tmp3 = _m_psrawi(v1, 1);
        b = _m_paddsw(b, tmp3);
        tmp3 = _m_psllwi(v1, 1);
        b = _m_paddsw(b, tmp3);
		
		/*
		r = r>255 ? 255 : r<0 ? 0 : r;
		g = g>255 ? 255 : g<0 ? 0 : g;
		b = b>255 ? 255 : b<0 ? 0 : b;
		*/
        // comparision not found
	}

	static void rgb2yuv(ARG_TYPE_MMX r, ARG_TYPE_MMX g, ARG_TYPE_MMX b, ARG_TYPE_MMX &y, ARG_TYPE_MMX &u, ARG_TYPE_MMX &v)
	{
		/*
		y = 0.256788*r + 0.504129*g + 0.097906*b + 16;
		u = -0.148223*r - 0.290993*g + 0.439216*b + 128;
		v = 0.439216*r - 0.367788*g - 0.071427*b + 128;
		*/
        y = _m_pmulhw(r, R_Y);
        ARG_TYPE_MMX tmp1 = _m_pmulhw(g, G_Y);
        y = _m_paddsw(tmp1, y);
        tmp1 = _m_psrlwi(g, 1);
        y = _m_paddsw(tmp1, y);
        tmp1 = _m_pmulhw(b, B_Y);
        y = _m_paddsw(tmp1, y);
        y = _m_paddsw(y, off16);
        
        u =  _m_pmulhw(r, R_U);
        tmp1 = _m_pmulhw(g, G_U);
        u =  _m_paddsw(tmp1, u);
        tmp1 = _m_pmulhw(b, B_U);
        u = _m_paddsw(tmp1, u);
        u = _m_paddsw(u, off128);
        
        v = _m_pmulhw(r, R_V);
        tmp1 = _m_pmulhw(g, G_V);
        v = _m_paddsw(tmp1, v);
        tmp1 =  _m_pmulhw(b, B_V);
        v = _m_paddsw(tmp1, v);
        v = _m_paddsw(v, off128);
		/*ARG_TYPE_MMX y0 = _mm_add_pi32(_MM_mul(_mm_set1_pi32(0.256788), r), _MM_mul(_mm_set1_pi32(0.504129), g));
		ARG_TYPE_MMX y1 = _mm_add_pi32(_MM_mul(_mm_set1_pi32(0.097906), b), _mm_set1_pi32(16.0));
		y = _mm_add_pi32(y0, y1);
		ARG_TYPE_MMX u0 = _mm_add_pi32(_MM_mul(_mm_set1_pi32(-0.148223), r), _MM_mul(_mm_set1_pi32(-0.290993), g));
		ARG_TYPE_MMX u1 = _mm_add_pi32(_MM_mul(_mm_set1_pi32(0.439216), b), _mm_set1_pi32(128.0));
		u = _mm_add_pi32(u0, u1);
		ARG_TYPE_MMX v0 = _mm_add_pi32(_MM_mul(_mm_set1_pi32(0.439216), r), _MM_mul(_mm_set1_pi32(-0.367788), g));
		ARG_TYPE_MMX v1 = _mm_add_pi32(_MM_mul(_mm_set1_pi32(-0.071427), b), _mm_set1_pi32(128.0));
		v = _mm_add_pi32(v0, v1);*/
		
		// printf("y = %d, u = %d, v = %d\n", y, u, v);
	}

	static void convert(char *yuv_pic, ARG_TYPE_MMX y, ARG_TYPE_MMX u, ARG_TYPE_MMX v, int width, int height,int offset,int i, int k, int alpha, int cnt) {
		ARG_TYPE_MMX r, g, b;
		yuv2rgb(y, u, v, r, g, b);
		
        ARG_TYPE_MMX alf = _mm_set_pi16(alpha, alpha, alpha, alpha);
        ARG_TYPE_MMX tmp = _m_pmullw(r, alf);
        tmp = _m_psrlwi(tmp, 8);
        r = tmp;
        tmp = _m_pmullw(g, alf);
        tmp = _m_psrlwi(tmp, 8);
        g = tmp;
        tmp = _m_pmullw(b, alf);
        tmp = _m_psrlwi(tmp, 8);
        b = tmp;
		//r = _MM_mul(r, _mm_set1_pi32(fac));
		//g = _MM_mul(g, _mm_set1_pi32(fac));
		//b = _MM_mul(b, _mm_set1_pi32(fac));

		ARG_TYPE_MMX y2, u2, v2;
		rgb2yuv(r, g, b, y2, u2, v2);
		
		yuv_pic[i] = (unsigned char)(((short*)&y2)[0]);
		yuv_pic[i+1] = (unsigned char)(((short*)&y2)[1]);
		//4 y blocks set one u and v
		if(cnt == 4)
		{
		    yuv_pic[offset + k] = (unsigned char)(((short*)&u2)[0]);
		    yuv_pic[offset + k + width*height/4] = (unsigned char)(((short*)&v2)[0]);
		}
	}
	
	static void convert_add(char *yuv_pic, ARG_TYPE_MMX y, ARG_TYPE_MMX y_, ARG_TYPE_MMX u, ARG_TYPE_MMX u_, ARG_TYPE_MMX v, ARG_TYPE_MMX v_, int width, int height,int offset,int i, int k, int alpha, int cnt) {
		ARG_TYPE_MMX r,g,b,r_,g_,b_;
		yuv2rgb(y, u, v, r, g, b);
		yuv2rgb(y_, u_, v_, r_, g_, b_);
		/*
		r=(alpha*r + (256-alpha)*r_)/256;
		g=(alpha*g + (256-alpha)*g_)/256;
		b=(alpha*b + (256-alpha)*b_)/256;
		*/
        ARG_TYPE_MMX alf = _mm_set_pi16(alpha, alpha, alpha, alpha);
        ARG_TYPE_MMX tmp = _m_pmullw(r, alf);
        tmp = _m_psrlwi(tmp, 8);
        r = tmp;
        tmp = _m_pmullw(g, alf);
        tmp = _m_psrlwi(tmp, 8);
        g = tmp;
        tmp = _m_pmullw(b, alf);
        tmp = _m_psrlwi(tmp, 8);
        b = tmp;
        ARG_TYPE_MMX off256 = _mm_set_pi16(256, 256, 256, 256);
        ARG_TYPE_MMX tmp2 =_m_psubsw(off256,alf);//(256-alpha)
        tmp = _m_pmullw(r_, tmp2);
        tmp = _m_psrlwi(tmp, 8);
        r = _m_paddsw(tmp, r);
        
        tmp = _m_pmullw(g_, tmp2);
        tmp = _m_psrlwi(tmp, 8);
        g = _m_paddsw(tmp, g);
        
        tmp = _m_pmullw(b_, tmp2);
        tmp = _m_psrlwi(tmp, 8);
        b = _m_paddsw(tmp, b);
        
		
		ARG_TYPE_MMX y2, u2, v2;
		rgb2yuv(r, g, b, y2, u2, v2);
		
		yuv_pic[i] = (unsigned char)(((short*)&y2)[0]);
		yuv_pic[i+1] = (unsigned char)(((short*)&y2)[1]);
		//4 y blocks set one u and v
		if(cnt == 4)
		{
		    yuv_pic[offset + k] = (unsigned char)(((short*)&u2)[0]);
		    yuv_pic[offset + k + width*height/4] = (unsigned char)(((short*)&v2)[0]);
		}
	}

	static void YUV2ARGB2YUV(char* data, char *yuv_pic, int width, int height,int alpha) {
		int size = width*height;  
		int offset = size;
		unsigned char u0, v0, y01, y02, y11, y12;
		ARG_TYPE_MMX y0, y1, u, v;
		for(int i=0, k=0; i < size; i+=2, k+=1) {
		    //printf("%d\n",i);
		    y01 = data[i];
		    y02 = data[i+1];
		    y11 = data[width+i];
		    y12 = data[width+i+1];
	   
		    u0 = data[offset+k];
		    v0 = data[offset+size/4+k];
		    
            y0 = _mm_set_pi16((short)y02, (short)y02, (short)y01,(short)y01);
            y1 = _mm_set_pi16((short)y12, (short)y12, (short)y11, (short)y11);
            
		    //y0 = _mm_set_pi32((double)y01, (double)y02);
		    //y1 = _mm_set_pi32((double)y11, (double)y12);
            u = _mm_set_pi16((short)u0,(short)u0,(short)u0,(short)u0);
            v = _mm_set_pi16((short)v0,(short)v0,(short)v0,(short)v0);
		    //u = _mm_set1_pi32((double)u0);
		    //v = _mm_set1_pi32((double)v0);
		    
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
		ARG_TYPE_MMX u, u_, v, v_, y0, y0_, y1, y1_;
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
		    
            y0 = _mm_set_pi16((short)y02, (short)y02, (short)y01,(short)y01);
            y1 = _mm_set_pi16((short)y12, (short)y12, (short)y11, (short)y11);
		    //y0 = _mm_set_pi32((double)y01, (double)y02);
		    //y1 = _mm_set_pi32((double)y11, (double)y12);
            u = _mm_set_pi16((short)u0,(short)u0,(short)u0,(short)u0);
            v = _mm_set_pi16((short)v0,(short)v0,(short)v0,(short)v0);
		    //u = _mm_set1_pi32((double)u0);
		    //v = _mm_set1_pi32((double)v0);
            y0_ = _mm_set_pi16((short)y02_, (short)y02_, (short)y01_,(short)y01_);
            y1_ = _mm_set_pi16((short)y12_, (short)y12_, (short)y11_, (short)y11_);
		    //y0_ = _mm_set_pi32((double)y01_, (double)y02_);
		    //y1_ = _mm_set_pi32((double)y11_, (double)y12_);
            u_ = _mm_set_pi16((short)u0_,(short)u0_,(short)u0_,(short)u0_);
            v_ = _mm_set_pi16((short)v0_,(short)v0_,(short)v0_,(short)v0_);
		    //u_ = _mm_set1_pi32((double)u0_);
		    //v_ = _mm_set1_pi32((double)v0_);
		    
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

