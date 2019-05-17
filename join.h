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
int res_index_blk_hash;
int blk_index_hash;
void loopJoinBlk(const int *blk1, const int *blk2, Buffer *buffer, int *new_blk){
    for (int i = 0; i < 14; i+=2) {
        for (int j = 0; j < 14; j+=2) {
            if(blk1[i]==0){
                continue;//避免最后的0参与运算
            }
            if(blk1[i]==blk2[j]){
                new_blk[blk_index_hash]=blk1[i];
                new_blk[blk_index_hash+1]=blk1[i+1];
                new_blk[blk_index_hash+2]=blk2[j+1];
                blk_index_hash+=3;//向后移三个位置
                if(blk_index_hash==15){
                    new_blk[15]=0;
                    writeBlockToDisk(cast2(new_blk),res_index_blk_hash, buffer);
                    freeBlockInBuffer(cast2(new_blk), buffer);
                    blk_index_hash=0;
                    new_blk= cast1(getNewBlockInBuffer(buffer));
                    res_index_blk_hash++;
                }
            }
        }
    }
}
void loopjoin(Buffer *buffer, int beg1, int num1, int beg2, int num2){
    int*  new_blk;
    int num=6;//外层循环一个读取6个块,内存循环一次读取1个块
    new_blk= cast1(getNewBlockInBuffer(buffer));
    for (int i = beg1; i <=beg1+num1-1; i+=num) {
        int num_blk=min(num,beg1+num1-i);
        int* blks[num_blk];
        for (int j = 0; j < num_blk; ++j) {
            blks[j]= cast1(readBlockFromDisk(i + j, buffer));//读入外层循环的磁盘块
        }
        for (int k = beg2; k <=beg2+num2-1 ; ++k) {
            int* blk2= cast1(readBlockFromDisk(k, buffer));
            for (int j = 0; j < num_blk; ++j) {
                loopJoinBlk(blks[j], blk2, buffer, new_blk);
            }
            freeBlockInBuffer(cast2(blk2), buffer);
        }
        for (int l = 0; l < num_blk; ++l) {
            freeBlockInBuffer(cast2(blks[l]), buffer);
        }
    }
    if(blk_index_hash!=0){
        for (int i = blk_index_hash; i <16 ; ++i) {
            new_blk[i]=0;
        }
        blk_index_hash=0;
        writeBlockToDisk(cast2(new_blk), res_index_blk_hash, buffer);
        freeBlockInBuffer(cast2(new_blk), buffer);
    }
}
void LOOPJOIN(Buffer *buffer){
    res_index_blk_hash=100;
    blk_index_hash=0;
    loopjoin(buffer, BEG_RELA_R, NUM_RELA_R, BEG_RELA_S, NUM_RELA_S);
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
    int* resblk= cast1(getNewBlockInBuffer(buffer));
    int index=0;
    int* blk1= cast1(readBlockFromDisk(beg1, buffer));
    int* blk2= cast1(readBlockFromDisk(beg2, buffer));
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
                    writeBlockToDisk(cast2(resblk), target, buffer);
                    target++;
                    freeBlockInBuffer(cast2(resblk), buffer);
                    resblk= cast1(getNewBlockInBuffer(buffer));
                    index=0;
                }
            } else{
                break;//不相等就退出
            }
        }
        if(i2==14){//读到了最后，开始读入下一块
            freeBlockInBuffer(cast2(blk2), buffer);
            beg2++;
            if(beg2<=end2){
                blk2= cast1(readBlockFromDisk(beg2, buffer));
            }
            pre2=0;//读入了下一块内容
        }
        i1+=2;//后退一步
        if(i1==14){//到达了最后一位
            beg1++;
            freeBlockInBuffer(cast2(blk1), buffer);
            if(beg1<=end1) {
                blk1 = cast1(readBlockFromDisk(beg1, buffer));
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
        writeBlockToDisk(cast2(resblk), target, buffer);
    }
}
/**
 * 在blk1中寻找和val相等的下标,beg1和i1随之变化
 * @param val
 * @param buffer
 * @param beg1
 * @param i1
 * @return
 */
int find_equals_val(int val,Buffer* buffer,int* blk,int& beg1,int& i1,int end){
    if(val>40){
        return -1;
    }
    while (i1-2>=0&&blk[i1-2]==val){//进行回退操作
        i1-=2;
    }
    while (blk[i1]!=val){
        i1+=2;
        if(i1==14){//继续读入下一块
            freeBlockInBuffer(cast2(blk), buffer);
            beg1++;
            i1=0;
            if(beg1>end){
                return -1;
            }
            blk= cast1(readBlockFromDisk(beg1, buffer));
        }
    }
    int num=0;
    for (int i = i1; i < 14; i+=2) {
        if(blk[i]==val){
            num++;
        }
    }
    return num;
}
void Sort_Merge(Buffer* buffer,int target){
    int beg2=470;
    int end2=501;
    int beg1=340;
    int end1=355;
    int i1=0,i2=0;
    bool flag1=true,flag2=true;
    int* blk1= cast1(readBlockFromDisk(beg1, buffer));
    int* blk2= cast1(readBlockFromDisk(beg2, buffer));
    beg1++;
    beg2++;
    int index=0;
    int* resblk= cast1(getNewBlockInBuffer(buffer));
    while (true){
        for (int i = 0; i < 14; i+=2) {
            int val=blk2[i];
            int num=find_equals_val(val,buffer,blk1,beg1,i1,end1);
            if(num==-1){
                break;
            }
            for (int j = 0; j < num; ++j) {
                resblk[index]=val;
                resblk[index+1]=blk1[2*j+i1+1];
                resblk[index+2]=blk2[i+1];
                index+=3;
                if(index==15){
                    resblk[index]=target+1;
                    writeBlockToDisk(cast2(resblk), target, buffer);
                    freeBlockInBuffer(cast2(resblk), buffer);
                    resblk= cast1(getNewBlockInBuffer(buffer));
                    index=0;
                    target++;
                }
            }

        }
        freeBlockInBuffer(cast2(blk2), buffer);
        beg2++;
        if(beg2>end2) {
            break;
        }
        blk2= cast1(readBlockFromDisk(beg2, buffer));
    }
    if(index!=0){
        for (int i = index; i < 16; ++i) {
            resblk[i]=0;
        }
        writeBlockToDisk(cast2(resblk), target, buffer);
        freeBlockInBuffer(cast2(resblk),buffer);
    }
}




/**
 * 开始hash连接
 */
 int* addElem(Buffer* buffer,int*  blk,int& index,int val1,int val2,int& target){
     blk[index]=val1;
     blk[index+1]=val2;
     index+=2;
     if(index==14){
         blk[15]=target+1;
         writeBlockToDisk(cast2(blk), target, buffer);
         freeBlockInBuffer(cast2(blk), buffer);
         target++;
         blk= cast1(getNewBlockInBuffer(buffer));
         index=0;
     }
    return blk;
 }

 void writeLastBlk(Buffer* buffer,int* blk,int i,int target){
     if(i!=0){
         for (int j = i; j <=15 ; ++j) {
             blk[j]=0;
         }
     }
     writeBlockToDisk(cast2(blk), target, buffer);
     freeBlockInBuffer(cast2(blk), buffer);
 }
 /**
  * 对R进行哈希操作
  * @param buffer 缓冲区
  */
 void HASH_R(Buffer *buffer){
     int* blk0=cast1(getNewBlockInBuffer(buffer));
     int* blk1= cast1(getNewBlockInBuffer(buffer));
     int* blk2=cast1(getNewBlockInBuffer(buffer));
     int* blk3=cast1(getNewBlockInBuffer(buffer));
     int* blk4=cast1(getNewBlockInBuffer(buffer));
     int target0=600,target1=610,target2=620,target3=630,target4=640;
     int i0=0,i1=0,i2=0,i3=0,i4=0;
     for (int i = 1; i <= 16 ; ++i) {
         int* blk= cast1(readBlockFromDisk(i, buffer));
         for (int j = 0; j < 14; j+=2) {
             if(blk[j]%5==0){
                 blk0=addElem(buffer,blk0,i0,blk[j],blk[j+1],target0);
             } else if(blk[j]%5==1){
                 blk1=addElem(buffer,blk1,i1,blk[j],blk[j+1],target1);
             } else if(blk[j]%5==2){
                 blk2=addElem(buffer,blk2,i2,blk[j],blk[j+1],target2);
             } else if(blk[j]%5==3){
                 blk3=addElem(buffer,blk3,i3,blk[j],blk[j+1],target3);
             } else{
                 blk4=addElem(buffer,blk4,i4,blk[j],blk[j+1],target4);
             }
         }
         freeBlockInBuffer(cast2(blk), buffer);
     }
     writeLastBlk(buffer,blk0,i0,target0);
     writeLastBlk(buffer,blk1,i1,target1);
     writeLastBlk(buffer,blk2,i2,target2);
     writeLastBlk(buffer,blk3,i3,target3);
     writeLastBlk(buffer,blk4,i4,target4);
 }

 /**
  * 对S进行哈希操作
  * @param buffer 缓冲区
  */
 void HASH_S(Buffer *buffer){
     int* blk0=cast1(getNewBlockInBuffer(buffer));
     int* blk1= cast1(getNewBlockInBuffer(buffer));
     int* blk2=cast1(getNewBlockInBuffer(buffer));
     int* blk3=cast1(getNewBlockInBuffer(buffer));
     int* blk4=cast1(getNewBlockInBuffer(buffer));
     int target0=650,target1=660,target2=670,target3=680,target4=690;
     int i0=0,i1=0,i2=0,i3=0,i4=0;
     for (int i = 20; i <= 51 ; ++i) {
         int* blk= cast1(readBlockFromDisk(i, buffer));
         for (int j = 0; j < 14; j+=2) {
             if(blk[j]%5==0){
                 blk0=addElem(buffer,blk0,i0,blk[j],blk[j+1],target0);
             } else if(blk[j]%5==1){
                 blk1=addElem(buffer,blk1,i1,blk[j],blk[j+1],target1);
             } else if(blk[j]%5==2){
                 blk2=addElem(buffer,blk2,i2,blk[j],blk[j+1],target2);
             } else if(blk[j]%5==3){
                 blk3=addElem(buffer,blk3,i3,blk[j],blk[j+1],target3);
             } else{
                 blk4=addElem(buffer,blk4,i4,blk[j],blk[j+1],target4);
             }
         }
         freeBlockInBuffer(cast2(blk), buffer);
     }
     writeLastBlk(buffer,blk0,i0,target0);
     writeLastBlk(buffer,blk1,i1,target1);
     writeLastBlk(buffer,blk2,i2,target2);
     writeLastBlk(buffer,blk3,i3,target3);
     writeLastBlk(buffer,blk4,i4,target4);
 }
 /**
  * 哈希连接总函数
  * @param buffer 就是buffer指针
  * 就从600号开始
  * 分5个桶
  */
void HashJoin(Buffer* buffer){
     HASH_R(buffer);
     HASH_S(buffer);
    res_index_blk_hash=700;
    blk_index_hash=0;
    loopjoin(buffer,600,4,650,7);
    loopjoin(buffer,610,2,660,6);
    loopjoin(buffer,620,4,670,7);
    loopjoin(buffer,630,4,680,9);
    loopjoin(buffer,640,4,690,7);
}



#endif //DATABASE4_1_JOIN_H
