//
// Created by xiao on 19-4-29.
//

#ifndef DATABASE4_1_PROJECT_H
#define DATABASE4_1_PROJECT_H

#include <set>
#include "extrem.h"

using namespace std;
void projectA(int addr, Buffer* buffer){
    set<int> set1;
    for (int i = 1; i <=16 ; ++i) {
        int* blk= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
        for (int j = 0; j < 14; j+=2) {
            set1.insert(blk[j]);
        }
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
    }
    int* blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    int index=0;
    for (auto x:set1){
        blk[index]=x;
        index++;
        if(index==15){
            blk[15]=0;
            writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), addr, buffer);
            addr++;
            index=0;
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
            blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
        }
    }
    if(index!=0){
        for (int i = index; i < 16; ++i) {
            blk[i]=0;
        }
        writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), addr, buffer);
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
    }
}
#endif //DATABASE4_1_PROJECT_H
