//
// Created by xiao on 19-4-29.
//

#ifndef DATABASE4_1_PROJECT_H
#define DATABASE4_1_PROJECT_H

#include <set>
#include "extrem.h"

using namespace std;
/**
 * 进行投影操作
 * 已经排好序的R在340-355上
 * addr设定为200
 * @param addr
 * @param buffer
 */
void projectA(int addr, Buffer* buffer){
//    set<int> set1;
//    for (int i = 1; i <=16 ; ++i) {
//        int* blk= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
//        for (int j = 0; j < 14; j+=2) {
//            set1.insert(blk[j]);
//        }
//        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
//    }
//    int* blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
//    int index=0;
//    for (auto x:set1){
//        blk[index]=x;
//        index++;
//        if(index==15){
//            blk[15]=0;
//            writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), addr, buffer);
//            addr++;
//            index=0;
//            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
//            blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
//        }
//    }
//    if(index!=0){
//        for (int i = index; i < 16; ++i) {
//            blk[i]=0;
//        }
//        writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), addr, buffer);
//        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
//    }
    int pre=0;
    bool  first= true;
    int* resblk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    int index=0;
    for (int i = 340; i <=355 ; ++i) {
        int* blk= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
        for (int j = 0; j < 14; j+=2) {
            if(first){
                pre=blk[0];
                first= false;
                resblk[index]=pre;
                index++;
                if(index==15){
                    resblk[index]=addr+1;
                    writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), addr, buffer);
                    index=0;
                }
            } else{
                int tem=blk[j];
                if(tem!=pre){
                    pre=tem;
                    resblk[index]=pre;
                    index++;
                    if(index==15){
                        resblk[index]=addr+1;
                        writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), addr, buffer);
                        freeBlockInBuffer(reinterpret_cast<unsigned char *>(resblk), buffer);
                        resblk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
                        index=0;
                        addr++;
                    }
                }
            }
        }
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
    }
    if(index!=0){
        for (int i = index; i <=15 ; ++i) {
            resblk[i]=0;
        }
        writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), addr, buffer);
    }
}
#endif //DATABASE4_1_PROJECT_H
