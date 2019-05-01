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

void hashJoinBlk(const int* blk1,const int* blk2,Buffer* buffer,int* new_blk){
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
void hashJoin(Buffer* buffer){
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
                hashJoinBlk(blks[j],blk2,buffer,new_blk);
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


#endif //DATABASE4_1_JOIN_H
