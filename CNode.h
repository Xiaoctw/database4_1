//
// Created by xiao on 19-4-29.
//

#ifndef DATABASE4_1_CNODE_H
#define DATABASE4_1_CNODE_H

#include <vector>

using namespace std;
enum NODE_TYPE{INTERNAL,LEAF};//种类
enum SIBLING_DIRECTION{LEFT,RIGHT};//节点方向,左兄弟和右兄弟
typedef int KeyType;
typedef int DataType;
const int ORDER=3;//B+树的阶
const int MINNUM_KEY=ORDER-1;
const int MAXNUM_KEY=2*ORDER-1;
const int MINNUM_CHILD=MINNUM_KEY+1;//最小子树个数
const int MAXNUM_CHILD=MAXNUM_KEY+1;
const int MINNUM_LEAF=MINNUM_KEY;//最小叶节点键值个数
const int MAXNUM_LEAF=MAXNUM_KEY;//最大叶节点键值个数

class CNode {
public:
    CNode();
    NODE_TYPE getType();
    void setType(NODE_TYPE type);
    virtual ~CNode();
    int getKeyNum();
    void setKeyNum(int n);
    KeyType getKeyValue(int i) const;
    void setKeyValue(int i,KeyType key);
    virtual int getKeyIndex(KeyType key);
    virtual void removeKey(int keyIndex, int childIndex)=0;  // 从结点中移除键值
    virtual void split(CNode* parentNode, int childIndex)=0; // 分裂结点
    virtual void mergeChild(CNode* parentNode, CNode* childNode, int keyIndex)=0;  // 合并结点
    virtual void clear()=0; // 清空结点，同时会清空结点所包含的子树结点
    virtual void borrowFrom(CNode* destNode, CNode* parentNode, int keyIndex, SIBLING_DIRECTION d)=0; // 从兄弟结点中借一个键值
    virtual int getChildIndex(KeyType key, int keyIndex)const=0;  // 根据键值获取孩子结点指针下标

protected:
    NODE_TYPE m_Type;
    int m_KeyNum;
    KeyType m_KeyValues[MAXNUM_KEY];
};
// 内结点
class CInternalNode : public CNode{
public:
    CInternalNode();
    virtual ~CInternalNode();

    CNode* getChild(int i) const {return m_Childs[i];}
    void setChild(int i, CNode* child){m_Childs[i] = child;}
    void insert(int keyIndex, int childIndex, KeyType key, CNode* childNode);
    virtual void split(CNode* parentNode, int childIndex);
    virtual void mergeChild(CNode* parentNode, CNode* childNode, int keyIndex);
    virtual void removeKey(int keyIndex, int childIndex);
    virtual void clear();
    virtual void borrowFrom(CNode* destNode, CNode* parentNode, int keyIndex, SIBLING_DIRECTION d);
    virtual int getChildIndex(KeyType key, int keyIndex)const;
private:
    CNode* m_Childs[MAXNUM_CHILD];
};

// 叶子结点
class CLeafNode : public CNode{
public:
    CLeafNode();
    virtual ~CLeafNode();
    CLeafNode* getLeftSibling() const {return m_LeftSibling;}
    void setLeftSibling(CLeafNode* node){m_LeftSibling = node;}
    CLeafNode* getRightSibling() const {return m_RightSibling;}
    void setRightSibling(CLeafNode* node){m_RightSibling = node;}
    DataType getData(int i) const {return m_Datas[i];}
    void setData(int i, const DataType& data){m_Datas[i] = data;}
    void insert(KeyType key, const DataType& data);
    virtual void split(CNode* parentNode, int childIndex);
    virtual void mergeChild(CNode* parentNode, CNode* childNode, int keyIndex);
    virtual void removeKey(int keyIndex, int childIndex);
    virtual void clear();
    virtual void borrowFrom(CNode* destNode, CNode* parentNode, int keyIndex, SIBLING_DIRECTION d);
    virtual int getChildIndex(KeyType key, int keyIndex)const;
private:
    CLeafNode* m_LeftSibling;
    CLeafNode* m_RightSibling;
    DataType m_Datas[MAXNUM_LEAF];
   // vector<DataType > m_Datas[MAXNUM_LEAF];
};


#endif //DATABASE4_1_CNODE_H
