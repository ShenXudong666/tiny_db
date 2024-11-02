#pragma once
#include "stdio.h"
#include "stdlib.h"
#define DB_HEAD_SIZE 4096 // head size must be pow of 2! 文件数据库的头大小
#define DB_BLOCK_SIZE 8192 // block size must be pow of 2! 文件数据库的数据块大小
#define ORDER_V 2    /* 为简单起见，把v固定为2，实际的B+树v值应该是可配的。这里的v是内部节点中键的最小值 */
#define MAXNUM_KEY (ORDER_V * 2)    /* 内部结点中最多键个数，为2v */
#define MAXNUM_POINTER (MAXNUM_KEY + 1)    /* 内部结点中最多指向子树的指针个数，为2v */
#define MAXNUM_DATA (ORDER_V * 2)    /* 叶子结点中最多数据个数，为2v */
/* 键值的类型*/
typedef int KEY_TYPE;    /* 为简单起见，定义为int类型，实际的B+树键值类型应该是可配的 */
/*备注： 为简单起见，叶子结点的数据也只存储键值*/
#include<string>
#include<iostream>
#include<fstream>

using namespace std;
#define INT_KEY 1
#define LL_KEY 2
#define STRING_KEY 3



typedef struct {
	string fpath;
	off_t offt_root;
	off_t offt_leftHead;
	off_t offt_rightHead;
	int m_Depth;//树的深度
	int key_type;//索引键的类型
	int key_max_size;
	size_t key_use_block;               /** 数据块为btree_key类型的总数 */
	size_t value_use_block;
}table;


typedef struct {
	off_t offt_self;
	off_t offt_pointers[MAXNUM_POINTER];
	off_t offt_father;

	int node_type;
	//KEY_TYPE m_Keys[MAXNUM_KEY];

}inter_node;

typedef struct {
	off_t offt_self;
	off_t offt_father;
	off_t offt_PrevNode;                    //前一个节点在文件中的偏移位置
	off_t offt_NextNode;                    //后一个位置在文件中的偏移位置

	int node_type;
	//KEY_TYPE m_Datas[MAXNUM_DATA];
	int value;								//用于测试，后续要删除
}leaf_node;

class FileManager {

public:

	static FileManager* getInstance() {
		static FileManager* m = new FileManager();
		return m;
		if (object == NULL) {
			object = new FileManager();
		}
		return object;
	}

	inter_node getCInternalNode(string fname, off_t offt) {
		const char* filename = fname.c_str();
		FILE* file = fopen(filename, "rb");
		if (fseek(file, offt* DB_BLOCK_SIZE, SEEK_SET) != 0) {
			perror("Failed to seek");
			fclose(file);
		}
		inter_node node;
		fread(&node, 1, sizeof(inter_node), file);
		fclose(file);
		return node;
		
	}
	bool flushInterNode(inter_node node, string fname, off_t offt) {
		const char* filename = fname.c_str();
		FILE* file = fopen(filename, "rb+");
		if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
			perror("Failed to seek");
			fclose(file);
		}
		fwrite(&node, 1, sizeof(inter_node), file);
		fclose(file);
		return true;
	}

	leaf_node getLeafNode(string fname, off_t offt) {
		const char* filename = fname.c_str();
		FILE* file = fopen(filename, "rb");
		if (fseek(file, offt* DB_BLOCK_SIZE, SEEK_SET) != 0) {
			perror("Failed to seek");
			fclose(file);
		}
		leaf_node node;
		fread(&node, 1, sizeof(leaf_node), file);
		fclose(file);
		
		return node;

	}
	bool flushLeafNode(leaf_node node, string fname, off_t offt) {
		const char* filename = fname.c_str();
		FILE* file = fopen(filename, "rb+");
		if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
			perror("Failed to seek");
			fclose(file);
		}
		fwrite(&node, 1, sizeof(leaf_node), file);
		fclose(file);
		return true;
	}
	table getTable (string fname, off_t offt) {
		const char* filename = fname.c_str();
		FILE* file = fopen(filename, "rb");
		if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
			perror("Failed to seek");
			fclose(file);
		}
		table t;
		fread(&t, 1, sizeof(table), file);
		fclose(file);

		return t;

	}
	bool flushTable(table t, string fname, off_t offt) {
		const char* filename = fname.c_str();
		FILE* file = fopen(filename, "rb+");
		if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
			perror("Failed to seek");
			fclose(file);
		}
		fwrite(&t, 1, sizeof(table), file);
		fclose(file);
		return true;
	}
	



	void newBlock(string fname) {
		const char* filename = fname.c_str();
		FILE* file = fopen(filename, "ab");
		char zero[DB_BLOCK_SIZE] = { 0 };
		fwrite(zero, 1, sizeof(zero), file);
		fclose(file);
	}


protected:
	 static FileManager* object;


};
//FileManager* FileManager::object = NULL;