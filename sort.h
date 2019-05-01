//
// Created by xiao on 19-5-1.
//

#ifndef DATABASE4_1_SORT_H
#define DATABASE4_1_SORT_H

#include <climits>
#include "extrem.h"
#include <cstring>
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
        if(blks[i][indexes[i]]<min_val){
            min_val=blks[i][indexes[i]];
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
void sort_merge(Buffer* buffer,int beg,int end,int target){
     int len=end-beg+1;
    int* blks[end-beg+1];//块的数组指针
    for (int i = beg; i <=end ; ++i) {
        blks[i]= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
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
            writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), target, buffer);
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

#endif //DATABASE4_1_SORT_H
