//
// Created by xiao on 19-4-29.
//
//在这里实现连接算法
#ifndef DATABASE4_1_JOIN_H
#define DATABASE4_1_JOIN_H
#define NUM_RELA_R 16
#define BEG_RELA_R 1
#define NUM_RELA_S 32
#define BEG_RELA_S 20

#include "extrem.h"
int res_index_blk_hash=100;
int blk_index_hash=0;

void loopJoinBlk(const int *blk1, const int *blk2, Buffer *buffer, int *new_blk){
    for (int i = 0; i < 14; i+=2) {
        for (int j = 0; j < 14; j+=2) {
            if(blk1[i]==blk2[j]){
                new_blk[blk_index_hash]=blk1[i];
                new_blk[blk_index_hash+1]=blk1[i+1];
                new_blk[blk_index_hash+2]=blk2[j+1];
                blk_index_hash+=3;//向后移三个位置
                if(blk_index_hash==15){
                    new_blk[15]=0;
                    writeBlockToDisk(reinterpret_cast<unsigned char *>(new_blk),res_index_blk_hash, buffer);
                    freeBlockInBuffer(reinterpret_cast<unsigned char *>(new_blk), buffer);
                    blk_index_hash=0;
                    new_blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
                    res_index_blk_hash++;
                }
            }
        }
    }
}
void loopJoin(Buffer *buffer){
    int*  new_blk;
    int num=6;//外层循环一个读取6个块,内存循环一次读取1个块
    new_blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    for (int i = BEG_RELA_R; i <= BEG_RELA_R+NUM_RELA_R-1; i+=num) {
        int num_blk=min(num,NUM_RELA_R-i+1);
        int* blks[num_blk];
        for (int j = 0; j < num_blk; ++j) {
            blks[j]= reinterpret_cast<int *>(readBlockFromDisk(i + j, buffer));//读入外层循环的磁盘块
        }
        for (int k = BEG_RELA_S; k <=BEG_RELA_S+NUM_RELA_S-1 ; ++k) {
            int* blk2= reinterpret_cast<int *>(readBlockFromDisk(k, buffer));
            for (int j = 0; j < num_blk; ++j) {
                loopJoinBlk(blks[j], blk2, buffer, new_blk);
            }
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk2), buffer);
        }
        for (int l = 0; l < num_blk; ++l) {
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blks[l]), buffer);
        }
    }
    if(blk_index_hash!=0){
        for (int i = blk_index_hash; i <16 ; ++i) {
            new_blk[i]=0;
        }
        writeBlockToDisk(reinterpret_cast<unsigned char *>(new_blk), res_index_blk_hash, buffer);
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(new_blk), buffer);
    }
}
/**
 * 进行merge连接算法
 * @param buffer
 */
void sort_merge(Buffer* buffer,int target){
    int beg2=470;
    int end2=501;
    int beg1=340;
    int end1=355;
    int pre2=0;
    int i1=0;
    bool flag1= true;
    int* resblk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    int index=0;
    int* blk1= reinterpret_cast<int *>(readBlockFromDisk(beg1, buffer));
    int* blk2= reinterpret_cast<int *>(readBlockFromDisk(beg2, buffer));
    while (flag1){
        int i2;
        for (i2 = pre2; i2 < 14; i2+=2) {
            if(blk2[i2]==blk1[i1]){
                resblk[index]=blk1[i1];
                resblk[index+1]=blk1[i1+1];
                resblk[index+2]=blk2[i2+1];
                index+=3;
                if(index==15){
                    resblk[14]=0;
                    resblk[15]=target+1;
                    writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), target, buffer);
                    target++;
                    freeBlockInBuffer(reinterpret_cast<unsigned char *>(resblk), buffer);
                    resblk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
                    index=0;
                }
            } else{
                break;//不相等就退出
            }
        }
        if(i2==14){//读到了最后，开始读入下一块
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk2), buffer);
            beg2++;
            if(beg2<=end2){
                blk2= reinterpret_cast<int *>(readBlockFromDisk(beg2, buffer));
            }
            pre2=0;//读入了下一块内容
        }
        i1+=2;//后退一步
        if(i1==14){//到达了最后一位
            beg1++;
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk1), buffer);
            if(beg1<=end1) {
                blk1 = reinterpret_cast<int *>(readBlockFromDisk(beg1, buffer));
            } else{
                flag1= false;
            }
            i1=0;
        } else{
            if(blk1[i1]!=blk1[i1-2]){
                pre2=i2;//跟进
            }
        }
    }
    if(index!=0){
        writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), target, buffer);
    }
}


#endif //DATABASE4_1_JOIN_H
