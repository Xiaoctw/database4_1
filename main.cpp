
#include "select.h"
#include "BPlus_tree.h"
#include "join.h"
#include "project.h"
#include "sort.h"

using namespace std;

int main() {
    auto* buffer=new Buffer();
    initBuffer(520,64,buffer);
//    initBuffer(520,64,buffer1);
//    Sort_Merge(buffer,510);
//    loopJoin(buffer1);
//    for (int i = 510; i <=558 ; ++i) {
//        int* blk= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
//        for (int j = 0; j <=15 ; ++j) {
//            cout<<blk[j]<<" ";
//        }
//        freeBlockInBuffer(reinterpret_cast<unsigned char *>(blk), buffer);
//        cout<<endl;
//    }
//    cout<<"merge连接"<<buffer->numIO;
//    cout<<"循环连接"<<buffer1->numIO;
      //  HashJoinR(buffer);
      HashJoinS(buffer);
}


/**
 * 线性选择算法,从R中选择A的值为val1的元组
 * @param val1 值
 * @param addr 写入硬盘初始地址
 * @param buffer 就是buffer
 */
