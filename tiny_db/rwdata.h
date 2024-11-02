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

using namespace std;
#define INT_KEY 1
#define LL_KEY 2
#define STRING_KEY 3



typedef struct {
	string fpath;
	off_t offt_root;
	off_t offt_leftHead;
	off_t offt_rightHead;
	int m_Depth;//�������
	int key_type;//������������
	int key_max_size;
	size_t key_use_block;               /** ���ݿ�Ϊbtree_key���͵����� */
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
	off_t offt_PrevNode;                    //ǰһ���ڵ����ļ��е�ƫ��λ��
	off_t offt_NextNode;                    //��һ��λ�����ļ��е�ƫ��λ��

	int node_type;
	//KEY_TYPE m_Datas[MAXNUM_DATA];
	int value;								//���ڲ��ԣ�����Ҫɾ��
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