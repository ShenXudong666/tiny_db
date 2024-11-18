#pragma once
#include "stdio.h"
#include "stdlib.h"
#include <sys/types.h>
#define DB_HEAD_SIZE 4096 // head size must be pow of 2! 文件数据库的头大小
#define DB_BLOCK_SIZE 4096 // block size must be pow of 2! 文件数据库的数据块大小
#define ORDER_V 2    /* 为简单起见，把v固定为2，实际的B+树v值应该是可配的。这里的v是内部节点中键的最小值 */
#define MAXNUM_KEY (ORDER_V * 2)    /* 内部结点中最多键个数，为2v */
#define MAXNUM_POINTER (MAXNUM_KEY + 1)    /* 内部结点中最多指向子树的指针个数，为2v */
#define MAXNUM_DATA (ORDER_V * 2)    /* 叶子结点中最多数据个数，为2v */
/* 键值的类型*/
typedef int KEY_KIND;    /* 为简单起见，定义为int类型，实际的B+树键值类型应该是可配的 */
/*备注： 为简单起见，叶子结点的数据也只存储键值*/
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

//用于
#define BLOCK_TLABE '1'
#define BLOCK_GRAPH '2'
#define BLOCK_INTER '3'
#define BLOCK_LEAF '4'
#define BLOCK_UNAVA '5'
#define BLOCK_FREE '0'

#define LOC_TABLE 0
#define LOC_GRAPH 1
#define LOC_ROOT 2
enum NODE_TYPE
{
	NODE_TYPE_ROOT = 1,    // 根结点
	NODE_TYPE_INTERNAL = 2,    // 内部结点
	NODE_TYPE_LEAF = 3,    // 叶子结点
};


typedef struct {
	char fpath[100];
	off_t offt_root;
	off_t offt_leftHead;
	off_t offt_rightHead;
	int m_Depth;//树的深度
	KEY_KIND key_kind;//索引键的类型
	size_t max_key_size;
	size_t key_use_block;               /** 数据块为btree_key类型的总数 */
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
	off_t offt_PrevNode;                    //前一个节点在文件中的偏移位置
	off_t offt_NextNode;                    //后一个位置在文件中的偏移位置
	size_t count;
	//有点没意义，后面可能会废除
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
	size_t max_size;
	KEY_KIND key_kind;
	Index(){}
	Index(const char* fname, off_t offt,
		off_t max_size, KEY_KIND key_type)
		:offt_self(offt), max_size(max_size), key_kind(key_type) {
		memcpy(fpath, fname, strlen(fname));
		fpath[strlen(fname)] = '\0';
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
	bool table_create(const char* path, KEY_KIND key_type, size_t max_key_size);
	void flush_key(void* key[MAXNUM_KEY], Index index);
	void flush_value(void* value[MAXNUM_DATA], Index index);
	void get_key(void* key[MAXNUM_KEY], Index index);
	void get_value(void* value[MAXNUM_DATA], Index index);
	void get_BlockGraph(const char* fname,char* freeBlock);
	char get_BlockType(const char* fname, off_t offt);
	void flush_BlockGraph(Index index, char* freeBlock);
	off_t getFreeBlock(const char* filename,char type_block);
	bool flushBlock(const char* filename, off_t offt, char type);
	off_t newBlock(const char* filename);
	//获取文件总共有多少块
	size_t getFileSize(const char* fileName);

	protected:
	static FileManager* object;
	

};
//FileManager* FileManager::object = NULL;