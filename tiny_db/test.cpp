#include"BPlusTree.h"
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
void outTry2() {
	
	ofstream outfile("example.bin", ios::out | ios::binary);
	btree_key bt;
	outfile.seekp(100,ios::beg);
	bt.child = 10;
	bt.value = 10;
	outfile.write((char*)&bt, sizeof(bt));
	cout << "д��ɹ�" << endl;
	outfile.close();
}
void inTry2() {
	ifstream infile("example.bin", ios::in | ios::binary);
	/*streamoff offset = 10;
	infile.seekg(offset, ios::beg);*/
	btree_key bt;
	infile.seekg(50, ios::beg);
	infile.read((char*)&bt, sizeof(bt));
	
	cout << bt.key << " " << endl;
	cout << "��ȡ�ɹ�" << endl;

	infile.close();
	
}


// �޸�help������ʹ��ͨ�����ô���void*���͵Ĳ���
void help(void*& data, int type) {
	if (type == 1) {
		data = (void*)new int(14); // ʹ��new�����ڴ�
	}
	else {
		data = (void*)new char[6](); // ʹ��new�����ڴ沢��ʼ���ַ���
		strcpy((char*)data, "hello");
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
int main() {
	//void* data = nullptr; // ��ʼ��dataΪnullptr
	//help(data, 2); // ����data������
	//cout << data << endl; // ���data�ĵ�ַ

	//cout << "The value is: " << strlen((char*)data) << endl;
	//newBlock();
	const char* data1 = "Hello, World!";
	
	size_t dataSize = strlen(data1) + 10;
	char* data = new char[dataSize + 1];
	int position = 64;
	test_read_char(data, 100, position);
	//test_write_char(data, dataSize, position);
	return 0;
}