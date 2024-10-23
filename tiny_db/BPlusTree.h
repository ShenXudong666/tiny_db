#pragma once
/* BPlusTree.h

B+�������ļ���������ʵ��һ���򵥵�B+��

Definition (from http://www.seanster.com/BplusTree/BplusTree.html ):
(1) A B+ tree of order v consists of a root, internal nodes and leaves.
(2) The root my be either leaf or node with two or more children.
(3) Internal nodes contain between v and 2v keys, and a node with k keys has k + 1 children.
(4) Leaves are always on the same level.
(5) If a leaf is a primary index, it consists of a bucket of records, sorted by search key. If it is a secondary index, it will have many short records consisting of a key and a pointer to the actual record.

(1) һ��v�׵�B+���ɸ���㡢�ڲ�����Ҷ�ӽ����ɡ�
(2) ����������Ҷ�ӽ�㣬Ҳ������������������������ڲ���㡣
(3) ÿ���ڲ�������v - 2v���������һ���ڲ�������k������������ֻ��k+1��ָ��������ָ�롣
(4) Ҷ�ӽ������������ͬһ���ϡ�
(5) ���Ҷ�ӽ������������������һ�鰴��ֵ����ļ�¼�����Ҷ�ӽ���Ǵ�������������һ��̼�¼��ÿ���̼�¼����һ�����Լ�ָ��ʵ�ʼ�¼��ָ�롣
(6) �ڲ����ļ�ֵ��Ҷ�ӽ�������ֵ���Ǵ�С��������ġ�
(7) ���м����У�ÿ�������������е����еļ���С���������ÿ�������������е����еļ������ڵ����������

*/


/* B+ ���Ľף����ڲ�����м�����С��Ŀv��
   Ҳ��Щ�˰ѽ׶���Ϊ�ڲ�����м��������Ŀ����2v��
   һ����ԣ�Ҷ�ӽ����������ݸ������ڲ����������������һ���ģ�Ҳ��2v��(������������Ŀ����Ϊ�˰��ڲ�����Ҷ�ӽ��ͳһ��ͬһ���ṹ�а�)
*/
#define ORDER_V 2    /* Ϊ���������v�̶�Ϊ2��ʵ�ʵ�B+��vֵӦ���ǿ���ġ������v���ڲ��ڵ��м�����Сֵ */

#define MAXNUM_KEY (ORDER_V * 2)    /* �ڲ����������������Ϊ2v */
#define MAXNUM_POINTER (MAXNUM_KEY + 1)    /* �ڲ���������ָ��������ָ�������Ϊ2v */
#define MAXNUM_DATA (ORDER_V * 2)    /* Ҷ�ӽ����������ݸ�����Ϊ2v */
#include<string>
#include<iostream>
#include<fstream>
#include<cstring>
using namespace std;
#include"rwdata.h"
#define DB_HEAD_SIZE 4096 // head size must be pow of 2! �ļ����ݿ��ͷ��С
#define DB_BLOCK_SIZE 8192 // block size must be pow of 2! �ļ����ݿ�����ݿ��С

/**
 * @brief �洢���ݶ��뷽ʽ
 */
#define DB_ALIGNMENT 16
#define db_align(d, a) (((d) + (a - 1)) & ~(a - 1))

#define ceil(M) (((M) - 1) / 2)

 /**
  * @brief ���ݿ�����
  */
#define TYPE_KEY 0
#define TYPE_VALUE 1


/* ��ֵ������*/
typedef int KEY_TYPE;    /* Ϊ�����������Ϊint���ͣ�ʵ�ʵ�B+����ֵ����Ӧ���ǿ���� */
/*��ע�� Ϊ�������Ҷ�ӽ�������Ҳֻ�洢��ֵ*/

/* ������� */
enum NODE_TYPE
{
    NODE_TYPE_ROOT = 1,    // �����
    NODE_TYPE_INTERNAL = 2,    // �ڲ����
    NODE_TYPE_LEAF = 3,    // Ҷ�ӽ��
};

#define NULL 0
#define INVALID 0

#define FLAG_LEFT 1
#define FLAG_RIGHT 2

/* ������ݽṹ��Ϊ�ڲ�����Ҷ�ӽ��ĸ��� */
class CNode
{
public:

    CNode();
    virtual ~CNode();

    //��ȡ�����ý������
    NODE_TYPE GetType() { return m_Type; }
    void SetType(NODE_TYPE type) { m_Type = type; }

    // ��ȡ��������Ч���ݸ���
    int GetCount() { return m_Count; }
    void SetCount(int i) { m_Count = i; }

    // ��ȡ������ĳ��Ԫ�أ����м���ָ��ֵ����Ҷ�ӽ��ָ����
    virtual KEY_TYPE GetElement(int i) { return 0; }
    virtual void SetElement(int i, KEY_TYPE value) { }

    // ��ȡ������ĳ��ָ�룬���м���ָָ�룬��Ҷ�ӽ��������
    virtual CNode* GetPointer(int i) { return NULL; }
    virtual void SetPointer(int i, CNode* pointer) { }

    // ��ȡ�����ø����
    CNode* GetFather() { return m_pFather; }
    void SetFather(CNode* father) { m_pFather = father; }

    off_t getPtFather() {
        return this->offt_father;
    }

    void setPtFather(off_t offt) {
        this->offt_father = offt;
    }

    // ��ȡһ��������ֵܽ��
    CNode* GetBrother(int& flag);

    // ɾ�����
    void DeleteChildren();

protected:

    NODE_TYPE m_Type;    // ������ͣ�ȡֵΪNODE_TYPE����

    int m_Count;    // ��Ч���ݸ��������м���ָ����������Ҷ�ӽ��ָ���ݸ���

    CNode* m_pFather;     // ָ�򸸽���ָ�룬��׼B+���в�û�и�ָ�룬������Ϊ�˸����ʵ�ֽ����Ѻ���ת�Ȳ���

    off_t offt_father;   //���׽ڵ��������ļ��е�λ��
};

/* �ڲ�������ݽṹ */
class CInternalNode : public CNode
{
public:

    CInternalNode();
    virtual ~CInternalNode();

    // ��ȡ�����ü�ֵ�����û���˵�����ִ�1��ʼ��ʵ���ڽ�����Ǵ�0��ʼ��
    KEY_TYPE GetElement(int i)
    {
        if ((i > 0) && (i <= MAXNUM_KEY))
        {
            return m_Keys[i - 1];
        }
        else
        {
            return INVALID;
        }
    }

    void SetElement(int i, KEY_TYPE key)
    {
        if ((i > 0) && (i <= MAXNUM_KEY))
        {
            m_Keys[i - 1] = key;
        }
    }

    // ��ȡ������ָ�룬���û���˵�����ִ�1��ʼ
    CNode* GetPointer(int i)
    {
        if ((i > 0) && (i <= MAXNUM_POINTER))
        {
            return m_Pointers[i - 1];
        }
        else
        {
            return NULL;
        }
    }

    void SetPointer(int i, CNode* pointer)
    {
        if ((i > 0) && (i <= MAXNUM_POINTER))
        {
            m_Pointers[i - 1] = pointer;
        }
    }

    // �ڽ��pNode�ϲ����value
    bool Insert(KEY_TYPE value, CNode* pNode);
    // ɾ����value
    bool Delete(KEY_TYPE value);

    // ���ѽ��
    KEY_TYPE Split(CInternalNode* pNode, KEY_TYPE key);
    // ��Ͻ��(�ϲ����)
    bool Combine(CNode* pNode);
    // ����һ�����һ��Ԫ�ص������
    bool MoveOneElement(CNode* pNode);

    //��ȡ�ļ�ʱ����ָ������
    void setPointers(off_t offt_pointer[]) {
        for (int i = 0; i < MAXNUM_POINTER; i++) {
            this->offt_pointers[i] = offt_pointer[i];
        }
    }
    //����index������һ���ڵ���ļ�ָ��
    off_t getPointer(int index) {
        return this->offt_pointers[index];
    }

    bool flush_file(string fname) {
        inter_node node;
        /*��һ���ֺ����������ݵ�������б��*/
        memcpy(node.m_Keys, this->m_Keys, sizeof(this->m_Keys));
        memcpy(node.offt_pointers, this->offt_pointers, sizeof(this->offt_pointers));
        node.offt_self = this->offt_self;
        node.offt_father = this->offt_father;
        FileManager().flushInterNode(node,fname, this->offt_self);

        return true;
    }

    bool get_file(string fname) {
        inter_node node=FileManager().getCInternalNode(fname, this->offt_self);
        memcpy(this->m_Keys, node.m_Keys, sizeof(node.m_Keys));
        memcpy(this->offt_pointers, node.offt_pointers, sizeof(node.m_Keys));
        
        this->offt_father=node.offt_father;

        return true;
    }


protected:

    KEY_TYPE m_Keys[MAXNUM_KEY];           // ������
    off_t offt_pointers[MAXNUM_POINTER];    //ָ����������ļ��е�λ��
    off_t offt_self;
    CNode* m_Pointers[MAXNUM_POINTER];     // ָ������
};

/* Ҷ�ӽ�����ݽṹ */
class CLeafNode : public CNode
{
public:

    CLeafNode();
    virtual ~CLeafNode();

    // ��ȡ����������
    KEY_TYPE GetElement(int i)
    {
        if ((i > 0) && (i <= MAXNUM_DATA))
        {
            return m_Datas[i - 1];
        }
        else
        {
            return INVALID;
        }
    }

    void SetElement(int i, KEY_TYPE data)
    {
        if ((i > 0) && (i <= MAXNUM_DATA))
        {
            m_Datas[i - 1] = data;
        }
    }

    // ��ȡ������ָ�룬��Ҷ�ӽ�������壬ֻ��ʵ�и�����麯��
    CNode* GetPointer(int i)
    {
        return NULL;
    }

    // ��������
    bool Insert(KEY_TYPE value);
    // ɾ������
    bool Delete(KEY_TYPE value);

    // ���ѽ��
    KEY_TYPE Split(CNode* pNode);
    // ��Ͻ��
    bool Combine(CNode* pNode);
    void sePtPrevNode(off_t offset) {
        this->offt_PrevNode = offset;
    }
    void setPtNextNode(off_t offset) {
        this->offt_NextNode = offset;
    }
    // ��ȡǰһ���ڵ��ƫ����
    off_t getPrevNodeOffset() const {
        return this->offt_PrevNode;
    }

    // ��ȡ��һ���ڵ��ƫ����
    off_t getNextNodeOffset() const {
        return this->offt_NextNode;
    }

    bool flush_file(string fname) {
        leaf_node node;
        /*��һ���ֺ����������ݵ�������б��*/
        memcmp(node.m_Datas, this->m_Datas, sizeof(this->m_Datas));
        
        node.offt_self = this->offt_self;
        node.offt_father = this->offt_father;
        node.offt_NextNode = this->offt_NextNode;
        node.offt_PrevNode = this->offt_PrevNode;
        FileManager().flushLeafNode(node, fname, this->offt_self);

        return true;
    }

    bool get_file(string fname) {
        leaf_node node = FileManager().getLeafNode(fname, this->offt_self);

        memcmp(this->m_Datas, node.m_Datas, sizeof(node.m_Datas));
        this->offt_PrevNode = node.offt_PrevNode;
        this->offt_NextNode = node.offt_NextNode;
        this->offt_father = node.offt_father;

        return true;
    }
public:
    // ����������������ʵ��˫������
    CLeafNode* m_pPrevNode;                 // ǰһ�����
    CLeafNode* m_pNextNode;                 // ��һ�����
    off_t offt_PrevNode;                    //ǰһ���ڵ����ļ��е�ƫ��λ��
    off_t offt_NextNode;                    //��һ��λ�����ļ��е�ƫ��λ��

protected:
    off_t offt_self;
    KEY_TYPE m_Datas[MAXNUM_DATA];    // ��������

};

/* B+�����ݽṹ */
class BPlusTree
{
public:

    BPlusTree();
    virtual ~BPlusTree();

    // ����ָ��������
    bool Search(KEY_TYPE data, char* sPath);
    // ����ָ��������
    bool Insert(KEY_TYPE data);
    // ɾ��ָ��������
    bool Delete(KEY_TYPE data);

    // �����
    void ClearTree();

    // ��ӡ��
    void PrintTree();

    // ��ת��
    BPlusTree* RotateTree();

    // ������Ƿ�����B+���Ķ���
    bool CheckTree();

    void PrintNode(CNode* pNode);

    // �ݹ����㼰�������Ƿ�����B+���Ķ���
    bool CheckNode(CNode* pNode);

    // ��ȡ�����ø����
    CNode* GetRoot()
    {
        return m_Root;
    }

    void SetRoot(CNode* root)
    {
        m_Root = root;
    }

    // ��ȡ���������
    int GetDepth()
    {
        return m_Depth;
    }

    void SetDepth(int depth)
    {
        m_Depth = depth;
    }

    // ��ȼ�һ
    void IncDepth()
    {
        m_Depth = m_Depth + 1;
    }

    // ��ȼ�һ
    void DecDepth()
    {
        if (m_Depth > 0)
        {
            m_Depth = m_Depth - 1;
        }
    }

    bool flush_file() {
        table t;
        t.fpath = this->fpath;
        t.offt_root = this->offt_root;
        t.offt_leftHead = this->offt_leftHead;
        t.offt_rightHead = this->offt_rightHead;
        t.key_use_block = this->key_use_block;
        t.value_use_block = this->value_use_block;
        t.key_type = this->key_type;
        t.m_Depth = this->m_Depth;
        FileManager().flushTable(t, this->fpath, this->offt_self);
        return true;

    }

    bool get_file() {
        table t = FileManager().getTable(this->fpath, this->offt_self);
        this->offt_root = t.offt_root;
        this->offt_leftHead = t.offt_leftHead;
        this->offt_rightHead = t.offt_rightHead;
        this->key_type = t.key_type;
        this->key_use_block = t.key_use_block;
        this->value_use_block = t.value_use_block;
        this->m_Depth = t.m_Depth;

        return true;
    }


public:
    // ����������������ʵ��˫������
    CLeafNode* m_pLeafHead;                 // ͷ���
    CLeafNode* m_pLeafTail;                   // β���
    off_t offt_leftHead;
    off_t offt_rightHead;
    string fpath;     //�ļ���Ҳ�����·��

protected:

    // Ϊ���������Ҷ�ӽ��
    CLeafNode* SearchLeafNode(KEY_TYPE data);
    //��������м���
    bool InsertInternalNode(CInternalNode* pNode, KEY_TYPE key, CNode* pRightSon);
    // ���м�����ɾ����
    bool DeleteInternalNode(CInternalNode* pNode, KEY_TYPE key);

    CNode* m_Root;    // �����
    off_t offt_root;    //���ڵ����ļ��е�ƫ����
    int m_Depth;      // �������
    size_t key_use_block;
    size_t value_use_block;
    int key_type;
    off_t offt_self;
};

