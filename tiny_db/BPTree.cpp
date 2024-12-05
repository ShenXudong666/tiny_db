#pragma once
#include "BPTree.h"
#include "rwdata.h"
#include "stdio.h"
#include "stdlib.h"
#include <climits>
#include <cstring>
#include <iostream>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <vector>

#define DB_HEAD_SIZE 4096 // head size must be pow of 2! 文件数据库的头大小


/**
 * @brief 存储数据对齐方式
 */
#define DB_ALIGNMENT 16
#define db_align(d, a) (((d) + (a - 1)) & ~(a - 1))

#define ceil(M) (((M) - 1) / 2)

 /**
  * @brief 数据块类型
  */
#define TYPE_KEY 0
#define TYPE_VALUE 1



CNode::CNode(const char* filename, KEY_KIND key_kind, size_t max_size,off_t offt)
{
    memcpy(this->fname, filename, strlen((char*)filename));
    this->fname[strlen((char*)filename)] = '\0';
    this->key_kind = key_kind;
    this->max_size = max_size;
    this->offt_self = offt;
    node_Type = NODE_TYPE_LEAF;
    m_Count = 0;
    m_pFather = NULL;
}
CNode::~CNode()
{
    DeleteChildren();
}

// 获取一个最近的兄弟结点
CNode* CNode::GetBrother(int& flag)
{
    CNode* pFather = GetFather();   //获取其父结点指针
    if (NULL == pFather)
    {
        return NULL;
    }

    CNode* pBrother = NULL;
    for (int i = 1; i <= pFather->GetCount() + 1; i++)   //GetCount()表示获取数据或关键字数，要比指针数小1。
    {
        // 找到本结点的位置
        if (pFather->GetPointer(i)->getPtSelf() == this->offt_self)
        {
            if (i == (pFather->GetCount() + 1))   //表示其为父结点的最右边孩子。
            {
                pBrother = pFather->GetPointer(i - 1);    // 本身是最后一个指针，只能找前一个指针
                flag = FLAG_LEFT;
            }
            else
            {
                pBrother = pFather->GetPointer(i + 1);    // 优先找后一个指针
                flag = FLAG_RIGHT;
            }
        }
    }

    return pBrother;
}

// 递归删除子结点
void CNode::DeleteChildren()   // 疑问：这里的指针下标是否需要从0开始
{
    //这里的+1是我自己添加的fixbug
    for (int i = 1; i <= GetCount()+1; i++)   //GetCount()为返回结点中关键字即数据的个数
    {
        CNode* pNode = GetPointer(i);
        if (NULL != pNode)    // 叶子结点没有指针
        {
            pNode->DeleteChildren();
        }
        //this->FreeBlock(); // 删除结点
        delete pNode;
    }
}
CNode* CNode::GetFather(){
    if(this->offt_father<=LOC_GRAPH||this->offt_father>NUM_ALL_BLOCK) return NULL;
    char type=FileManager::getInstance()->get_BlockType(this->fname, this->offt_father);
    if(type==BLOCK_INTER)return new CInternalNode(this->fname, this->key_kind, this->max_size, this->offt_father);

    else if(type==BLOCK_LEAF)return new CLeafNode(this->fname, this->key_kind, this->max_size, this->offt_father);
    else {
        this->offt_father=INVALID;
        return NULL;
    }
    
}


//将内部节点的关键字和指针分别初始化为0和空
CInternalNode::CInternalNode(const char* filename, KEY_KIND key_kind, size_t max_size,off_t offt):CNode(filename,  key_kind,  max_size,  offt)
{

    node_Type = NODE_TYPE_INTERNAL;
    //其实这里的offt有点不安全，后面可能会修改
    for(int i=0; i < MAXNUM_KEY; i++) {
        this->m_Keys[i] = Invalid(this->key_kind);
    }
    for(int i = 0; i < MAXNUM_POINTER; i++)
    {
        this->offt_pointers[i] = INT_MIN;
    }
    if(this->offt_self != NEW_OFFT){
        this->get_file();
    }
    else{
        this->offt_self=FileManager::getInstance()->getFreeBlock(filename, BLOCK_INTER);
        
    
    }
    FileManager::getInstance()->flushBlock(fname, this->offt_self, BLOCK_INTER);

}
// CInternalNode::CInternalNode()
// {
    
//     node_Type = NODE_TYPE_INTERNAL;

//     int i = 0;
//     for (int i=0; i < MAXNUM_KEY; i++) {
//         this->m_Keys[i] = new int();
//     }

//     for (i = 0; i < MAXNUM_POINTER; i++)
//     {
//         this->offt_pointers[i] = NULL;
//     }


// }
CInternalNode::~CInternalNode()
{
    for (int i = 0; i < MAXNUM_POINTER; i++)
    {
        this->offt_pointers[i] = NULL;
    }
}
CNode* CInternalNode::GetPointer(int i)
{
    if ((i > 0) && (i <= MAXNUM_POINTER))
    {
            //这里后面再改为指针读取
        char type=FileManager::getInstance()->get_BlockType(this->fname, this->offt_pointers[i - 1]);
        if(type==BLOCK_INTER)return new CInternalNode(this->fname, this->key_kind, this->max_size, this->offt_pointers[i - 1]);
        else if(type==BLOCK_LEAF)return new CLeafNode(this->fname, this->key_kind, this->max_size, this->offt_pointers[i - 1]);
        else return NULL;
    }
        
        
    return NULL;
        
}
// 在中间结点中插入键。
/*疑问：中间结点需要插入值吗？在插入值时，通常都是先找到在叶子结点中的位置，然后再插入。
中间结点通常当叶子结点需要分裂时将分裂后的两个孩子结点插入其中*/
bool CInternalNode::Insert(void* value, CNode* pNode)
{
    int i;
    // 如果中间结点已满，直接返回失败
    if (GetCount() >= MAXNUM_KEY)
    {
        return false;
    }

    int j = 0;

    // 找到要插入键的位置,这里的比较规则要改,这里相当于大于等于
    for (i = 0;  (i < m_Count)&&(cmp(value , m_Keys[i],this->key_kind)||eql(value , m_Keys[i],this->key_kind)) ; i++)
    {
    }

    // 当前位置及其后面的键依次后移，空出当前位置
    for (j = m_Count; j > i; j--)
    {
        m_Keys[j] = (void*)(new int(*(int*)m_Keys[j - 1]));
    }

    // 当前位置及其后面的指针依次后移
    for (j = m_Count + 1; j > i + 1; j--)
    {
        this->offt_pointers[j] = this->offt_pointers[j - 1];
    }

    // 把键和指针存入当前位置
    m_Keys[i] = value;
    this->offt_pointers[i + 1] = pNode->getPtSelf();    // 注意是第i+1个指针而不是第i个指针
    pNode->SetFather(this);      // 非常重要  该函数的意思是插入关键字value及其所指向子树
    pNode->flush_file();
    m_Count++;


    // 返回成功
    return true;
}

// 在中间结点中删除键，以及该键后的指针
bool CInternalNode::Delete(void* key)
{
    int i, j, k;
    for (i = 0; (i < m_Count)&&(cmp(key ,m_Keys[i],this->key_kind)||eql(key , m_Keys[i],this->key_kind))  ; i++)
    {
    }

    for (j = i - 1; j < m_Count - 1; j++)
    {
        m_Keys[j] = m_Keys[j + 1];
    }
    m_Keys[j] = Invalid(this->key_kind);

    for (k = i; k < m_Count; k++)
    {
        this->offt_pointers[k] = this->offt_pointers[k + 1];
    }
    this->offt_pointers[k] = NULL;

    m_Count--;
    this->flush_file();
    return true;
}

/* 分裂中间结点
分裂中间结点和分裂叶子结点完全不同，因为中间结点不仅有2V键，还有2V+1指针，如果单纯地一分为2，指针将无法分 配。
因此根据http://www.seanster.com/BplusTree/BplusTree.html ，分裂中 间结点的算法是：
根据要插入的键key判断：
(1)如果key小于第V个键，则把第V个键提出来,其左右的键分别分到两个结点中
(2) 如果key大于第V+1个键，则把第V+1个键提出来,其左右的键分别分到两个结点中
(3)如果key介于第V和V+1个键之间，则把key作为 要提出的键，原来的键各分一半到两个结点中
提出来的RetKey作用是便于后续插入到祖先结点
*/
void* CInternalNode::Split(CInternalNode* pNode, void* key)  //key是新插入的值，pNode是分裂结点
{
    int i = 0, j = 0;

    // 如果要插入的键值在第V和V+1个键值中间，需要翻转一下，因此先处理此情况
    if (cmp(key , this->GetElement(ORDER_V),this->key_kind) && cmp( this->GetElement(ORDER_V + 1),key,this->key_kind))
    {
        // 把第V+1 -- 2V个键移到指定的结点中

        for (i = ORDER_V + 1; i <= MAXNUM_KEY; i++)
        {
            j++;
            pNode->SetElement(j, this->GetElement(i));
            this->SetElement(i, Invalid(this->key_kind));
        }

        // 把第V+2 -- 2V+1个指针移到指定的结点中
        j = 0;
        for (i = ORDER_V + 2; i <= MAXNUM_POINTER; i++)
        {
            j++;
            this->GetPointer(i)->SetFather(pNode);    // 重新设置子结点的父亲
            pNode->SetPointer(j, this->GetPointer(i));
            this->SetPointer(i, INVALID);
        }

        // 设置好Count个数
        this->SetCount(ORDER_V);
        pNode->SetCount(ORDER_V);

        // 把原键值返回
        return key;
    }

    // 以下处理key小于第V个键值或key大于第V+1个键值的情况

    // 判断是提取第V还是V+1个键
    int position = 0;
    // if (key < this->GetElement(ORDER_V))
    // {
    //     position = ORDER_V;
    // }
    if(cmp(this->GetElement(ORDER_V + 1),key,this->key_kind)){
        position = ORDER_V;
    }
    else
    {
        position = ORDER_V + 1;
    }

    // 把第position个键提出来，作为新的键值返回
    void* RetKey = this->GetElement(position);

    // 把第position+1 -- 2V个键移到指定的结点中
    j = 0;
    for (i = position + 1; i <= MAXNUM_KEY; i++)
    {
        j++;
        pNode->SetElement(j, this->GetElement(i));
        this->SetElement(i, Invalid(this->key_kind));
    }

    // 把第position+1 -- 2V+1个指针移到指定的结点中(注意指针比键多一个)
    j = 0;
    for (i = position + 1; i <= MAXNUM_POINTER; i++)
    {
        j++;
        this->GetPointer(i)->SetFather(pNode);    // 重新设置子结点的父亲
        pNode->SetPointer(j, this->GetPointer(i));
        this->SetPointer(i, INVALID);
    }

    // 清除提取出的位置
    this->SetElement(position, Invalid(this->key_kind));

    // 设置好Count个数
    this->SetCount(position - 1);
    pNode->SetCount(MAXNUM_KEY - position);


    return RetKey;
}

//结合结点，把指定中间结点的数据全部剪切到本中间结点
bool CInternalNode::Combine(CNode* pNode)
{
    // 参数检查
    if (this->GetCount() + pNode->GetCount() + 1 > MAXNUM_DATA)    // 预留一个新键的位置
    {
        return false;
    }

    // 取待合并结点的第一个孩子的第一个元素作为新键值
    void* NewKey=NULL;
    CNode* node=pNode->GetPointer(1);
    while(node->GetType()!=NODE_TYPE_LEAF)node=node->GetPointer(1);
    NewKey = node->GetElement(1);
    //assign(NewKey,pNode->GetPointer(1)->GetElement(1) , this->key_kind);
    m_Keys[m_Count] = NewKey;
    m_Count++;
    this->offt_pointers[m_Count] = pNode->GetPointer(1)->getPtSelf();   //疑问：感觉应该为m_Pointers[m_Count+1] = pNode->GetPointer(1);

    for (int i = 1; i <= pNode->GetCount(); i++)
    {
        m_Keys[m_Count] = pNode->GetElement(i);
        m_Count++;
        this->offt_pointers[m_Count] = pNode->GetPointer(i + 1)->getPtSelf();
    }
    pNode->FreeBlock();
    return true;
}

// 从另一结点移一个元素到本结点
bool CInternalNode::MoveOneElement(CNode* pNode)
{
    // 参数检查
    if (this->GetCount() >= MAXNUM_DATA)
    {
        return false;
    }

    int i, j;


    // 兄弟结点在本结点左边
    
    if (cmp(this->GetElement(1), pNode->GetElement(1),this->key_kind))
    {
        // 先腾出位置
        for (i = m_Count; i > 0; i--)
        {
            m_Keys[i] = m_Keys[i - 1];
        }
        for (j = m_Count + 1; j > 0; j--)
        {
            this->offt_pointers[j] = this->offt_pointers[j - 1];
        }

        // 赋值
        // 第一个键值不是兄弟结点的最后一个键值，而是本结点第一个子结点的第一个元素的值
        m_Keys[0] = GetPointer(1)->GetElement(1);
        // 第一个子结点为兄弟结点的最后一个子结点
        this->offt_pointers[0] = pNode->GetPointer(pNode->GetCount() + 1)->getPtSelf();

        // 修改兄弟结点
        pNode->SetElement(pNode->GetCount(), Invalid(this->key_kind));
        pNode->SetPointer(pNode->GetCount() + 1, INVALID);
    }
    else    // 兄弟结点在本结点右边
    {
        // 赋值
        // 最后一个键值不是兄弟结点的第一个键值，而是兄弟结点第一个子结点的第一个元素的值
        m_Keys[m_Count] = pNode->GetPointer(1)->GetElement(1);
        // 最后一个子结点为兄弟结点的第一个子结点
        this->offt_pointers[m_Count + 1] = pNode->GetPointer(1)->getPtSelf();

        // 修改兄弟结点
        // for (i = 1; i < pNode->GetCount() - 1; i++)
        // {
        //     pNode->SetElement(i, pNode->GetElement(i + 1));
        // }
        for (i = 1; i < pNode->GetCount(); i++)
        {
            pNode->SetElement(i, pNode->GetElement(i + 1));
        }
        pNode->SetElement(pNode->GetCount(), Invalid(this->key_kind));
        for (j = 1; j <= pNode->GetCount(); j++)
        {
            pNode->SetPointer(j, pNode->GetPointer(j + 1));
        }
    }

    // 设置数目
    this->SetCount(this->GetCount() + 1);
    pNode->SetCount(pNode->GetCount() - 1);
    this->flush_file();
    pNode->flush_file();
    return true;
}

// 清除叶子结点中的数据
// CLeafNode::CLeafNode()
// {
//     node_Type = NODE_TYPE_LEAF;

//     for (int i = 0; i < MAXNUM_DATA; i++)
//     {
//         m_Datas[i] = INVALID;
//     }

//     m_pPrevNode = NULL;
//     m_pNextNode = NULL;
// }
CLeafNode::CLeafNode(const char* fname,KEY_KIND key_kind,size_t max_size,off_t offt):CNode(fname,key_kind,max_size,offt)
{
    this->node_Type = NODE_TYPE_LEAF;
    m_pPrevNode = NULL;
    m_pNextNode = NULL;
    this->offt_NextNode=NULL;
    this->offt_PrevNode=NULL;
    for (int i = 0; i < MAXNUM_DATA; i++)m_Datas[i] = Invalid(this->key_kind);
    for(int i=0;i<MAXNUM_DATA;i++)this->offt_data[i]=INVALID;
    if(this->offt_self != NEW_OFFT){
        this->get_file();
    }
    else{
        this->offt_self=FileManager::getInstance()->getFreeBlock(fname, BLOCK_LEAF);
        //后面一定一定要记得更新
        }
    FileManager::getInstance()->flushBlock(fname, this->offt_self, BLOCK_LEAF);
    
}
CLeafNode::~CLeafNode()
{

}

// 在叶子结点中插入数据
bool CLeafNode::Insert(void* value,off_t offt_data)
{
    int i, j;
    // 如果叶子结点已满，直接返回失败
    if (GetCount() >= MAXNUM_DATA)
    {
        return false;
    }

    // 找到要插入数据的位置
    for (i = 0; (i < m_Count)&&cmp(value , m_Datas[i],this->key_kind) ; i++)
    {
    }

    // 当前位置及其后面的数据依次后移，空出当前位置
    for (j = m_Count; j > i; j--)
    {
        m_Datas[j] = m_Datas[j - 1];
        this->offt_data[j]=this->offt_data[j-1];
    }

    // 把数据存入当前位置，唯一插入的地方
    m_Datas[i] = value;
    this->offt_data[i]=offt_data;

    m_Count++;
    

    // 返回成功
    return true;
}

bool CLeafNode::Delete(void* value,bool deleteo_offtData)
{
    int i, j;
    bool found = false;
    for (i = 0; i < m_Count; i++)
    {
        if (eql(value, this->m_Datas[i], this->key_kind))
        {
            found = true;
            break;
        }
    }
    // 如果没有找到，返回失败
    if (false == found)
    {
        return false;
    }
    if(deleteo_offtData)FileManager::getInstance()->flushBlock(this->fname, this->offt_data[i], BLOCK_FREE);
    // 后面的数据依次前移
    for (j = i; j < m_Count - 1; j++)
    {
        m_Datas[j] = m_Datas[j + 1];
    }
    for(j=i;j<m_Count-1;j++){
        this->offt_data[j]=this->offt_data[j+1];
    }
    // 设置最后一个数据为无效
    m_Datas[j] = Invalid(this->key_kind);
    this->offt_data[j]=INVALID;
    //将这个数据块释放
    
    this->flush_file();
    m_Count--;
    
    
    // 返回成功
    return true;

}

// 分裂叶子结点，把本叶子结点的后一半数据剪切到指定的叶子结点中
void* CLeafNode::Split(CNode* pNode)
{
    // 把本叶子结点的后一半数据移到指定的结点中
    int j = 0;
    for (int i = ORDER_V + 1; i <= MAXNUM_DATA; i++)
    {
        j++;
        pNode->SetElement(j, this->GetElement(i));
        ((CLeafNode*)pNode)->SetElement_offt(j, this->GetElement_offt(i));
        this->SetElement(i, Invalid(this->key_kind));
        this->SetElement_offt(i, INVALID);
    }
    // 设置好Count个数
    this->SetCount(this->GetCount() - j);
    pNode->SetCount(pNode->GetCount() + j);

    // 返回新结点的第一个元素作为键
    return pNode->GetElement(1);
}

// 结合结点，把指定叶子结点的数据全部剪切到本叶子结点
bool CLeafNode::Combine(CNode* pNode)
{
    // 参数检查
    if (this->GetCount() + pNode->GetCount() > MAXNUM_DATA)
    {
        return false;
    }

    for (int i = 1; i <= pNode->GetCount(); i++)
    {
        this->Insert(pNode->GetElement(i),((CLeafNode*)pNode)->GetElement_offt(i));
    }
    pNode->FreeBlock();
    return true;
}

void CLeafNode::SetPrevNode(CLeafNode* node){
    this->m_pPrevNode = node;
    if(node!=NULL){this->offt_PrevNode = node->offt_self;}
}
CLeafNode* CLeafNode::GetPrevNode(){
    if(this->offt_PrevNode<=LOC_GRAPH)return NULL;
    char type = FileManager::getInstance()->get_BlockType(this->fname, this->offt_PrevNode);
    if(type!=BLOCK_LEAF)return NULL;
    return new CLeafNode(this->fname, this->key_kind, this->max_size, this->offt_PrevNode);
}
void CLeafNode::SetNextNode(CLeafNode* node){
    this->m_pNextNode = node;
    if(node!=NULL){this->offt_NextNode = node->offt_self;}
    
}
CLeafNode* CLeafNode::GetNextNode(){
    if(this->offt_NextNode<=LOC_GRAPH)return NULL;
    char type = FileManager::getInstance()->get_BlockType(this->fname, this->offt_NextNode);
    if(type!=BLOCK_LEAF)return NULL;
    return new CLeafNode(this->fname, this->key_kind, this->max_size, this->offt_NextNode);
}
BPlusTree::BPlusTree()
{
    m_Depth = 0;
    m_Root = NULL;
    m_pLeafHead = NULL;
    m_pLeafTail = NULL;
    this->offt_self = 0;
    
}
BPlusTree::BPlusTree(const std::string& fname)
{
    strcpy(this->fpath, fname.c_str());
    this->offt_self = 0;
    get_file();
    this->m_Root=NULL;
    this->m_pLeafHead=NULL;
    this->m_pLeafTail=NULL;
    FileManager::getInstance()->get_BlockGraph(this->fpath, this->Block_GRAPH);
    // cout<<"初始化位图为："<<endl;
    // for(int i=0;i<30;i++){
    //     cout<<this->Block_GRAPH[i]<<" ";
    // }
    // cout<<endl;
    if(this->Block_GRAPH[offt_root]==BLOCK_LEAF){
        this->m_Root=new CLeafNode(this->fpath, this->key_kind, this->max_key_size, this->offt_root);

    }
    else if(this->Block_GRAPH[offt_root]==BLOCK_INTER){
        this->m_Root=new CInternalNode(this->fpath, this->key_kind, this->max_key_size, this->offt_root);
    }
    
}
BPlusTree::~BPlusTree()
{
    ClearTree();
}
CLeafNode* BPlusTree::GetLeafHead(){
    if(this->m_pLeafHead!=NULL)return this->m_pLeafHead;
    char type = FileManager::getInstance()->get_BlockType(this->fpath, this->offt_leftHead);
    if(type==BLOCK_LEAF)return new CLeafNode(this->fpath, this->key_kind, this->max_key_size, this->offt_leftHead);
    return NULL;
}
CLeafNode* BPlusTree::GetLeafTail(){
    if(this->m_pLeafTail!=NULL)return this->m_pLeafTail;
    char type = FileManager::getInstance()->get_BlockType(this->fpath, this->offt_rightHead);
    if(type==BLOCK_LEAF)return new CLeafNode(this->fpath, this->key_kind, this->max_key_size, this->offt_rightHead);
    return NULL;
}
void BPlusTree::SetLeafHead(CLeafNode* node){
    this->m_pLeafHead = node;
    if(node!=NULL)this->offt_leftHead = node->getPtSelf();
}
void BPlusTree::SetLeafTail(CLeafNode* node){
    this->m_pLeafTail = node;
    if(node!=NULL)this->offt_rightHead = node->getPtSelf();
}
// 在树中查找数据
off_t BPlusTree::Search(void* data)
{
    int i = 0;
    int offset = 0;
    // if (NULL != sPath)
    // {
    //     (void)sprintf(sPath + offset, "The serach path is:");
    //     offset += 19;
    // }

    CNode* pNode = GetRoot();
    // 循环查找对应的叶子结点
    while (NULL != pNode)
    {
        // 结点为叶子结点，循环结束
        if (NODE_TYPE_LEAF == pNode->GetType())
        {
            break;
        }

        // 找到第一个键值大于等于key的位置
        for (i = 1;  (i <= pNode->GetCount())&&(cmp(data , pNode->GetElement(i),this->key_kind)|| eql(data,pNode->GetElement(i),this->key_kind)); i++)
        {
        }

        // if (NULL != sPath)
        // {
        //     (void)sprintf(sPath + offset, " %3d -->", pNode->GetElement(1));
        //     offset += 8;
        // }

        pNode = pNode->GetPointer(i);
    }

    // 没找到，这里可能会有情况，为null的情况
    if (NULL == pNode)
    {
        return INVALID;
    }

    // if (NULL != sPath)
    // {
    //     (void)sprintf(sPath + offset, "%3d", pNode->GetElement(1));
    //     offset += 3;
    // }

    // 在叶子结点中继续找
    off_t found = INVALID;
    for (i = 1; (i <= pNode->GetCount()); i++)
    {
        if (eql(data,pNode->GetElement(i),this->key_kind))
        {
            found = ((CLeafNode*)pNode)->GetElement_offt(i);
        }
    }


    // if (NULL != sPath)
    // {
    //     if (true == found)
    //     {

    //         (void)sprintf(sPath + offset, " ,successed.");
    //     }
    //     else
    //     {
    //         (void)sprintf(sPath + offset, " ,failed.");
    //     }
    // }
    //释放内存
    delete pNode;
    return found;
}

/* 在B+树中插入数据
插入数据首先要找到理论上要插入的叶子结点，然后分三种情况：
(1) 叶子结点未满。直接在该结点中插入即可；
(2) 叶子结点已满，且无父结点(即根结点是叶子结点)，需要首先把叶子结点分裂，然后选择插入原结点或新结点，然后新生成根节点；
(3) 叶子结点已满，但其父结点未满。需要首先把叶子结点分裂，然后选择插入原结点或新结点，再修改父结点的指针；
(4) 叶子结点已满，且其父结点已满。需要首先把叶子结点分裂，然后选择插入原结点或新结点，接着把父结点分裂，再修改祖父结点的指针。
    因为祖父结点也可能满，所以可能需要一直递归到未满的祖先结点为止。
*/
off_t BPlusTree::Insert(void* data)  //
{
    // 检查是否重复插入
    off_t found = Search(data);
    if (found!=INVALID)
    {
        return INVALID;
    }
    // for debug
    //if (289 == data)
    //{
    //    printf("\n%d,check failed!",data);
    //}
    //在文件中拿到一个空闲块
    off_t offt_data = FileManager::getInstance()->getFreeBlock(this->fpath, BLOCK_DATA);

    // 查找理想的叶子结点
    CLeafNode* pOldNode = SearchLeafNode(data);
    // 如果没有找到，说明整个树是空的，生成根结点
    if (NULL == pOldNode)
    {
        pOldNode = new CLeafNode(this->fpath, this->key_kind, this->max_key_size, NEW_OFFT);
        this->SetLeafHead(pOldNode);
        this->SetLeafTail(pOldNode);
        SetRoot(pOldNode);
    }

    // 叶子结点未满，对应情况1，直接插入
    if (pOldNode->GetCount() < MAXNUM_DATA)
    {
        bool success= pOldNode->Insert(data,offt_data);
        pOldNode->flush_file();
        if(pOldNode->getPtSelf()==this->offt_root)this->SetRoot(pOldNode);
        //插入完立马更新数据
        //SetRoot(pOldNode);
        delete pOldNode;
        if(success)return offt_data;
        return INVALID;
    }

    // 原叶子结点已满，新建叶子结点，并把原结点后一半数据剪切到新结点
    CLeafNode* pNewNode = new CLeafNode(this->fpath, this->key_kind, this->max_key_size, NEW_OFFT);
    void* key = INVALID;
    off_t offt_key = INVALID;
    key = pOldNode->Split(pNewNode);

    // 在双向链表中插入结点
    // CLeafNode* pOldNext = pOldNode->m_pNextNode;
    // pOldNode->m_pNextNode = pNewNode;
    // pNewNode->m_pNextNode = pOldNext;
    // pNewNode->m_pPrevNode = pOldNode;
    CLeafNode* pOldNext = pOldNode->GetNextNode();
    pOldNode->SetNextNode(pNewNode);
    pNewNode->SetNextNode(pOldNext);
    pNewNode->SetPrevNode(pOldNode);

    if (NULL == pOldNext)
    {
        //m_pLeafTail = pNewNode;
        this->SetLeafTail(pNewNode);
    }
    else
    {
        //pOldNext->m_pPrevNode = pNewNode;
        pOldNext->SetPrevNode(pNewNode);
    }


    // 判断是插入到原结点还是新结点中，确保是按数据值排序的
    if (cmp(key, data, this->key_kind))
    {
        pOldNode->Insert(data,offt_data);    // 插入原结点
    }
    else
    {
        pNewNode->Insert(data,offt_data);    // 插入新结点
    }

    // 父结点
    CInternalNode* pFather = (CInternalNode*)(pOldNode->GetFather());

    // 如果原结点是根节点，对应情况2
    if (NULL == pFather)
    {
        CNode* pNode1 = new CInternalNode(this->fpath, this->key_kind, this->max_key_size, NEW_OFFT);
        pNode1->SetPointer(1, pOldNode);                           // 指针1指向原结点
        pNode1->SetElement(1, key);                                // 设置键
        pNode1->SetPointer(2, pNewNode);                           // 指针2指向新结点
        pOldNode->SetFather(pNode1);                               // 指定父结点
        pNewNode->SetFather(pNode1);                               // 指定父结点
        pNode1->SetCount(1);

        SetRoot(pNode1);
        delete pNode1;                                           // 指定新的根结点
        pOldNode->flush_file();
        pNewNode->flush_file();
        delete pFather;
        delete pNewNode;
        delete pOldNode;
        delete pOldNext;
        return offt_data;
    }
   
    // 情况3和情况4在这里实现
    bool ret = InsertInternalNode(pFather, key, pNewNode);
    pFather->flush_file();
    pOldNode->flush_file();
    pNewNode->flush_file();
    pOldNode->flush_file();
    delete pFather;
    delete pNewNode;
    delete pOldNode;
    delete pOldNext;
    if(ret){
        return offt_data;
    }
    return INVALID;
}
bool BPlusTree::Insert_Data(vector<vector<string>> value){
    void* data[ATTR_MAX_NUM];
    //先找到键值
    void* key = INVALID;
    int key_index = -1;
    int key_index2=-1;
    for(int i=0;i<value[0].size();i++){
        if(_stricmp(this->key_attr, value[0][i].c_str())==0){
            key_index = i;
        }
    }
    for(int i=1;i<value.size();i++){
        for(int j=0;j<attr_num;j++){
            for(int z=0;z<value[i].size();z++){
                if(_stricmp(this->attr[j].name, value[0][z].c_str())==0){
                    data[j]=str2value(value[i][z].c_str(), this->attr[j].key_kind);
                    if(z==key_index)key_index2 = j;
                    
                    break;
                }
                if(z==value[i].size()-1){
                    data[j]=str2value("NULL", this->attr[j].key_kind);
                }
            }
       
       }
    
    }

    //assign(key, data[key_index2], this->attr[key_index2].key_kind);
    off_t offt_data = Insert(data[key_index2]);
    if(offt_data == INVALID){
        return false;
    }
    FileManager::getInstance()->flush_data(this->fpath, data, this->attr, this->attr_num, offt_data);
    return true;
}
void BPlusTree::Get_Data(void* data[ATTR_MAX_NUM],off_t offt){
    FileManager::getInstance()->get_data(this->fpath, data, this->attr, this->attr_num, offt);
}
void BPlusTree::Print_Data(void* data[ATTR_MAX_NUM]){
    for(int i=0;i<attr_num;i++){
        cout<<this->attr[i].name<<" ";
    }
    cout<<endl;
    for(int i=0;i<attr_num;i++){
        print_key(data[i], attr[i].key_kind);
    }
    cout<<endl;
}
/* 删除某数据
删除数据的算法如下：
(1) 如果删除后叶子结点填充度仍>=50%，只需要修改叶子结点，如果删除的是父结点的键，父结点也要相应修改；
(2) 如果删除后叶子结点填充度<50%，需要先找到一个最近的兄弟结点(左右均可)，然后分两种情况：
    A. 如果该兄弟结点填充度>50%，把该兄弟结点的最近一个数据剪切到本结点，父结点的键值也要相应修改。
    B. 如果该兄弟结点的填充度=50%，则把两个结点合并，父结点键也相应合并。(如果合并后父结点的填充度<50%，则需要递归)
*/
bool BPlusTree::Delete(void* data)
{
    // 查找理想的叶子结点
    CLeafNode* pOldNode = SearchLeafNode(data);
    // 如果没有找到，返回失败
    if (NULL == pOldNode)
    {
        return false;
    }

    // 删除数据，如果失败一定是没有找到，直接返回失败
    bool success = pOldNode->Delete(data,true);
    pOldNode->flush_file();
    if (false == success)
    {
        return false;
    }

    // 获取父结点
    CInternalNode* pFather = (CInternalNode*)(pOldNode->GetFather());
    if (NULL == pFather)
    {
        pOldNode->flush_file();
        // 如果一个数据都没有了，删除根结点(只有根节点可能出现此情况)
        if (0 == pOldNode->GetCount())
        {
            
            pOldNode->FreeBlock();
            this->SetLeafHead(NULL);
            this->SetLeafTail(NULL);
            //m_pLeafHead = NULL;
            //m_pLeafTail = NULL;
            SetRoot(NULL);
        }
        delete pOldNode;
        return true;
    }


    // 删除后叶子结点填充度仍>=50%，对应情况1
    if (pOldNode->GetCount() >= ORDER_V)
    {
        for (int i = 1; (i <= pFather->GetCount())&& (cmp(data , pFather->GetElement(i),this->key_kind)||eql(data, pFather->GetElement(i), this->key_kind)) ; i++)
        {
            // 如果删除的是父结点的键值，需要更改该键
            if (eql(pFather->GetElement(i) ,data,this->key_kind))
            {
                pFather->SetElement(i, pOldNode->GetElement(1));    // 更改为叶子结点新的第一个元素
            }
        }
        updateNode(pFather);
        updateNode(pOldNode);
        return true;
    }

    // 找到一个最近的兄弟结点(根据B+树的定义，除了叶子结点，总是能找到的)
    int flag = FLAG_LEFT;
    CLeafNode* pBrother = (CLeafNode*)(pOldNode->GetBrother(flag));
    //这里可能会因为特殊原因找不到父亲节点，导致pBrother为空，所以需要判断一下
    if (pBrother == NULL){
        //修复正确的父亲节点
        bool b=this->SetCorrentFather(pOldNode);
        if(b)CLeafNode* pBrother = (CLeafNode*)(pOldNode->GetBrother(flag));
    }
    if(NULL == pBrother)
    {
        return true;//无可奈何，无法向上传播了。
    }
    // 兄弟结点填充度>50%，对应情况2A
    void* NewData = INVALID;
    off_t NewDataPos = 0;
    if (pBrother->GetCount() > ORDER_V)
    {
        if (FLAG_LEFT == flag)    // 兄弟在左边，移最后一个数据过来
        {
            NewData = pBrother->GetElement(pBrother->GetCount());
            NewDataPos = pBrother->GetElement_offt( pBrother->GetCount());
        }
        else    // 兄弟在右边，移第一个数据过来
        {
            NewData = pBrother->GetElement(1);
            NewDataPos = pBrother->GetElement_offt(1);
        }

        pOldNode->Insert(NewData, NewDataPos);
        pBrother->Delete(NewData,false);
        pOldNode->flush_file();
        pBrother->flush_file();
        // 修改父结点的键值
        if (FLAG_LEFT == flag)
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                if (pFather->GetPointer(i)->getPtSelf()== pOldNode->getPtSelf()&&i > 1)
                {
                    pFather->SetElement(i - 1, pOldNode->GetElement(1));    // 更改本结点对应的键
                }
            }
        }
        else
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                if (pFather->GetPointer(i)->getPtSelf()==pOldNode->getPtSelf() &&i > 1)
                {
                    pFather->SetElement(i - 1, pOldNode->GetElement(1));    // 更改本结点对应的键
                }
                if (pFather->GetPointer(i)->getPtSelf()==pBrother->getPtSelf() && i > 1)
                {
                    pFather->SetElement(i - 1, pBrother->GetElement(1));    // 更改兄弟结点对应的键
                }
            }
        }

        updateNode(pFather);
        updateNode(pOldNode);
        updateNode(pBrother);
        return true;
    }

    // 情况2B

    // 父结点中要删除的键
    void* NewKey = NULL;

    // 把本结点与兄弟结点合并，无论如何合并到数据较小的结点，这样父结点就无需修改指针

    if (FLAG_LEFT == flag)
    {
        pBrother->Combine(pOldNode);
        NewKey = pOldNode->GetElement(1);
        print_key(NewKey, this->key_kind);
        CLeafNode* pOldNext = pOldNode->GetNextNode();
        pBrother->SetPrevNode(pOldNext);
        //pBrother->m_pNextNode = pOldNext;
        // 在双向链表中删除结点
        if (NULL == pOldNext)
        {
            this->SetLeafTail(pBrother);
            //m_pLeafTail = pBrother;
        }
        else
        {
            pOldNext->SetPrevNode(pBrother);
            //pOldNext->m_pPrevNode = pBrother;
        }
        // 删除本结点
        pBrother->flush_file();
        delete pOldNode;
    }
    else
    {
        pOldNode->Combine(pBrother);
        NewKey = pBrother->GetElement(1);

        CLeafNode* pOldNext = pBrother->GetNextNode();
        pOldNode->SetNextNode(pOldNext);
        //pOldNode->m_pNextNode = pOldNext;
        // 在双向链表中删除结点
        if (NULL == pOldNext)
        {
            this->SetLeafTail(pOldNode);
            //m_pLeafTail = pOldNode;
        }
        else
        {
            pOldNext->SetPrevNode(pOldNode);
            //pOldNext->m_pPrevNode = pOldNode;
        }
        // 删除本结点
        pOldNode->flush_file();
        delete pBrother;
    }

    return DeleteInternalNode(pFather, NewKey);
}

// 清除整个树，删除所有结点
void BPlusTree::ClearTree()
{
    CNode* pNode = GetRoot();
    if (NULL != pNode)
    {
        pNode->DeleteChildren();

        delete pNode;
    }

    m_pLeafHead = NULL;
    m_pLeafTail = NULL;
    SetRoot(NULL);
}

// 旋转以重新平衡，实际上是把整个树重构一下,结果不理想，待重新考虑
BPlusTree* BPlusTree::RotateTree()
{
    BPlusTree* pNewTree = new BPlusTree;
    int i = 0;
    CLeafNode* pNode = m_pLeafHead;
    while (NULL != pNode)
    {
        for (int i = 1; i <= pNode->GetCount(); i++)
        {
            (void)pNewTree->Insert(pNode->GetElement(i));
        }

        pNode = pNode->m_pNextNode;
    }

    return pNewTree;

}
// 检查树是否满足B+树的定义
bool BPlusTree::CheckTree()
{
    CLeafNode* pThisNode = m_pLeafHead;
    CLeafNode* pNextNode = NULL;
    while (NULL != pThisNode)
    {
        pNextNode = pThisNode->m_pNextNode;
        if (NULL != pNextNode)
        {
            if (pThisNode->GetElement(pThisNode->GetCount()) > pNextNode->GetElement(1))
            {
                return false;
            }
        }
        pThisNode = pNextNode;
    }

    return CheckNode(GetRoot());
}

// 递归检查结点及其子树是否满足B+树的定义
bool BPlusTree::CheckNode(CNode* pNode)
{
    if (NULL == pNode)
    {
        return true;
    }

    int i = 0;
    bool ret = false;

    // 检查是否满足50%的填充度
    if ((pNode->GetCount() < ORDER_V) && (pNode != GetRoot()))
    {
        return false;
    }

    // 检查键或数据是否按大小排序
    for (i = 1; i < pNode->GetCount(); i++)
    {
        if (pNode->GetElement(i) > pNode->GetElement(i + 1))
        {
            return false;
        }
    }

    if (NODE_TYPE_LEAF == pNode->GetType())
    {
        return true;
    }

    // 对中间结点，递归检查子树
    for (i = 1; i <= pNode->GetCount() + 1; i++)
    {
        ret = CheckNode(pNode->GetPointer(i));
        // 只要有一个不合法就返回不合法
        if (false == ret)
        {
            return false;
        }
    }

    return true;

}

// 打印整个树
void BPlusTree::PrintTree()
{
    CNode* pRoot = GetRoot();
    if (NULL == pRoot) return;

    CNode* p1, * p2, * p3;
    int i, j, k;
    int total = 0;

    printf("\n第一层\n | ");
    PrintNode(pRoot);
    total = 0;
    printf("\n第二层\n | ");
    for (i = 1; i <= MAXNUM_POINTER; i++)
    {
        p1 = pRoot->GetPointer(i);
        if (NULL == p1) continue;
        PrintNode(p1);
        total++;
        if (total % 4 == 0) printf("\n | ");
    }
    total = 0;
    printf("\n第三层\n | ");
    for (i = 1; i <= MAXNUM_POINTER; i++)
    {
        p1 = pRoot->GetPointer(i);
        if (NULL == p1) continue;
        for (j = 1; j <= MAXNUM_POINTER; j++)
        {
            p2 = p1->GetPointer(j);
            if (NULL == p2) continue;
            PrintNode(p2);
            total++;
            if (total % 4 == 0) printf("\n | ");
        }
    }
    total = 0;
    printf("\n第四层\n | ");
    for (i = 1; i <= MAXNUM_POINTER; i++)
    {
        p1 = pRoot->GetPointer(i);
        if (NULL == p1) continue;
        for (j = 1; j <= MAXNUM_POINTER; j++)
        {
            p2 = p1->GetPointer(j);
            if (NULL == p2) continue;
            for (k = 1; k <= MAXNUM_POINTER; k++)
            {
                p3 = p2->GetPointer(k);
                if (NULL == p3) continue;
                PrintNode(p3);
                total++;
                if (total % 4 == 0) printf("\n | ");
            }
        }
    }
}

// 打印某结点
void BPlusTree::PrintNode(CNode* pNode)
{
    if (NULL == pNode)
    {
        return;
    }

    for (int i = 1; i <= MAXNUM_KEY; i++)
    {
        if(this->key_kind==INT_KEY){
            cout<<*(int*)pNode->GetElement(i)<<" ";
        }
        else if (this->key_kind==LL_KEY) {
            cout<<*(long long*)pNode->GetElement(i)<<" ";
        }
        else{
            cout<<(char*)pNode->GetElement(i)<<" ";
        }
        
        if (i >= MAXNUM_KEY)
        {
            printf(" | ");
        }
    }
}

// 查找对应的叶子结点
CLeafNode* BPlusTree::SearchLeafNode(void* data)
{
    int i = 0;

    CNode* pNode = GetRoot();
    // 循环查找对应的叶子结点
    while (NULL != pNode)
    {
        // 结点为叶子结点，循环结束
        if (NODE_TYPE_LEAF == pNode->GetType())
        {
            break;
        }

        // 找到第一个键值大于等于key的位置
        for (i = 1; i <= pNode->GetCount(); i++)
        {
            
            if (cmp(pNode->GetElement(i), data,this->key_kind))
            {
                break;
            }
        }

        pNode = pNode->GetPointer(i);
    }

    return (CLeafNode*)pNode;
}

bool BPlusTree::SetCorrentFather(CLeafNode* leaf){
    int i = 0;

    CNode* pNode = GetRoot();
    CNode* p1 = NULL;
    // 循环查找对应的叶子结点，真怕后面直接死循环
    while (pNode->GetType()!=NODE_TYPE_LEAF)
    {
        
        // 找到第一个键值大于等于key的位置
        for (i = 1; i <= pNode->GetCount(); i++)
        {
            if (cmp(pNode->GetElement(i), leaf->GetElement(1),this->key_kind))
            {
                break;
            }
        }
        delete p1;
        p1 = new CInternalNode(this->fpath, this->key_kind, this->max_key_size, pNode->getPtSelf());
        pNode = pNode->GetPointer(i);
    }
    if(i!=0&&p1!=NULL&&p1->GetType()==NODE_TYPE_INTERNAL){
        leaf->SetFather(p1);
        leaf->flush_file();
        delete p1;
        return true;
    }
    else{
        return false;
    }
    
}
bool BPlusTree::SetCorrentFather(CInternalNode* node){
    int i = 0;

    CNode* pNode = GetRoot();
    CNode* p1 = NULL;
    // 循环查找对应的叶子结点，真怕后面直接死循环
    while (pNode->getPtSelf()!=node->getPtSelf())
    {
        
        // 找到第一个键值大于等于key的位置
        for (i = 1; i <= pNode->GetCount(); i++)
        {
            if (cmp(pNode->GetElement(i), node->GetElement(1),this->key_kind))
            {
                break;
            }
        }
        if(pNode->getPtSelf()==node->getPtSelf())break;
        delete p1;
        p1 = new CInternalNode(this->fpath, this->key_kind, this->max_key_size, pNode->getPtSelf());
        pNode = pNode->GetPointer(i);
    }
    if(i!=0&&p1!=NULL&&p1->GetType()==NODE_TYPE_INTERNAL){
        node->SetFather(p1);
        node->flush_file();
        delete p1;
        return true;
    }
    else{
        return false;
    }
}
//递归函数：插入键到中间结点
bool BPlusTree::InsertInternalNode(CInternalNode* pNode, void* key, CNode* pRightSon)
{
    if (NULL == pNode || NODE_TYPE_LEAF == pNode->GetType())
    {
        return false;
    }

    // 结点未满，直接插入
    if (pNode->GetCount() < MAXNUM_KEY)
    {
        bool ans=pNode->Insert(key, pRightSon);
        pNode->flush_file();
        return ans;
    }

    CInternalNode* pBrother = new CInternalNode(this->fpath, this->key_kind, this->max_key_size, NEW_OFFT);  //C++中new 类名表示分配一个类需要的内存空间，并返回其首地址；
    void* NewKey = INVALID;
    // 分裂本结点
    NewKey = pNode->Split(pBrother, key);
    //print_key(NewKey, this->key_kind);
    if (pNode->GetCount() < pBrother->GetCount())
    {
        pNode->Insert(key, pRightSon);
    }
    else if (pNode->GetCount() > pBrother->GetCount())
    {
        pBrother->Insert(key, pRightSon);
    }
    else    // 两者相等，即键值在第V和V+1个键值中间的情况，把字节点挂到新结点的第一个指针上
    {
        pBrother->SetPointer(1, pRightSon);
        pRightSon->SetFather(pBrother);
    }
    pRightSon->flush_file();
    CInternalNode* pFather = (CInternalNode*)(pNode->GetFather());
    // 直到根结点都满了，新生成根结点
    if (NULL == pFather)
    {
        pFather = new CInternalNode(this->fpath, this->key_kind, this->max_key_size, NEW_OFFT);
        pFather->SetPointer(1, pNode);                           // 指针1指向原结点
        pFather->SetElement(1, NewKey);                          // 设置键
        pFather->SetPointer(2, pBrother);                        // 指针2指向新结点
        pNode->SetFather(pFather);                               // 指定父结点
        pBrother->SetFather(pFather);                            // 指定父结点
        pFather->SetCount(1);

        pBrother->flush_file();
        pNode->flush_file();
        SetRoot(pFather);
        delete pBrother;
        delete pFather;
                                                // 指定新的根结点
        return true;
    }
    pNode->flush_file();
    // 递归
    return InsertInternalNode(pFather, NewKey, pBrother);
}

// 递归函数：在中间结点中删除键
bool BPlusTree::DeleteInternalNode(CInternalNode* pNode, void* key)
{
    // 删除键，如果失败一定是没有找到，直接返回失败
    bool success = pNode->Delete(key);
    
    if (false == success)
    {
        return false;
    }

    // 获取父结点
    CInternalNode* pFather = (CInternalNode*)(pNode->GetFather());
    if (NULL == pFather)
    {
        // 如果一个数据都没有了，把根结点的第一个结点作为根结点
        if (0 == pNode->GetCount())
        {
            SetRoot(pNode->GetPointer(1));
            pNode->FreeBlock(); // 释放结点
            delete pNode;
        }
        else SetRoot(pNode);

        return true;
    }

    // 删除后结点填充度仍>=50%
    if (pNode->GetCount() >= ORDER_V)
    {
        for (int i = 1; (i <= pFather->GetCount())&&(cmp(key , pFather->GetElement(i),this->key_kind) || eql(key, pFather->GetElement(i), this->key_kind)) ; i++)
        {
            // 如果删除的是父结点的键值，需要更改该键
            if (eql(pFather->GetElement(i) , key,this->key_kind))
            {
                CNode* node= pNode->GetPointer(i+1);
                while(node->GetType()!=NODE_TYPE_LEAF)node=node->GetPointer(1);
                pFather->SetElement(i, node->GetElement(1));    // 更改为叶子结点新的第一个元素
                pFather->flush_file();
            }
        }
        delete pFather;
        return true;
    }

    //找到一个最近的兄弟结点(根据B+树的定义，除了根结点，总是能找到的)
    int flag = FLAG_LEFT;
    CInternalNode* pBrother = (CInternalNode*)(pNode->GetBrother(flag));
    //对找不到兄弟结点的情况，作修复处理
    if (NULL == pBrother)
    {
        bool a=this->SetCorrentFather(pNode);
        if(a)pBrother = (CInternalNode*)(pNode->GetBrother(flag));
        
    }
    if(pBrother==NULL)return true;//无可奈何地返回
    // 兄弟结点填充度>50%
    void* NewData = INVALID;
    if (pBrother->GetCount() > ORDER_V)
    {
        pNode->MoveOneElement(pBrother);
        // 修改父结点的键值
        if (FLAG_LEFT == flag)
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                // if (pFather->GetPointer(i)->getPtSelf() == pNode->getPtSelf() && i > 1)
                // {
                //     pFather->SetElement(i - 1, pNode->GetElement(1));    // 更改本结点对应的键
                // }
                 if (pFather->GetPointer(i)->getPtSelf() == pNode->getPtSelf() && i > 1)
                {
                    CNode* node= pNode->GetPointer(1);
                    while(node->GetType()!=NODE_TYPE_LEAF)node=node->GetPointer(1);
                    pFather->SetElement(i - 1, node->GetElement(1));    // 更改本结点对应的键
                }
            }
        }
        else
        {
            for (int i = 1; i <= pFather->GetCount() + 1; i++)
            {
                // if (pFather->GetPointer(i)->getPtSelf() == pNode->getPtSelf() && i > 1)
                // {
                //     pFather->SetElement(i - 1, pNode->GetElement(1));    // 更改本结点对应的键
                // }
                // if (pFather->GetPointer(i)->getPtSelf() == pBrother->getPtSelf() && i > 1)
                // {
                //     pFather->SetElement(i - 1, pBrother->GetElement(1));    // 更改兄弟结点对应的键
                // }
                if (pFather->GetPointer(i)->getPtSelf() == pNode->getPtSelf() && i > 1)
                {
                    CNode* node= pNode->GetPointer(1);
                    while(node->GetType()!=NODE_TYPE_LEAF)node=node->GetPointer(1);
                    pFather->SetElement(i - 1, node->GetElement(1));    // 更改本结点对应的键
                    //pFather->SetElement(i - 1, pNode->GetPointer(1)->GetElement(1));    // 更改本结点对应的键
                }
                if (pFather->GetPointer(i)->getPtSelf() == pBrother->getPtSelf() && i > 1)
                {
                    CNode* node= pBrother->GetPointer(1);
                    while(node->GetType()!=NODE_TYPE_LEAF)node=node->GetPointer(1);
                    pFather->SetElement(i - 1, node->GetElement(1));    // 更改兄弟结点对应的键
                }
            }
        }
        updateNode(pFather);
        updateNode(pBrother);
        pNode->flush_file();
        return true;
    }

    // 父结点中要删除的键：兄弟结点都不大于50，则需要合并结点，此时父结点需要删除键
    void* NewKey = NULL;

    // 把本结点与兄弟结点合并，无论如何合并到数据较小的结点，这样父结点就无需修改指针
    if (FLAG_LEFT == flag)
    {
        pBrother->Combine(pNode);
        NewKey = pNode->GetElement(1);
        pBrother->flush_file();
        pNode->FreeBlock();
        delete pNode;
    }
    else
    {
        pNode->Combine(pBrother);
        NewKey = pBrother->GetElement(1);
        pNode->flush_file();
        //pBrother->FreeBlock();
        delete pBrother;
    }

    // 递归
    return DeleteInternalNode(pFather, NewKey);
}
void BPlusTree::Select_Data(vector<string>attributenames,vector<LOGIC>Logics,vector<WhereCondition>w){
    //先把只有id检测的写了
    if(w.size()==1&&_stricmp(w[0].attribute.c_str(),this->key_attr)==0&&w[0].operatorSymbol=="="){
        void* key;
        if(w[0].operatorSymbol=="="){
            key=str2value(w[0].value, this->key_kind);
        }
        off_t offt_data=this->Search(key);
        if(offt_data==INVALID){
            this->Print_Header(attributenames);
            return;
        }
        void* data[ATTR_MAX_NUM];
        this->Get_Data(data, offt_data);
        this->Print_Header(attributenames);
        this->Print_Data(data,attributenames);
        return;
    }
    //后面的情况等完成遍历后再写
    this->Print_Header(attributenames);
    for(int i=0;i<NUM_ALL_BLOCK;i++){
        if(this->Block_GRAPH[i]==BLOCK_DATA){
            void* data[ATTR_MAX_NUM];
            this->Get_Data(data, i);
            if(this->SatisfyConditions(w, Logics, data))this->Print_Data(data,attributenames);
        }
    }


}
void BPlusTree::Print_Data(void* data[ATTR_MAX_NUM],vector<string>attributenames){
    if(attributenames[0]=="*"){
        for(int i=0;i<this->attr_num;i++){
            print_key(data[i],this->attr[i].key_kind );
        }
        cout<<endl;
        return;
    }
    vector<int>index;
    for(int i=0;i<attributenames.size();i++){
        for(int j=0;j<this->attr_num;j++){
            if(_stricmp(attributenames[i].c_str(),this->attr[j].name)==0){
                index.push_back(j);
                break;
            }
            if(j==this->attr_num-1){
                cout<<"error:attribute not found"<<endl;
                return;
            }
        }
    }
    if(attributenames.size()!=index.size()){
        cout<<"error:attribute not found"<<endl;
        return;

    }
    for(int i=0;i<attributenames.size();i++)
        print_key(data[index[i]],this->attr[index[i]].key_kind );
    
}
void BPlusTree::Print_Header(vector<string>attributenames){
    if(attributenames[0]=="*"){
        for(int i=0;i<this->attr_num;i++){
            cout<<'|'<<this->attr[i].name<<'\t';
        }
        cout<<endl;
        return;
    }

    vector<int>index;
    for(int i=0;i<attributenames.size();i++){
        for(int j=0;j<this->attr_num;j++){
            if(_stricmp(attributenames[i].c_str(),this->attr[j].name)==0){
                index.push_back(j);
                break;
            }
            if(j==this->attr_num-1){
                cout<<"error:attribute not found"<<endl;
                return;
            }
        }
    }
    if(attributenames.size()!=index.size()){
        cout<<"error:attribute not found"<<endl;
        return;

    }
    for(int i=0;i<attributenames.size();i++){
        cout<<'|'<<attributenames[i]<<'\t';
    }
    cout<<endl;
}

bool BPlusTree::SatisfyCondition(WhereCondition w,void* data[ATTR_MAX_NUM]){
    void* compare_key;
    int j;
    for(int i=0;i<this->attr_num;i++){
        if(_stricmp(w.attribute.c_str(),this->attr[i].name)==0){
            compare_key=str2value(w.value, this->attr[i].key_kind);
            j=i;
            break;
        }
    }
    
    if(w.operatorSymbol[0]=='='){
        if(eql(data[j],compare_key,this->attr[j].key_kind))return true;
        return false;
    }
    else if(w.operatorSymbol[0]=='<'){
        if(cmp(data[j],compare_key,this->attr[j].key_kind))return false;
        
        if(w.operatorSymbol.size()==1
            &&eql(data[j],compare_key,this->attr[j].key_kind))return false;

        return true;
    }
    else if(w.operatorSymbol[0]=='>'){
        if(cmp(compare_key,data[j],this->attr[j].key_kind))return false;
        if(w.operatorSymbol.size()==1
            &&eql(data[j],compare_key,this->attr[j].key_kind))return false;

        return true;
        }

    return true;
}
bool BPlusTree::SatisfyConditions(vector<WhereCondition>w,vector<LOGIC>Logics,void* data[ATTR_MAX_NUM]){
    bool flag=true;
    if(w.size()==0)return true;
    bool flag1=SatisfyCondition(w[0],data);
    if(Logics.size()==0)return flag1;
    for(int i=1;i<w.size();i++){
        if(Logics[i-1]==AND_LOGIC){
            flag=SatisfyCondition(w[i],data);
            flag1=flag1&&flag;
        }
        else if(Logics[i-1]==OR_LOGIC){
            flag=SatisfyCondition(w[i],data);
            if(flag1||flag)return true;
            flag1=false;
        }
    }
    return flag1;
}
bool BPlusTree::Delete_Data(vector<WhereCondition>w){
    if(w.size()==0){
        //删除整棵树
        FileManager::getInstance()->deleteFile(this->fpath);
        return true;
    }
    void* key=str2value(w[0].value, this->key_kind);
    return this->Delete(key);
    
}
bool BPlusTree::Update_Data(vector<WhereCondition>w,vector<WhereCondition>attributenames){
    void* key=str2value(w[0].value, this->key_kind);
    cout<<"key:"<<*(int*)key<<endl;
    off_t offt_data=this->Search(key);
    if(offt_data==INVALID){
        cout<<"未找到该数据，修改失败"<<endl;
        return false;
    }
    void* data[ATTR_MAX_NUM];
    this->Get_Data(data, offt_data);
    for(int i=0;i<attributenames.size();i++){
        for(int j=0;j<this->attr_num;j++){
            if(_stricmp(attributenames[i].attribute.c_str(),this->attr[j].name)==0){
                data[j]=str2value(attributenames[i].value, this->attr[j].key_kind);
                break;
            }
        }
    }
    FileManager::getInstance()->flush_data(this->fpath, data,this->attr,this->attr_num,offt_data);
    return true;
}