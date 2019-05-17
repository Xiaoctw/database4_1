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
    int pre=0;
    bool  first= true;
    int* resblk= cast1(getNewBlockInBuffer(buffer));
    int index=0;
    for (int i = 340; i <=355 ; ++i) {
        int* blk= cast1(readBlockFromDisk(i, buffer));
        for (int j = 0; j < 14; j+=2) {
            if(first){
                pre=blk[0];
                first= false;
                resblk[index]=pre;
                index++;
                if(index==15){
                    resblk[index]=addr+1;
                    writeBlockToDisk(cast2(resblk), addr, buffer);
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
                        writeBlockToDisk(cast2(resblk), addr, buffer);
                        freeBlockInBuffer(cast2(resblk), buffer);
                        resblk= cast1(getNewBlockInBuffer(buffer));
                        index=0;
                        addr++;
                    }
                }
            }
        }
        freeBlockInBuffer(cast2(blk), buffer);
    }
    if(index!=0){
        for (int i = index; i <=15 ; ++i) {
            resblk[i]=0;
        }
        writeBlockToDisk(cast2(resblk), addr, buffer);
    }
}
#endif //DATABASE4_1_PROJECT_H
