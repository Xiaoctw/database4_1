
#include "select.h"
#include "BPlus_tree.h"
using namespace std;

int main() {
    auto* buffer=new Buffer();
    initBuffer(520,64,buffer);
//    BinarySearch2(30,90,buffer);
//    LinearSelect2(30,70,buffer);
//    LinearSelect1(30,60,buffer);
//    int* blk1= reinterpret_cast<int *>(readBlockFromDisk(70, buffer));
//    int* blk2= reinterpret_cast<int *>(readBlockFromDisk(90, buffer));
//    for (int i = 0; i < 16; ++i) {
//        cout<<blk1[i]<<" ";
//    }
//    cout<<endl;
//    for (int j = 0; j < 16; ++j) {
//        cout<<blk2[j]<<" ";
//    }
    CBPlusTree cbPlusTree;
    cbPlusTree.insert(3,2);
    cbPlusTree.insert(4,2);
    cbPlusTree.insert(2,4);
    cbPlusTree.insert(2,5);
    cout<<cbPlusTree.search(5)<<endl;
    cout<<cbPlusTree.search(3)<<endl;
    cout<<cbPlusTree.search(2)<<endl;

}


/**
 * 线性选择算法,从R中选择A的值为val1的元组
 * @param val1 值
 * @param addr 写入硬盘初始地址
 * @param buffer 就是buffer
 */
