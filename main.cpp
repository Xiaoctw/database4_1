
#include "select.h"
#include "BPlus_tree.h"
#include "join.h"
#include "project.h"
#include "sort.h"
using namespace std;

int main() {
    auto* buffer=new Buffer();
    initBuffer(520,64,buffer);
    for (int i = 301; i <=316 ; ++i) {
        int* block= reinterpret_cast<int *>(readBlockFromDisk(i, buffer));
        for (int j = 0; j < 16; ++j) {
            cout<<block[j]<<" ";
        }
        freeBlockInBuffer(reinterpret_cast<unsigned char *>(block), buffer);
        cout<<endl;
    }
    sort_merge(buffer,301,307,320);
}


/**
 * 线性选择算法,从R中选择A的值为val1的元组
 * @param val1 值
 * @param addr 写入硬盘初始地址
 * @param buffer 就是buffer
 */
