#include"rwdata.h"
#include "BPlusTree.h"
#include<iostream>
#include<fstream>
#include<unistd.h>
using namespace std;

#define BLOCK_SIZE 4096
typedef struct {
	off_t value;
	off_t child;
	unsigned char key[0];
	
} btree_key;

void newBlock() {
	FILE* file = fopen("output.bin", "ab");
	char zero[BLOCK_SIZE] = { 0 };
	fwrite(zero, 1, sizeof(zero), file);
	fclose(file);
}

void outTry(off_t offset) {
	FILE* file = fopen("example.bin", "rb+");
	btree_key bt;
	bt.child = 20;
	bt.value = 20;

	if (fseek(file, offset, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	
	fwrite(&bt, sizeof(btree_key), 1, file);
	fclose(file);
	cout << "д��ɹ�" << endl;
}
void inTry(off_t offset) {
	FILE* file = fopen("example.bin", "rb");
	btree_key bt;

	if (fseek(file, offset, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}

	fread(&bt, sizeof(btree_key), 1, file);
	fclose(file);
	cout << "��ȡ�ɹ�" << endl;
	cout << bt.child << " " << bt.value << endl;
}

// �޸�help������ʹ��ͨ�����ô���void*���͵Ĳ���
void help(void** data, int type) {
	if (type == 1) {
		for (int i = 0; i < 10; i++) {
			data[i] = (void*)new int(14); // ʹ��new�����ڴ�
		}
		
	}
	else {
		for (int i = 0; i < 10; i++) {
			data[i] = (void*)new char[6](); // ʹ��new�����ڴ沢��ʼ���ַ���
			strcpy((char*)data[i], "hello");
		}
		
	}
}
void test_write_char(const char* data,int dataSize,int position) {
	FILE* file = fopen("output.bin", "rb+"); // �Զ�/д������ģʽ���ļ�
	if (fseek(file, position, SEEK_SET) != 0) {
		std::cerr << "Failed to seek to the specified position." << std::endl;
		fclose(file);
		return ;
	}

	// ʹ��fwriteд������
	size_t bytesWritten = fwrite(data, sizeof(char), dataSize, file);
	// �ر��ļ�
	fclose(file);

	cout << "Data written to file successfully." << endl;
}
void test_read_char(char* data, int dataSize, int position) {
	// ���ļ�
	FILE* file = fopen("output.bin", "rb"); // �Զ�����д��ģʽ���ļ�
	if (fseek(file, position, SEEK_SET) != 0) {
		std::cerr << "Failed to seek to the specified position." << std::endl;
		fclose(file);
		return;
	}
	size_t bytesRead = fread(data, sizeof(char), dataSize, file);
	cout << "Data read from file: " << data << endl;
	fclose(file); // �ر��ļ�

}
void func1() {
	void* data[10];
	help(data, 1);
	//cout << ((char*)data[0] <(char*)data[1]) << endl;
	for (int i = 0; i < 10; i++) {
		cout << *(int*)data[i] << endl;
	}
}
void func2() {
	//FileManager::getInstance()->newBlock("table.bin");
	/*ss::newoneBlock("table.bin");
	ss::newoneBlock("table.bin");*/
	const char* fname = "table.bin";
	FileManager::getInstance()->table_create(fname, INT_KEY, sizeof(int));
	//cout << "��ͷд��ɹ�" << endl;
	table t= FileManager::getInstance()->getTable("table.bin", 0);
	cout << "��ȡ��ɹ�" << endl;
	cout << "�������ֵΪ��" << t.key_type << endl;
	cout << "max_size: " << t.max_key_size << endl;
	Index index;
	memcpy(index.fpath, fname, sizeof(fname) + 1);
	index.fpath[sizeof(fname) + 1] = '\0';
	index.max_size = sizeof(int);
	index.key_type = INT_KEY;
	void* data[MAXNUM_DATA];
	leaf_node l = FileManager::getInstance()->getLeafNode(index, data, 1);
	cout << *(int*)data[0] << " "<< *(int*)data[1] << endl;
	cout << "����ƫ��ֵΪ" << endl;
	cout << l.offt_self << endl;

}
void func3(void** data) {
	FILE* file = fopen("output.bin", "rb+"); // �Զ�/д������ģʽ���ļ�
	if (fseek(file, 2*DB_BLOCK_SIZE, SEEK_SET) != 0) {
		std::cerr << "Failed to seek to the specified position." << std::endl;
		fclose(file);
		return;
	}
	int arr[5];
	cout << "д������" << endl;
	for (int i = 0; i < 5; i++) {
		arr[i] = *(int*)data[i];
		cout << arr[i] << endl;
	}
	fwrite(&arr, sizeof(int), 5, file);
	fclose;
}
void func4(void** data) {
	FILE* file = fopen("output.bin", "rb"); // �Զ�/д������ģʽ���ļ�
	if (fseek(file, 2*DB_BLOCK_SIZE, SEEK_SET) != 0) {
		std::cerr << "Failed to seek to the specified position." << std::endl;
		fclose(file);
		return;
	}
	
	int* temp = new int();
	for (int i = 0; i < 5; i++) {
		fread(temp, sizeof(int), 1, file);
		data[i] = (void*)temp;
	}
	fclose;
}
void xiebiao(KEY_TYPE key) {
	const char* fname = "table.bin";
	if(key==INT_KEY)FileManager::getInstance()->table_create(fname, key, sizeof(int));
	else if(key==LL_KEY)FileManager::getInstance()->table_create(fname, key, sizeof(long long));
	else FileManager::getInstance()->table_create(fname, key, 100);
}
int main() {
	//void* data = nullptr; // ��ʼ��dataΪnullptr
	//help(data, 2); // ����data������
	//cout << data << endl; // ���data�ĵ�ַ

	//cout << "The value is: " << strlen((char*)data) << endl;
	//newBlock();
	/*const char* data1 = "Hello, World!";
	
	size_t dataSize = strlen(data1) + 10;
	char* data = new char[dataSize + 1];
	int position = 64;
	test_write_char(data1, dataSize, position);
	test_read_char(data, 100, position);*/
	
	/*void* data[5];
	for (int i = 0; i < 5; i++) {
		data[i] = (void*)new int(4);
		cout << *(int*)data[i] << endl;
	}
	func3(data);
	cout << "dataд��ɹ�" << endl;
	void* data1[5];
	func4(data1);
	cout << "data1�����ɹ�" << endl;
	cout << *(int*)data1[0] << endl;*/

	//func2();
	//xiebiao(INT_KEY);
	CLeafNode* root = new CLeafNode(1);
	const char* fname = "table.bin";
	root->get_file(fname, INT_KEY, sizeof(4));
	for (int i = 0; i < MAXNUM_DATA; i++) cout << *(int*)(root->values[i]) << endl;
	
	

	return 0;
}