
#include "select.h"
#include "BPlus_tree.h"
#include "join.h"
#include "project.h"
#include "sort.h"

using namespace std;

int main() {
    auto* buffer=new Buffer();
    initBuffer(520,64,buffer);
//    int* blk= reinterpret_cast<int *>(readBlockFromDisk(60, buffer));
//    for (int i = 0; i < 14; i+=2) {
//        cout<<blk[i]<<" "<<blk[i+1]<<" ";
//    }
//    cout<<endl;
//    BinarySearch(buffer,30,61);
    int* resblk= reinterpret_cast<int *>(readBlockFromDisk(61, buffer));
    for (int i = 0; i < 14; i+=2) {
        cout<<resblk[i]<<" "<<resblk[i+1]<<" ";
    }
    cout<<endl;
    SearchBPlusTree(buffer,30,62);
    resblk= reinterpret_cast<int *>(readBlockFromDisk(62, buffer));
    for (int i = 0; i < 14; i+=2) {
        cout<<resblk[i]<<" "<<resblk[i+1]<<" ";
    }
}


/**
 * 线性选择算法,从R中选择A的值为val1的元组
 * @param val1 值
 * @param addr 写入硬盘初始地址
 * @param buffer 就是buffer
 */
