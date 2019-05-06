//
// Created by xiao on 19-5-1.
//

#ifndef DATABASE4_1_SORT_H
#define DATABASE4_1_SORT_H

#include <climits>
#include "extrem.h"
#include <cstring>
#define MAX_SIZE 7
#define CAST reinterpret_cast<int *>
/**
 * 进行外存归并排序,这只是一部分函数
 * beg与end之间最多有7个元组
 * @param buffer 缓冲
 * @param beg 起始位置
 * @param end 终止位置
 * @param target 存放地点的首地址
 */
 int find_min(int* blks[],const int* indexes,int len){
     int min_index=0;
     int min_val=INT_MAX;
    for (int i = 0; i < len; ++i) {
        if (indexes[i]==14){//如果达到了最后的位置,就不再计算了
            continue;
        }
       // int val=*(*(blks)+len*i+indexes[i]);
       int val=blks[i][indexes[i]];
        if(val<min_val){
            min_val=val;
            min_index=i;
        }
    }
    return min_index;
 }
 /**
  * 判断是否全部到达了末尾
  * @param indexes
  * @param len
  * @return
  */
 bool judge1(const int* indexes,int len){
     for (int i = 0; i < len; ++i) {
         if(indexes[i]!=14){
             return false;
         }
     }
     return true;
 }
 /**
  * 进行归并排序的步骤
  * @param buffer 缓冲区
  * @param beg 开始位置
  * @param end 终止位置
  * @param target 目标地址
  */
void sort_merge1(Buffer *buffer, int beg, int end, int target){
     int len=end-beg+1;
    int* blks[end-beg+1];//块的数组指针
    for (int i = beg; i <=end ; ++i) {
        blks[i-beg]= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
    }
    int* resblk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    int indexes[end-beg+1];
    int res_index=0;
    memset(indexes,0, sizeof(indexes));
    while (!judge1(indexes,len)){
        int index=find_min(blks,indexes,len);
        resblk[res_index]=blks[index][indexes[index]];
        resblk[res_index+1]=blks[index][indexes[index]+1];
        res_index+=2;
        indexes[index]+=2;
        if(res_index==14){
            res_index=0;
            resblk[15]=target+1;
            target++;//进行自增操作
            writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), target-1, buffer);
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(resblk), buffer);
            resblk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
        }
    }
    if(res_index!=0){
        writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), target, buffer);
    }
    for (int j = 0; j < len; ++j) {
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blks[j]), buffer);
    }
    freeBlockInBuffer(reinterpret_cast<unsigned char *>(resblk), buffer);
}

/**
 * 对指定块进行sort操作,放到target处
 * @param addr
 * @param buffer
 * @param target
 */
void sort_blk(int addr,Buffer* buffer,int target){
     int* blk= reinterpret_cast<int *>(readBlockFromDisk(addr, buffer));
    for (int i = 0; i < 14; i+=2) {
        int min=i;
        for (int j = i+2; j < 14; j+=2) {
            if(blk[j]<blk[min]){
                min=j;
            }
        }
        int tem1=blk[i];
        int tem2=blk[i+1];
        blk[i]=blk[min];
        blk[i+1]=blk[min+1];
        blk[min]=tem1;
        blk[min+1]=tem2;
    }
    writeBlockToDisk(reinterpret_cast<unsigned char *>(blk),target,buffer);
    freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
 }

 /**
  * 进行第二步归并操作
  * 归并三个串
  * @param beg1
  * @param end1
  * @param beg2
  * @param end2
  * @param beg3
  * @param end3
  * @param target
  */
  int find_min(int val1,int val2,int val3, bool flag1,bool flag2, bool flag3){
      if(flag1&&flag2&&flag3) {
          if (val1 < val2) {
              if (val1 < val3) {
                  return 1;
              } else {
                  return 3;
              }
          } else {
              if (val2 < val3) {
                  return 2;
              } else {
                  return 3;
              }
          }
      }
      if(flag1&&flag2){
          if(val1<val2){
              return 1;
          } else{
              return 2;
          }
      }
      if(flag1&&flag3){
          if(val1<val3){
              return 1;
          } else{
              return 3;
          }
      }
      if(flag2&&flag3){
          if(val2<val3){
              return 2;
          } else{
              return 3;
          }
      }
      if(flag1){
          return 1;
      }
      if(flag2){
          return 2;
      }
     return 3;
  }
 void sort_merge2(Buffer* buffer,int beg1,int end1,int beg2,int end2,int beg3,int end3,int target){
     int i1=0,i2=0,i3=0;
     bool flag1= beg1<end1,flag2= beg2<end2,flag3=beg3<end3;
     int* blk1=CAST(readBlockFromDisk(beg1, buffer));
     int* blk2= reinterpret_cast<int *>(readBlockFromDisk(beg2, buffer));
     int* blk3;
     if(!flag3){
         blk3= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
     } else {
         blk3 = reinterpret_cast<int *>(readBlockFromDisk(beg3, buffer));
     }
    int* newblk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
     int i4=0;
    do{
       int index=find_min(blk1[i1],blk2[i2],blk3[i3],flag1,flag2,flag3);
       if(index==1){
           newblk[i4]=blk1[i1];
           newblk[i4+1]=blk1[i1+1];
           i1+=2;
           if(i1==14){
               freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk1), buffer);
               if(beg1<end1) {
                   blk1 = reinterpret_cast<int *>(readBlockFromDisk(beg1 + 1, buffer));
                   i1=0;
                   beg1++;
               } else{
                   flag1= false;
               }
           }
       } else if(index==2){
           newblk[i4]=blk2[i2];
           newblk[i4+1]=blk2[i2+1];
           i2+=2;
           if(i2==14){
               freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk2), buffer);
               if(beg2<end2) {
                   blk2 = reinterpret_cast<int *>(readBlockFromDisk(beg2 + 1, buffer));
                   beg2++;
                   i2=0;
               } else{
                   flag2= false;
               }
           }
       } else{
           newblk[i4]=blk3[i3];
           newblk[i4+1]=blk3[i3+1];
           i3+=2;
           if(i3==14){
               freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk3), buffer);
               if(beg3<end3) {
                   i3=0;
                   blk3 = reinterpret_cast<int *>(readBlockFromDisk(beg3 + 1, buffer));
                   beg3++;
               } else{
                   flag3= false;
               }
           }
       }
       i4+=2;
       if(i4==14){
           newblk[i4]=0;
           newblk[i4+1]=target+1;
           writeBlockToDisk(reinterpret_cast<unsigned char *>(newblk), target, buffer);
           newblk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
           target++;
           i4=0;
       }
    }while (flag1||flag2||flag3);
}
 /**
  * 总的函数,进行排序算法
  * 排序关系R
  */
 void MergeSort(Buffer* buffer){
     for (int i = 1; i <= 16; ++i) {
         sort_blk(i,buffer,300+i);
     }
     sort_merge1(buffer, 301, 307, 320);//归并1-7块
     sort_merge1(buffer, 308, 314, 327);//归并8-14块
     sort_merge1(buffer, 315, 316, 334);//归并15,16块
     sort_merge2(buffer,320,326,327,333,334,335,340);
     for (int j = 301; j <=316 ; ++j) {
         dropBlockOnDisk(j);
     }
     for (int j = 320; j <=335 ; ++j) {
         dropBlockOnDisk(j);
     }
}

/**
 * 排序关系S
 * 从360开始进行排序
 * @param buffer
 */
void MergeSort2(Buffer * buffer){
    for (int i = 0; i < 32; ++i) {
        sort_blk(20+i,buffer,361+i);
    }
    sort_merge1(buffer,361,367,401);
    sort_merge1(buffer,368,374,408);
    sort_merge1(buffer,375,381,415);
    sort_merge1(buffer,382,388,422);
    sort_merge1(buffer,389,392,429);
    sort_merge2(buffer,401,407,408,414,415,421,433);
    sort_merge2(buffer,422,428,429,432,0,-1,454);
    sort_merge2(buffer,433,453,454,464,0,-1,470);
    for (int j = 361; j <= 392 ; ++j) {
        dropBlockOnDisk(j);
    }
    for (int j = 401; j <=464 ; ++j) {
        dropBlockOnDisk(j);
    }
 }
#endif //DATABASE4_1_SORT_H
