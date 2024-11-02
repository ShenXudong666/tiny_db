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

	inter_node getCInternalNode(const char* filename, off_t offt);
	bool flushInterNode(inter_node node, const char* filename, off_t offt);
	leaf_node getLeafNode(const char* filename, off_t offt);
	bool flushLeafNode(leaf_node node, const char* filename, off_t offt);
	table getTable(const char* filename, off_t offt);
	bool flushTable(table t, const char* filename, off_t offt);
	bool table_create(const char* path, KEY_TYPE key_type, size_t max_key_size);
	void newBlock(const char* filename) {

		FILE* file = fopen(filename, "ab");
		char zero[DB_BLOCK_SIZE] = { 0 };
		fwrite(zero, 1, sizeof(zero), file);
		fclose(file);
	}


protected:
	 static FileManager* object;


};
//FileManager* FileManager::object = NULL;