#include <fstream>
#include <iostream>
#include "YUV2ARGB2YUV.h"
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <stdio.h>
#include <time.h>
//#include <intrin.h>
using namespace std;
int process_without_simd();
int process_with_sse();
int main()
{
    int time1 = process_without_simd();
    printf("----time for total process_without_simd:%d \n", time1);
    return 0;
}
int process_without_simd(){
    int height = 1080;
    int width = 1920;
    int char_num = (height*width*3)>>1;
    cout<<"run picture processing 1 or 2?"<<endl;
    int index;
    cin>>index;
    clock_t start,end,start_tmp;
    start = clock();
    int time = 0;
    if(index==1){
        char* yuv_1 = new char[(1080*1920*3)>>1];//source image
        char* yuv_2 = new char[(1080*1920*3)>>1];//dest image
        ifstream fin;
        fin.open("dem2.yuv",ios::binary);
        fin.read(yuv_1, char_num);
        fin.close();
        ofstream fout;
        for(int A=1;A<256;A=A+3) {
            //printf("%d\n",A);
            start_tmp = clock();
            YUV2ARGB2YUV(yuv_1,yuv_2,1920,1080,A);
            end =clock();
            time = (int)((end - start_tmp)/1000);
            printf("time for loop %d is %d\n",(A-1)/3+1,time);
            char name[10];
            sprintf(name,"%d.yuv",(A-1)/3);
            //printf("%s",name);
            fout.open(name,ios::binary);
            fout.write(yuv_2,char_num);
            fout.close();
        }
    }
    else{
        char* yuv_0 = new char[(1080*1920*3)>>1];//source image 1
        char* yuv_1 = new char[(1080*1920*3)>>1];//source image 2
        char* yuv_2 = new char[(1080*1920*3)>>1];//dest image
        ifstream fin;
        fin.open("dem2.yuv",ios::binary);
        fin.read(yuv_0, char_num);
        fin.close();
        ifstream fin1;
        fin1.open("dem1.yuv",ios::binary);
        fin1.read(yuv_1, char_num);
        fin1.close();
        ofstream fout;
        for(int A=1;A<256;A=A+3) {
            //printf("%d\n",A);
            start_tmp = clock();
            YUV2ARGB2YUV_add(yuv_0,yuv_1,yuv_2,1920,1080,A);
            end =clock();
            time = (int)((end - start_tmp)/1000);
            printf("time for loop %d is %d\n",(A-1)/3+1,time);
            char name[10];
            sprintf(name,"add%d.yuv",(A-1)/3);
            //printf("%s",name);
            fout.open(name,ios::binary);
            fout.write(yuv_2,char_num);
            fout.close();
        }
    }
    end =clock();
    time = (int)((end - start)/1000);//count by second / ms
    return time;
}

int process_with_sse(){
    int time = 0;
    
    
    return time;
}