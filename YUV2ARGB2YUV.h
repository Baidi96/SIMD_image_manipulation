void yuv2rgb(int y, int u, int v, int &r, int &g, int &b)
{
	// r = y+1.140*v;
    // g = y-0.394*u-0.581*v;
    // b = y+2.032*u;

    // r = 1.164383 * (y - 16) + 1.596027*(v - 128);
    // b = 1.164383 * (y - 16) + 2.017232*(u - 128);
    // g = 1.164383 * (y - 16) - 0.391762*(u - 128) - 0.812968*(v - 128);

	r = (76309 * (y - 16) + 104597 * (v - 128)) / 65536;
    b = (76309 * (y - 16) + 133201 * (u - 128)) / 65536;
    g = (76309 * (y - 16) - 25675 * (u - 128) - 53279 * (v - 128)) / 65336;
    r = r>255 ? 255 : r<0 ? 0 : r;
    g = g>255 ? 255 : g<0 ? 0 : g;
    b = b>255 ? 255 : b<0 ? 0 : b;
}

void rgb2yuv(int r, int g, int b, int &y, int &u, int &v)
{
	//char y2 = 0.299*r+0.587*g+0.114*b;
	//char u2 = 0.492*(b-y2);
    //char v2 = 0.877*(r-y2);
    
    //unsigned char y2 = 0.256788*r + 0.504129*g + 0.097906*b + 16;
    //unsigned char u2 = -0.148223*r - 0.290993*g + 0.439216*b + 128;
    //unsigned char v2 = 0.439216*r - 0.367788*g - 0.071427*b + 128;
    
	y = (16829 * r + 33039 * g + 6416 * b) / 65536 + 16;
	u = (-9714 * r - 19071 * g + 28784 * b) / 65536 + 128;
    v = (28784 * r - 24103 * g - 4681 * b) / 65536 + 128;
}

void convert(char *yuv_pic,unsigned char y,unsigned char u,unsigned char v, int width, int height,int offset,int i, int k, int alpha, int cnt) {
    int r, g, b;
    yuv2rgb(y, u, v, r, g, b);
    
    r=(alpha*r)/256;
    g=(alpha*g)/256;
    b=(alpha*b)/256;

    int y2, u2, v2;
    rgb2yuv(r, g, b, y2, u2, v2);
    
    yuv_pic[i] = y2;
    //4 y blocks set one u and v
    if(cnt == 4) {
        yuv_pic[offset + k] = u2;
        yuv_pic[offset + k + width*height/4] = v2;
    }
}

void convert_add(char *yuv_pic,unsigned char y,unsigned char y_,unsigned char u,unsigned char u_,unsigned char v,unsigned char v_, int width, int height,int offset,int i, int k, int alpha, int cnt) {
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
}

void YUV2ARGB2YUV(char* data, char *yuv_pic, int width, int height,int alpha) {
    int size = width*height;  
    int offset = size;
    unsigned char u, v, y1, y2, y3, y4;
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


void YUV2ARGB2YUV_add(char* data, char* data2,char *yuv_pic, int width, int height,int alpha) {
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

