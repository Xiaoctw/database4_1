//
// Created by xiao on 19-4-29.
//

#ifndef DATABASE4_1_BPLUS_TREE_H
#define DATABASE4_1_BPLUS_TREE_H
#include <iostream>
#include "BTreeNode.h"
#include <algorithm>
#include <vector>
using namespace std;

struct SelectResult
{
    int keyIndex;
    CLeafNode* targetNode;
};

class CBPlusTree{
public:
    CBPlusTree();
    ~CBPlusTree();
    bool insert(KeyType key, const DataType& data);
    // 范围查询，BETWEEN
    vector<DataType> selectArrange(KeyType smallKey, KeyType largeKey);
    bool search(KeyType key); // 查找是否存在
    void clear();             // 清空
private:
    void recursive_insert(BTreeNode* parentNode, KeyType key, const DataType& data);
    void recursive_remove(BTreeNode* parentNode, KeyType key);
    bool recursive_search(BTreeNode *pNode, KeyType key)const;
    void changeKey(BTreeNode *pNode, KeyType oldKey, KeyType newKey);
    void search(KeyType key, SelectResult& result);
    void recursive_search(BTreeNode* pNode, KeyType key, SelectResult& result);
    void recursive_remove(BTreeNode* parentNode, KeyType key, DataType& dataValue);
private:
    BTreeNode* m_Root;//根节点
    CLeafNode* m_DataHead;//最左端叶子节点
    KeyType m_MaxKey{};  // B+树中的最大键

};

CBPlusTree::CBPlusTree(){
    m_Root = nullptr;
    m_DataHead = nullptr;
}

CBPlusTree::~CBPlusTree(){
    clear();
}

bool CBPlusTree::insert(KeyType key, const DataType& data){
    // 是否已经存在
//    if (search(key))
//    {
//        return false;
//    }
    // 找到可以插入的叶子结点，否则创建新的叶子结点
    if(m_Root==nullptr)
    {
        m_Root = new CLeafNode();
        m_DataHead = (CLeafNode*)m_Root;
        m_MaxKey = key;
    }
    if (m_Root->getKeyNum()>=MAXNUM_KEY) // 根结点已满，分裂
    {
        auto* newNode = new CInternalNode();  //创建新的根节点
        newNode->setChild(0, m_Root);
        m_Root->split(newNode, 0);    // 叶子结点分裂
        m_Root = newNode;  //更新根节点指针
    }
    if (key>m_MaxKey)  // 更新最大键值
    {
        m_MaxKey = key;
    }
    recursive_insert(m_Root, key, data);
    return true;
}

void CBPlusTree::recursive_insert(BTreeNode* parentNode, KeyType key, const DataType& data)
{
    if (parentNode->getType()==LEAF)  // 叶子结点，直接插入
    {
        ((CLeafNode*)parentNode)->insert(key, data);
    }
    else
    {
        // 找到子结点
        int keyIndex = parentNode->getKeyIndex(key);
        int childIndex= parentNode->getChildIndex(key, keyIndex); // 孩子结点指针索引
        auto childNode = ((CInternalNode*)parentNode)->getChild(childIndex);
        if (childNode->getKeyNum()>=MAXNUM_LEAF)  // 子结点已满，需进行分裂
        {
            childNode->split(parentNode, childIndex);
            if (parentNode->getKeyValue(childIndex)<=key)   // 确定目标子结点
            {
                childNode = ((CInternalNode*)parentNode)->getChild(childIndex+1);
            }
        }
        recursive_insert(childNode, key, data);
    }
}

void CBPlusTree::clear()
{
    if (m_Root!=nullptr)
    {
        m_Root->clear();
        delete m_Root;
        m_Root = nullptr;
        m_DataHead = nullptr;
    }
}

bool CBPlusTree::search(KeyType key)
{
    return recursive_search(m_Root, key);
}

bool CBPlusTree::recursive_search(BTreeNode *pNode, KeyType key)const
{
    if (pNode==nullptr)  //检测节点指针是否为空，或该节点是否为叶子节点
    {
        return false;
    }
    else
    {
        int keyIndex = pNode->getKeyIndex(key);
        int childIndex = pNode->getChildIndex(key, keyIndex); // 孩子结点指针索引
        if (keyIndex<pNode->getKeyNum() && key==pNode->getKeyValue(keyIndex))
        {
            return true;
        }
        else
        {
            if (pNode->getType()==LEAF)   //检查该节点是否为叶子节点
            {
                return false;
            }
            else
            {
                return recursive_search(((CInternalNode*)pNode)->getChild(childIndex), key);
            }
        }
    }
}


// parentNode中包含的键值数>MINNUM_KEY
void CBPlusTree::recursive_remove(BTreeNode* parentNode, KeyType key)
{
    int keyIndex = parentNode->getKeyIndex(key);
    int childIndex= parentNode->getChildIndex(key, keyIndex); // 孩子结点指针索引
    if (parentNode->getType()==LEAF)// 找到目标叶子节点
    {
        if (key==m_MaxKey&&keyIndex>0)
        {
            m_MaxKey = parentNode->getKeyValue(keyIndex-1);
        }
        parentNode->removeKey(keyIndex, childIndex);  // 直接删除
        // 如果键值在内部结点中存在，也要相应的替换内部结点
        if (childIndex==0 && m_Root->getType()!=LEAF && parentNode!=m_DataHead)
        {
            changeKey(m_Root, key, parentNode->getKeyValue(0));
        }
    }
    else // 内结点
    {
        BTreeNode *pChildNode = ((CInternalNode*)parentNode)->getChild(childIndex); //包含key的子树根节点
        if (pChildNode->getKeyNum()==MINNUM_KEY)                       // 包含关键字达到下限值，进行相关操作
        {
            BTreeNode *pLeft = childIndex>0 ? ((CInternalNode*)parentNode)->getChild(childIndex-1) : nullptr;                       //左兄弟节点
            BTreeNode *pRight = childIndex<parentNode->getKeyNum() ? ((CInternalNode*)parentNode)->getChild(childIndex+1) : nullptr;//右兄弟节点
            // 先考虑从兄弟结点中借
            if (pLeft && pLeft->getKeyNum()>MINNUM_KEY)// 左兄弟结点可借
            {
                pChildNode->borrowFrom(pLeft, parentNode, childIndex-1, LEFT);
            }
            else if (pRight && pRight->getKeyNum()>MINNUM_KEY)//右兄弟结点可借
            {
                pChildNode->borrowFrom(pRight, parentNode, childIndex, RIGHT);
            }
                //左右兄弟节点都不可借，考虑合并
            else if (pLeft)                    //与左兄弟合并
            {
                pLeft->mergeChild(parentNode, pChildNode, childIndex-1);
                pChildNode = pLeft;
            }
            else if (pRight)                   //与右兄弟合并
            {
                pChildNode->mergeChild(parentNode, pRight, childIndex);
            }
        }
        recursive_remove(pChildNode, key);
    }
}

void CBPlusTree::changeKey(BTreeNode *pNode, KeyType oldKey, KeyType newKey)
{
    if (pNode!=nullptr && pNode->getType()!=LEAF)
    {
        int keyIndex = pNode->getKeyIndex(oldKey);
        if (keyIndex<pNode->getKeyNum() && oldKey==pNode->getKeyValue(keyIndex))  // 找到
        {
            pNode->setKeyValue(keyIndex, newKey);
        }
        else   // 继续找
        {
            changeKey(((CInternalNode*)pNode)->getChild(keyIndex), oldKey, newKey);
        }
    }
}

void CBPlusTree::recursive_remove(BTreeNode* parentNode, KeyType key, DataType& dataValue)
{
    int keyIndex = parentNode->getKeyIndex(key);
    int childIndex= parentNode->getChildIndex(key, keyIndex); // 孩子结点指针索引
    if (parentNode->getType()==LEAF)// 找到目标叶子节点
    {
        if (key==m_MaxKey&&keyIndex>0)
        {
            m_MaxKey = parentNode->getKeyValue(keyIndex-1);
        }
        dataValue = ((CLeafNode*)parentNode)->getData(keyIndex);
        parentNode->removeKey(keyIndex, childIndex);  // 直接删除
        // 如果键值在内部结点中存在，也要相应的替换内部结点
        if (childIndex==0 && m_Root->getType()!=LEAF && parentNode!=m_DataHead)
        {
            changeKey(m_Root, key, parentNode->getKeyValue(0));
        }
    }
    else // 内结点
    {
        auto pChildNode = ((CInternalNode*)parentNode)->getChild(childIndex); //包含key的子树根节点
        if (pChildNode->getKeyNum()==MINNUM_KEY)                       // 包含关键字达到下限值，进行相关操作
        {
            auto pLeft = childIndex > 0 ? ((CInternalNode*)parentNode)->getChild(childIndex - 1) : nullptr;                       //左兄弟节点
            auto pRight = childIndex < parentNode->getKeyNum() ? ((CInternalNode*)parentNode)->getChild(childIndex + 1) : nullptr;//右兄弟节点
            // 先考虑从兄弟结点中借
            if (pLeft && pLeft->getKeyNum()>MINNUM_KEY)// 左兄弟结点可借
            {
                pChildNode->borrowFrom(pLeft, parentNode, childIndex-1, LEFT);
            }
            else if (pRight && pRight->getKeyNum()>MINNUM_KEY)//右兄弟结点可借
            {
                pChildNode->borrowFrom(pRight, parentNode, childIndex, RIGHT);
            }
                //左右兄弟节点都不可借，考虑合并
            else if (pLeft)                    //与左兄弟合并
            {
                pLeft->mergeChild(parentNode, pChildNode, childIndex-1);
                pChildNode = pLeft;
            }
            else if (pRight)                   //与右兄弟合并
            {
                pChildNode->mergeChild(parentNode, pRight, childIndex);
            }
        }
        recursive_remove(pChildNode, key, dataValue);
    }
}

vector<DataType> CBPlusTree::selectArrange(KeyType smallKey, KeyType largeKey)
{
    vector<DataType> results;
    if (smallKey<=largeKey)
    {
        SelectResult start{}, end{};
        search(smallKey, start);
        search(largeKey, end);
        CLeafNode* itr = start.targetNode;
        int i = start.keyIndex;
        if (itr->getKeyValue(i)<smallKey)
        {
            ++i;
        }
        if (end.targetNode->getKeyValue(end.keyIndex)>largeKey)
        {
            --end.keyIndex;
        }
        while (itr!=end.targetNode)
        {
            for (; i<itr->getKeyNum(); ++i)
            {
                results.push_back(itr->getData(i));
            }
            itr = itr->getRightSibling();
            i = 0;
        }
        for (; i<=end.keyIndex; ++i)
        {
            results.push_back(itr->getData(i));
        }
    }
    sort<vector<DataType>::iterator>(results.begin(), results.end());
    return results;
}

void CBPlusTree::search(KeyType key, SelectResult& result)
{
    recursive_search(m_Root, key, result);
}

void CBPlusTree::recursive_search(BTreeNode* pNode, KeyType key, SelectResult& result)
{
    int keyIndex = pNode->getKeyIndex(key);
    int childIndex = pNode->getChildIndex(key, keyIndex); // 孩子结点指针索引
    if (pNode->getType()==LEAF)
    {
        result.keyIndex = keyIndex;
        result.targetNode = (CLeafNode*)pNode;
        return;
    }
    else
    {
        return recursive_search(((CInternalNode*)pNode)->getChild(childIndex), key, result);
    }
}


#endif
