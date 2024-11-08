#pragma once
#include "stdio.h"
#include "stdlib.h"
#define DB_HEAD_SIZE 4096 // head size must be pow of 2! �ļ����ݿ��ͷ��С
#define DB_BLOCK_SIZE 8192 // block size must be pow of 2! �ļ����ݿ�����ݿ��С
#define ORDER_V 2    /* Ϊ���������v�̶�Ϊ2��ʵ�ʵ�B+��vֵӦ���ǿ���ġ������v���ڲ��ڵ��м�����Сֵ */
#define MAXNUM_KEY (ORDER_V * 2)    /* �ڲ����������������Ϊ2v */
#define MAXNUM_POINTER (MAXNUM_KEY + 1)    /* �ڲ���������ָ��������ָ�������Ϊ2v */
#define MAXNUM_DATA (ORDER_V * 2)    /* Ҷ�ӽ����������ݸ�����Ϊ2v */
/* ��ֵ������*/
typedef int KEY_TYPE;    /* Ϊ�����������Ϊint���ͣ�ʵ�ʵ�B+����ֵ����Ӧ���ǿ���� */
/*��ע�� Ϊ�������Ҷ�ӽ�������Ҳֻ�洢��ֵ*/
#include<string>
#include<iostream>
#include<fstream>
#include <sys/stat.h>
using namespace std;
#define INT_KEY 1
#define LL_KEY 2
#define STRING_KEY 3

#define NUM_ALL_BLOCK 2048 

#define NULL 0
#define INVALID 0

#define FLAG_LEFT 1
#define FLAG_RIGHT 2
enum NODE_TYPE
{
	NODE_TYPE_ROOT = 1,    // �����
	NODE_TYPE_INTERNAL = 2,    // �ڲ����
	NODE_TYPE_LEAF = 3,    // Ҷ�ӽ��
};


typedef struct {
	char fpath[100];
	off_t offt_root;
	off_t offt_leftHead;
	off_t offt_rightHead;
	int m_Depth;//�������
	int key_type;//������������
	size_t max_key_size;
	size_t key_use_block;               /** ���ݿ�Ϊbtree_key���͵����� */
	size_t value_use_block;
}table;


typedef struct {
	off_t offt_self;
	off_t offt_pointers[MAXNUM_POINTER];
	off_t offt_father;
	size_t count;
	NODE_TYPE node_type;


}inter_node;

struct leaf_node {
	off_t offt_self;
	off_t offt_father;
	off_t offt_PrevNode;                    //ǰһ���ڵ����ļ��е�ƫ��λ��
	off_t offt_NextNode;                    //��һ��λ�����ļ��е�ƫ��λ��
	size_t count;
	NODE_TYPE node_type;
	leaf_node(){}
	leaf_node(off_t offt, size_t count, NODE_TYPE node_type,off_t
		offt_father=NULL, off_t offt_PrevNode=NULL
		, off_t offt_NextNode=NULL) : offt_self(offt), count(count), node_type(node_type),
		offt_father(offt_father), offt_PrevNode(offt_PrevNode),
		offt_NextNode(offt_NextNode) {}
	
};

struct Index{
	char fpath[100];
	off_t offt_self;
	off_t max_size;
	KEY_TYPE key_type;
	Index(){}
	Index(const char* fname, off_t offt,
		off_t max_size, KEY_TYPE key_type)
		:offt_self(offt), max_size(max_size), key_type(key_type) {
		memcpy(fpath, fname, sizeof(fname) + 1);
		fpath[sizeof(fname) + 1] = '\0';
	}
};

class FileManager {

public:

	static FileManager* getInstance();

	inter_node getCInternalNode(Index index, void* data[MAXNUM_KEY], off_t offt);
	bool flushInterNode(inter_node node, Index index, void** key);
	leaf_node getLeafNode(Index index, void* data[MAXNUM_DATA], off_t offt);
	bool flushLeafNode(leaf_node node, Index index, void** value);
	table getTable(const char* filename, off_t offt);
	bool flushTable(table t, const char* filename, off_t offt);
	bool table_create(const char* path, KEY_TYPE key_type, size_t max_key_size);
	void flush_key(void* key[MAXNUM_KEY], Index index);
	void flush_value(void* value[MAXNUM_DATA], Index index);
	void get_key(void* key[MAXNUM_KEY], Index index);
	void get_value(void* value[MAXNUM_DATA], Index index);
	void get_FreeGraph(Index index,char* freeBlock);
	void flush_FreeGraph(Index index, char* freeBlock);

	void newBlock(const char* filename);
	//��ȡ�ļ��ܹ��ж��ٿ�
	size_t getFileSize(const char* fileName);

	protected:
	static FileManager* object;
	

};
//FileManager* FileManager::object = NULL;