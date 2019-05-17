//
// Created by xiao on 19-4-29.
//

#include <iostream>
#include "extrem.h"
#include "BPlus_tree.h"
#include <random>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <unordered_set>
#include <set>
#include <fstream>
#include <cstring>
#include "BPlus_tree.h"
#ifndef DATABASE4_1_SELECT_H
#define DATABASE4_1_SELECT_H
#define N 50
using namespace std;
/**
 * 线性选择算法,从R中选择A的值为val1的元组
 * @param val1 值
 * @param addr 写入硬盘初始地址
 * @param buffer 就是buffer
 */
void LinearSelect1(int val1,int addr,Buffer* buffer){
    int totalNumBlock=16;
    int K=7;
    int i=1;
    int* blk=cast1(getNewBlockInBuffer(buffer));
    int index=0;
    while (totalNumBlock>0){
        int num=min(totalNumBlock,K);
        totalNumBlock-=num;//总的数量减少K个
        int* blocks[num];
        for (int j = 0; j < num; ++j) {
            blocks[j]= cast1(readBlockFromDisk(i + j, buffer));
            //  memset(blocks[j],0, sizeof(blocks[j]));
        }
        i+=num;//跟进起始位置
        for (int j = 0; j < num; ++j) {
            int* block=blocks[j];
            for (int k = 0; k < 14; k+=2) {
                if(block[k]==val1){
                    blk[index]=block[k];
                    blk[index+1]=block[k+1];
                    index+=2;
                }
                if(index==14){
                    blk[index]=0;
                    blk[index+1]=addr+1;
                    writeBlockToDisk(cast2(blk), addr, buffer);//进行写入操作
                    freeBlockInBuffer(cast2(blk), buffer);
                    addr++;
                    index=0;
                    blk= cast1(getNewBlockInBuffer(buffer));
                }
            }
        }
        for (int j = 0; j < num; ++j) {
            blk[14]=0;
            blk[15]=addr+1;
            freeBlockInBuffer(cast2(blocks[j]), buffer);
        }
    }
    if(index!=0){
        for (int j = index; j <=15 ; ++j) {
            blk[j]=0;
        }
        writeBlockToDisk(cast2(blk), addr, buffer);
    }
}

/**
 * 现行线性选择算法,从S中选择C的值为val的元祖
 * @param val 值
 * @param addr 初始的地址
 * @param buffer 就是buffer
 */
void LinearSelect2(int val,int addr,Buffer* buffer){
    int totalNumBlock=32;
    int K=7;//每个块中元组个数
    int i=20;//初始位置
    int* blk= cast1(getNewBlockInBuffer(buffer));
    int index=0;//设置写入磁盘的块对应的下标
    while (totalNumBlock>0){
        int num=min(totalNumBlock,K);
        totalNumBlock-=num;//总的数量减少K个
        int* blocks[num];
        for (int j = 0; j < num; ++j) {
            blocks[j]= cast1(readBlockFromDisk(i + j, buffer));
        }
        i+=num;//跟进起始位置
        for (int j = 0; j < num; ++j) {
            int* block=blocks[j];
            for (int k = 0; k < 14; k+=2) {
                if(block[k]==val){
                    blk[index]=block[k];
                    blk[index+1]=block[k+1];
                    index+=2;
                }
                if(index==14){
                    blk[index]=0;
                    blk[index+1]=addr+1;
                    writeBlockToDisk(cast2(blk), addr, buffer);//进行写入操作
                    freeBlockInBuffer(cast2(blk), buffer);
                    addr++;
                    index=0;
                    blk= cast1(getNewBlockInBuffer(buffer));
                }
            }
        }
        for (int j = 0; j < num; ++j) {
            freeBlockInBuffer(cast2(blocks[j]), buffer);
        }
    }
    if(index!=0){
        for (int j = index; j <=15 ; ++j) {
            blk[j]=0;
        }
        writeBlockToDisk(cast2(blk), addr, buffer);
    }
}

/**
 * 二分搜索算法,在关系1中搜索A的值为val的元组
 * @param val
 * @param addr
 * @param buffer
 */
vector<int> split(string& str,string& pattern){
    vector<int > vector1;
    str+=pattern;
    int pos=str.find(pattern);
    int size=str.size();
    while (pos!=string::npos){
        string string1=str.substr(0,pos);
        if (string1.empty()){
            break;
        }
        vector1.push_back(atoi(string1.c_str()));
        str=str.substr(pos+1,size);
        pos=str.find(pattern);
    }
    return vector1;
}


/**
 * 二分搜索算法,从340-355中选择出A的值为给定的值的所有元素
 * @param buffer
 * @param val 给定的值
 * @param addr 放到这里
 */
void BinarySearch(Buffer* buffer,int val,int addr){
    int lo=340,hi=355;
    int target=-1;
    while (lo<=hi){
        int mid=(lo+hi)/2;
        if(mid<hi){
            int* blk1= reinterpret_cast<int *>(readBlockFromDisk(mid, buffer));
            int* blk2= reinterpret_cast<int *>(readBlockFromDisk(mid+1, buffer));
            if(blk1[0]<=val&&blk2[0]>=val){
                target=mid;
                break;
            } else if(blk1[0]>val){
                hi=mid-1;
            } else{
                lo=mid+1;
            }
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk1), buffer);
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk2), buffer);
        } else{
            target=mid;
            break;
        }
    }
    int* resblk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    int index=0;
    for (int i = target; i <=355 ; ++i) {
        int* blk= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
        for (int j = 0; j < 14; j+=2) {
            if(blk[j]==val){
               // cout<<blk[j]<<" "<<blk[j+1]<<" ";
               resblk[index]=blk[j];
               resblk[index+1]=blk[j+1];
               index+=2;
               if(index==14){
                   resblk[index]=0;
                   resblk[index+1]=addr+1;
                   writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), addr, buffer);
                   addr++;
                   index=0;
               }
            }
            if(blk[j]>val){
                if(index!=0){
                    for (int k = index; k < 16; ++k) {
                        resblk[k]=0;
                    }
                    writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), addr, buffer);
                }
                return;
            }
        }
    }
}
/**
 * 建立一颗B+树
 * @return 返回一个B+树
 */
CBPlusTree* makeTree(Buffer* buffer){
    auto* tree=new CBPlusTree();
    tree->insert(1,4);
    tree->insert(2,4);
    tree->insert(3,2);
    int tem=0;
    bool  first= true;
    for (int i = 340; i <=355 ; ++i) {
        int* blk= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
        for (int j = 0; j < 14; j+=2) {
            if(first){
                first= false;
                tem=blk[j];
                tree->insert(tem,i);
            } else{
                if(blk[j]!=tem){
                    tem=blk[j];
                    tree->insert(tem,i);
                }
            }
        }
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
    }
    return tree;
}
/**
 * 利用B+树进行建立索引,并且查找操作
 * @param buffer 缓冲区
 * @param val 值
 * @param addr 存放地址
 */
void SearchBPlusTree(Buffer* buffer,int val,int addr){
    auto tree=makeTree(buffer);
    auto res=tree->selectArrange(val,val);
    int beg=res[0];
    int* resblk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    int index=0;
    for (int i = beg; i <=355 ; ++i) {
        int* blk= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
        for (int j = 0; j < 14; j+=2) {
            if(blk[j]==val){
                // cout<<blk[j]<<" "<<blk[j+1]<<" ";
                resblk[index]=blk[j];
                resblk[index+1]=blk[j+1];
                index+=2;
                if(index==14){
                    resblk[index]=0;
                    resblk[index+1]=addr+1;
                    writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), addr, buffer);
                    addr++;
                    index=0;
                }
            }
            if(blk[j]>val){
                if(index!=0){
                    for (int k = index; k < 16; ++k) {
                        resblk[k]=0;
                    }
                    writeBlockToDisk(reinterpret_cast<unsigned char *>(resblk), addr, buffer);
                }
                return;
            }
        }
    }
}









#endif //DATABASE4_1_SELECT_H
