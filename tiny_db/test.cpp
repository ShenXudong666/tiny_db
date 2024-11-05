#include"rwdata.h"
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
	cout << "写入成功" << endl;
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
	cout << "读取成功" << endl;
	cout << bt.child << " " << bt.value << endl;
}

// 修改help函数，使其通过引用传递void*类型的参数
void help(void** data, int type) {
	if (type == 1) {
		for (int i = 0; i < 10; i++) {
			data[i] = (void*)new int(14); // 使用new分配内存
		}
		
	}
	else {
		for (int i = 0; i < 10; i++) {
			data[i] = (void*)new char[6](); // 使用new分配内存并初始化字符串
			strcpy((char*)data[i], "hello");
		}
		
	}
}
void test_write_char(const char* data,int dataSize,int position) {
	FILE* file = fopen("output.bin", "rb+"); // 以读/写二进制模式打开文件
	if (fseek(file, position, SEEK_SET) != 0) {
		std::cerr << "Failed to seek to the specified position." << std::endl;
		fclose(file);
		return ;
	}

	// 使用fwrite写入数据
	size_t bytesWritten = fwrite(data, sizeof(char), dataSize, file);
	// 关闭文件
	fclose(file);

	cout << "Data written to file successfully." << endl;
}
void test_read_char(char* data, int dataSize, int position) {
	// 打开文件
	FILE* file = fopen("output.bin", "rb"); // 以二进制写入模式打开文件
	if (fseek(file, position, SEEK_SET) != 0) {
		std::cerr << "Failed to seek to the specified position." << std::endl;
		fclose(file);
		return;
	}
	size_t bytesRead = fread(data, sizeof(char), dataSize, file);
	cout << "Data read from file: " << data << endl;
	fclose(file); // 关闭文件

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
	FileManager::getInstance()->table_create(fname, LL_KEY, sizeof(long long));
	//cout << "表头写入成功" << endl;
	table t= FileManager::getInstance()->getTable("table.bin", 0);
	cout << "读取表成功" << endl;
	cout << "表的索引值为：" << t.key_type << endl;
	cout << "max_size: " << t.max_key_size << endl;
	Index index;
	memcpy(index.fpath, fname, sizeof(fname) + 1);
	index.fpath[sizeof(fname) + 1] = '\0';
	index.max_size = sizeof(long long);
	index.key_type = LL_KEY;
	void* data[MAXNUM_DATA];
	leaf_node l = FileManager::getInstance()->getLeafNode(index, data, 1);
	cout << *(int*)data[0] << " "<< *(int*)data[1] << endl;
	cout << "根的偏移值为" << endl;
	cout << l.offt_self << endl;

}
void func3(void** data) {
	FILE* file = fopen("output.bin", "rb+"); // 以读/写二进制模式打开文件
	if (fseek(file, 2*DB_BLOCK_SIZE, SEEK_SET) != 0) {
		std::cerr << "Failed to seek to the specified position." << std::endl;
		fclose(file);
		return;
	}
	int arr[5];
	cout << "写进过程" << endl;
	for (int i = 0; i < 5; i++) {
		arr[i] = *(int*)data[i];
		cout << arr[i] << endl;
	}
	fwrite(&arr, sizeof(int), 5, file);
	fclose;
}
void func4(void** data) {
	FILE* file = fopen("output.bin", "rb"); // 以读/写二进制模式打开文件
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
int main() {
	//void* data = nullptr; // 初始化data为nullptr
	//help(data, 2); // 传递data的引用
	//cout << data << endl; // 输出data的地址

	//cout << "The value is: " << strlen((char*)data) << endl;
	//newBlock();
	/*const char* data1 = "Hello, World!";
	
	size_t dataSize = strlen(data1) + 10;
	char* data = new char[dataSize + 1];
	int position = 64;
	test_write_char(data1, dataSize, position);
	test_read_char(data, 100, position);*/
	func2();
	/*void* data[5];
	for (int i = 0; i < 5; i++) {
		data[i] = (void*)new int(4);
		cout << *(int*)data[i] << endl;
	}
	func3(data);
	cout << "data写入成功" << endl;
	void* data1[5];
	func4(data1);
	cout << "data1读进成功" << endl;
	cout << *(int*)data1[0] << endl;*/


	return 0;
}