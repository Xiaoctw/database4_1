
#include "select.h"
#include "BPlus_tree.h"
#include "join.h"
#include "project.h"
#include "sort.h"

using namespace std;

int main() {
    auto* buffer=new Buffer();
    initBuffer(520,64,buffer);
    FILE *fp=fopen("原始数据.txt","w");
    fprintf(fp,"关系R,位于块1-块16:\n");
    for (int i = 1; i <=16 ; ++i) {
        int* blk= cast1(readBlockFromDisk(i, buffer));
        for (int j = 0; j <=15 ; ++j) {
            fprintf(fp,"%d ",blk[j]);
        }
        fprintf(fp,"\n");
        freeBlockInBuffer(cast2(blk),buffer);
    }
    fprintf(fp,"关系S,位于块20-块62:\n");
    for (int i = 20; i <=51 ; ++i) {
        int* blk= cast1(readBlockFromDisk(i, buffer));
        for (int j = 0; j <=15 ; ++j) {
            fprintf(fp,"%d ",blk[j]);
        }
        fprintf(fp,"\n");
        freeBlockInBuffer(cast2(blk),buffer);
    }
    fp=fopen("选择结果.txt","w");
    fprintf(fp,"选择RA为30的元组\n");
    for (int i = 60; i <=62 ; ++i) {
        if (i==60){
            fprintf(fp,"线性索引,60块:\n");
        }else if(i==62){
            fprintf(fp,"B+树索引,62块:\n");
        } else{
            fprintf(fp,"二元搜索,61块:\n");
        }
        int* blk= cast1(readBlockFromDisk(i, buffer));
        for (int j = 0; j <=15 ; ++j) {
            fprintf(fp,"%d ",blk[j]);
        }
        fprintf(fp,"\n");
        freeBlockInBuffer(cast2(blk),buffer);
    }
    fp=fopen("连接结果.txt","w");
    fprintf(fp,"Nest-Loop-Join,从100块到167块:\n");
    for (int i = 100; i <=167 ; ++i) {
        int* blk= cast1(readBlockFromDisk(i, buffer));
        for (int j = 0; j <=15 ; ++j) {
            fprintf(fp,"%d ",blk[j]);
        }
        fprintf(fp,"\n");
        freeBlockInBuffer(cast2(blk),buffer);
    }
    fprintf(fp,"Sort-Merge-Join,从510-558块:\n");
    for (int i = 510; i <=558 ; ++i) {
        int* blk= cast1(readBlockFromDisk(i, buffer));
        for (int j = 0; j <=15 ; ++j) {
            fprintf(fp,"%d ",blk[j]);
        }
        fprintf(fp,"\n");
        freeBlockInBuffer(cast2(blk),buffer);
    }
    fprintf(fp,"Hash-Join,从700-768块:\n");

    for (int i = 700; i <=768 ; ++i) {
        int* blk= cast1(readBlockFromDisk(i, buffer));
        for (int j = 0; j <=15 ; ++j) {
            fprintf(fp,"%d ",blk[j]);
        }
        fprintf(fp,"\n");
        freeBlockInBuffer(cast2(blk),buffer);
    }

    fp=fopen("投影结果.txt","w");
    fprintf(fp,"投影关系R的A属性,位于块200-202之间:\n");
    for (int i = 200; i <=202 ; ++i) {
        int* blk= cast1(readBlockFromDisk(i, buffer));
        for (int j = 0; j <=15 ; ++j) {
            fprintf(fp,"%d ",blk[j]);
        }
        fprintf(fp,"\n");
        freeBlockInBuffer(cast2(blk),buffer);
    }
    fp=fopen("排序结果.txt","w");
    fprintf(fp,"把关系R按照属性A进行排序:\n");
    for (int i = 340; i <=355 ; ++i) {
        int* blk= cast1(readBlockFromDisk(i, buffer));
        for (int j = 0; j <=15 ; ++j) {
            fprintf(fp,"%d ",blk[j]);
        }
        fprintf(fp,"\n");
        freeBlockInBuffer(cast2(blk),buffer);
    }
    fprintf(fp,"把关系S按照属性C进行排序:\n");
    for (int i = 470; i <=501 ; ++i) {
        int* blk= cast1(readBlockFromDisk(i, buffer));
        for (int j = 0; j <=15 ; ++j) {
            fprintf(fp,"%d ",blk[j]);
        }
        fprintf(fp,"\n");
        freeBlockInBuffer(cast2(blk),buffer);
    }
//    int* blk= reinterpret_cast<int *>(readBlockFromDisk(90, buffer));
//        for (int j = 0; j <=15 ; ++j) {
//            cout<<blk[j]<<" ";
//        }
//        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
//    initBuffer(520,64,buffer1);
//    Sort_Merge(buffer,510);
//    LOOPJOIN(buffer1);

    for (int i = 510; i <=558 ; ++i) {
        int* blk= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
        for (int j = 0; j <=15 ; ++j) {
            cout<<blk[j]<<" ";
        }
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
        cout<<endl;
    }
//    cout<<"merge连接"<<buffer->numIO;
//    cout<<"循环连接"<<buffer1->numIO;
      //  HASH_R(buffer);
   // HASH_S(buffer);
  // LOOPJOIN(buffer);
   //HashJoin(buffer);
//    for (int i =600 ; i <=603 ; ++i) {
//        int* blk= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
//        for (int j =0 ; j <=15 ; ++j) {
//            cout<<blk[j]<<" ";
//        }
//        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
//        cout<<endl;
//    }
//    for (int i = 100; i <=167 ; ++i) {
//        int* blk= cast1(readBlockFromDisk(i, buffer));
//        for (int j = 0; j <=15 ; ++j) {
//            cout<<blk[j]<<" ";
//        }
//        freeBlockInBuffer(cast2(blk), buffer);
//        cout<<endl;
//    }
}


/**
 * 线性选择算法,从R中选择A的值为val1的元组
 * @param val1 值
 * @param addr 写入硬盘初始地址
 * @param buffer 就是buffer
 */
