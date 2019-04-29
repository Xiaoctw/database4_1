//
// Created by xiao on 19-4-29.
//

#include <iostream>
#include "extrem.h"
#include <random>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <unordered_set>
#include <set>
#include <fstream>
#include <cstring>
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
    int* blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    int index=0;
    while (totalNumBlock>0){
        int num=min(totalNumBlock,K);
        totalNumBlock-=num;//总的数量减少K个
        int* blocks[num];
        for (int j = 0; j < num; ++j) {
            blocks[j]= reinterpret_cast<int *>(readBlockFromDisk(i + j, buffer));
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
                    writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), addr, buffer);//进行写入操作
                    freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
                    addr++;
                    index=0;
                    blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
                }
            }
        }
        for (int j = 0; j < num; ++j) {
            blk[14]=0;
            blk[15]=addr+1;
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blocks[j]), buffer);
        }
    }
    if(index!=0){
        for (int j = index; j <=15 ; ++j) {
            blk[j]=0;
        }
        writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), addr, buffer);
    }
}

void LinearSelect2(int val,int addr,Buffer* buffer){
    int totalNumBlock=32;
    int K=7;//每个块中元组个数
    int i=20;//初始位置
    int* blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    int index=0;//设置写入磁盘的块对应的下标
    while (totalNumBlock>0){
        int num=min(totalNumBlock,K);
        totalNumBlock-=num;//总的数量减少K个
        int* blocks[num];
        for (int j = 0; j < num; ++j) {
            blocks[j]= reinterpret_cast<int *>(readBlockFromDisk(i + j, buffer));
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
                    writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), addr, buffer);//进行写入操作
                    freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
                    addr++;
                    index=0;
                    blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
                }
            }
        }
        for (int j = 0; j < num; ++j) {
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blocks[j]), buffer);
        }
    }
    if(index!=0){
        for (int j = index; j <=15 ; ++j) {
            blk[j]=0;
        }
        writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), addr, buffer);
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

void makeIndex1(Buffer* buffer){
    map<int,set<int>> indexes;
    for (int i = 1; i <= 16 ; ++i) {
        int* block= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
        for (int j = 0; j < 14; j+=2) {
            int i1=block[j];
            indexes[i1].insert(i);
        }
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(block), buffer);
    }
    FILE* fp=fopen("关系R索引文件","w");
    for(const auto& pair:indexes){
        fprintf(fp,"%d ",pair.first);
        for(auto x:pair.second){
            fprintf(fp,"%d ",x);
        }
        fprintf(fp,"\n");
    }
}
void makeIndex2(Buffer* buffer){
    map<int,set<int>> indexes;
    for (int i = 20; i <= 51 ; ++i) {
        int* block= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
        for (int j = 0; j < 14; j+=2) {
            int i1=block[j];
            indexes[i1].insert(i);
        }
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(block), buffer);
    }
    FILE* fp=fopen("关系S索引文件","w");
    for(const auto& pair:indexes){
        fprintf(fp,"%d ",pair.first);
        for(auto x:pair.second){
            fprintf(fp,"%d ",x);
        }
        fprintf(fp,"\n");
    }
}

void BinarySearch1(int val, int beg_blk,Buffer* buffer){
    vector<vector<int>> vectors ;
//    FILE* fp=fopen("关系R索引文件","r");
    ifstream in("关系R索引文件");
    string line;
    string pattern=" ";
    if(in){
        while (getline(in,line)){
            vector<int > vector1=split(line, pattern);
            vectors.push_back(vector1);
        }
    }
    int end=vectors.size()-1,beg=0;
    vector<int> vector1;
    while (beg<=end){
        int mid=(beg+end)/2;
        if(vectors[mid][0]==val){
            vector1= vectors[mid];
            break;
        }else if(vectors[mid][0]>val){
            end=mid-1;
        } else{
            beg=mid+1;
        }
    }
    if(beg>end){
        cout<<"没有找到对应的块"<<endl;
        return;
    }
    vector<pair<int,int>> pairs;
    for (int i = 1; i < vector1.size(); ++i) {
        int num_blk=vector1[i];
        int* blk= reinterpret_cast<int *>(readBlockFromDisk(num_blk, buffer));
        for (int j = 0; j < 14; j+=2) {
            if(blk[j]==val){
                pairs.emplace_back(blk[j],blk[j+1]);
            }
        }
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
    }
    int index=0;
    int* blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    for (auto & pair : pairs) {
        blk[index]=pair.first;
        blk[index+1]=pair.second;
        index+=2;
        if(index==14){
            blk[15]=beg_blk+1;
            beg_blk++;
            writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), beg_blk, buffer);
            index=0;
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
            blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
        }
    }
    if(index!=0){
        for (int i = index; i <= 15; ++i) {
            blk[i]=0;
        }
        writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), beg_blk, buffer);
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
    }
}
void BinarySearch2(int val, int beg_blk,Buffer* buffer){
    vector<vector<int>> vectors ;
//    FILE* fp=fopen("关系R索引文件","r");
    ifstream in("关系S索引文件");
    string line;
    string pattern=" ";
    if(in){
        while (getline(in,line)){
            vector<int > vector1=split(line, pattern);
            vectors.push_back(vector1);
        }
    }
    int end=vectors.size()-1,beg=0;
    vector<int> vector1;
    while (beg<=end){
        int mid=(beg+end)/2;
        if(vectors[mid][0]==val){
            vector1= vectors[mid];
            break;
        }else if(vectors[mid][0]>val){
            end=mid-1;
        } else{
            beg=mid+1;
        }
    }
    if(beg>end){
        cout<<"没有找到对应的块"<<endl;
        return;
    }
    vector<pair<int,int>> pairs;
    for (int i = 1; i < vector1.size(); ++i) {
        int num_blk=vector1[i];
        int* blk= reinterpret_cast<int *>(readBlockFromDisk(num_blk, buffer));
        for (int j = 0; j < 14; j+=2) {
            if(blk[j]==val){
                pairs.emplace_back(blk[j],blk[j+1]);
            }
        }
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
    }
    int index=0;
    int* blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
    for (auto & pair : pairs) {
        blk[index]=pair.first;
        blk[index+1]=pair.second;
        index+=2;
        if(index==14){
            blk[15]=beg_blk+1;
            beg_blk++;
            writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), beg_blk, buffer);
            index=0;
            freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
            blk= reinterpret_cast<int *>(getNewBlockInBuffer(buffer));
        }
    }
    if(index!=0){
        for (int i = index; i <= 15; ++i) {
            blk[i]=0;
        }
        writeBlockToDisk(reinterpret_cast<unsigned char *>(blk), beg_blk, buffer);
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
    }
}





#endif //DATABASE4_1_SELECT_H
