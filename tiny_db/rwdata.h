#include "stdio.h"
#include "stdlib.h"
#define DB_HEAD_SIZE 4096 // head size must be pow of 2! 文件数据库的头大小
#define DB_BLOCK_SIZE 8192 // block size must be pow of 2! 文件数据库的数据块大小
#include<string>
#include<iostream>
#include<fstream>
#include"BPlusTree.h"
using namespace std;

typedef struct {
	string fpath;
	off_t offt_root;
	off_t offt_leftHead;
	off_t offt_rightHead;
	int m_Depth;//树的深度
	int key_type;						//索引键的类型
	size_t key_use_block;               /** 数据块为btree_key类型的总数 */
	size_t value_use_block;
}table;


typedef struct {
	off_t offt_self;
	off_t offt_pointers[MAXNUM_POINTER];
	off_t offt_father;

	int node_type;
	KEY_TYPE m_Keys[MAXNUM_KEY];

}inter_node;

typedef struct {
	off_t offt_self;
	off_t offt_father;
	off_t offt_PrevNode;                    //前一个节点在文件中的偏移位置
	off_t offt_NextNode;                    //后一个位置在文件中的偏移位置

	int node_type;
	KEY_TYPE m_Datas[MAXNUM_DATA];
}leaf_node;

class FileManager {

public:
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

	bool flushInterNode(inter_node node,string fname, off_t offt) {
		const char* filename = fname.c_str();
		FILE* file = fopen(filename, "rb+");
		if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
			perror("Failed to seek");
			fclose(file);
		}
		fwrite(&node, 1, sizeof(inter_node), file);
		fclose(file);

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

	}

	void newBlock(string fname) {
		const char* filename = fname.c_str();
		FILE* file = fopen(filename, "ab");
		char zero[DB_BLOCK_SIZE] = { 0 };
		fwrite(zero, 1, sizeof(zero), file);
		fclose(file);
	}


protected:



};