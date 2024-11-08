#include "rwdata.h"

 FileManager* FileManager::getInstance() {
	static FileManager* m = new FileManager();
	
	return m;
}
inter_node FileManager::getCInternalNode(Index index,void *data[MAXNUM_KEY],off_t offt) {
	FILE* file = fopen(index.fpath, "rb");
	if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	inter_node node;
	fread(&node, 1, sizeof(inter_node), file);
	off_t arr_offt = ftell(file);

	index.offt_self = arr_offt;
	fclose(file);
	get_key(data, index);
	index.offt_self = offt;

	return node;

}
bool FileManager::flushInterNode(inter_node node, Index index,void** key) {

	FILE* file = fopen(index.fpath, "rb+");
	if (fseek(file, index.offt_self * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	fwrite(&node, 1, sizeof(inter_node), file);
	off_t offset = ftell(file);
	index.offt_self = offset;
	fclose(file);
	flush_key(key, index);
	return true;
}
leaf_node FileManager::getLeafNode(Index index, void *data[MAXNUM_DATA], off_t offt) {

	FILE* file = fopen(index.fpath, "rb");
	cout << "读取叶节点的文件名为 " << index.fpath << endl;
	if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	leaf_node node;
	fread(&node, 1, sizeof(leaf_node), file);
	off_t arr_offt = ftell(file);
	
	index.offt_self = arr_offt;
	fclose(file);
	get_value(data, index);
	index.offt_self = offt;
	return node;

}
bool FileManager::flushLeafNode(leaf_node node, Index index, void** value) {
	//以这个为例，开始写变长的索引
	FILE* file = fopen(index.fpath, "rb+");
	if (fseek(file, node.offt_self * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	fwrite(&node, 1, sizeof(leaf_node), file);
	off_t offset = ftell(file);
	index.offt_self = offset;
	fclose(file);
	//在这里之后得到偏移指针的偏移
	flush_value(value, index);
	
	return true;
}

table FileManager::getTable(const char* filename, off_t offt) {

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
bool FileManager::flushTable(table t, const char* filename, off_t offt) {

	FILE* file = fopen(filename, "rb+");
	offt = offt * DB_BLOCK_SIZE;
	cout << filename << endl;
	if (fseek(file, offt, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	fwrite(&t, 1, sizeof(table), file);
	fclose(file);
	return true;
}
void FileManager::flush_value(void* value[MAXNUM_DATA], Index index)
{
	FILE* file = fopen(index.fpath, "rb+");
	cout << "写进value的偏移量为" << index.offt_self << endl;
	if (fseek(file, index.offt_self, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	if (index.key_type == INT_KEY) {
		int temp[MAXNUM_DATA];
		for (int i = 0; i < MAXNUM_DATA; i++) {
			temp[i] = *(int*)value[i];
		}
		fwrite(&temp, sizeof(int), MAXNUM_DATA, file);
	}
	else if (index.key_type == LL_KEY) {
		long long temp[MAXNUM_DATA];
		for (int i = 0; i < MAXNUM_DATA; i++) {
			temp[i] = *(long long*)value[i];
		}
		fwrite(&temp, sizeof(long long), MAXNUM_DATA, file);
	}
	else {
		//暂时这样，后面可能有bug要修改
		

		for (int i = 0; i < MAXNUM_DATA; i++) {
			char* temp = new char(index.max_size);
			temp = (char*)value[i];
			cout << "写入数据前为" << temp << endl;
			int t=fwrite(temp, sizeof(char), index.max_size, file);
			cout << "写了位数为：" << t << endl;
			delete temp;
		}
		
	}

	fclose(file);
}

void FileManager::get_key(void* key[MAXNUM_KEY], Index index)
{
	FILE* file = fopen(index.fpath, "rb");
	if (fseek(file, index.offt_self, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	void* temp;
	if (index.key_type == INT_KEY) {
		temp = new int();
	}
	else if (index.key_type == LL_KEY) {
		temp = new long long();
	}
	else {
		temp = new char[index.max_size];
	}
	for (int i = 0; i < MAXNUM_KEY; i++) {
		fread(temp, index.max_size, 1, file);
		key[i] = (void*)temp;
	}
	fclose(file);
}

void FileManager::get_value(void* value[MAXNUM_DATA], Index index)
{
	FILE* file = fopen(index.fpath, "rb");
	cout<<"index的文件" << index.fpath << endl;
	if (fseek(file, index.offt_self, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	//void* temp;
	cout << "读取value的偏移量为" << index.offt_self << endl;
	cout << "index.key" << endl;
	cout << index.key_type << endl;

	if (index.key_type == INT_KEY) {
		int *temp = new int();
		for (int i = 0; i < MAXNUM_DATA; ++i) {

			if (fread(temp, sizeof(int), 1, file) != 1) {
				cout << *(int*)value[i] << endl;
				perror("Failed to read data");
				fclose(file);
				return;
			}
			value[i] = (void*)temp;
		}
	}
	else if (index.key_type == LL_KEY) {
		long long *temp = new long long();
		for (int i = 0; i < MAXNUM_DATA; ++i) {

			if (fread(temp, sizeof(long long), 1, file) != 1) {
				//cout << *(long long*)value[i] << endl;
				perror("Failed to read data");
				fclose(file);
				return;
			}
			value[i] = (void*)temp;
		}
	}
	else {
		char temp[1024] = { 0 };
		
		for (int i = 0; i < MAXNUM_DATA; ++i) {

			if (fread(temp, sizeof(char), index.max_size, file) != index.max_size) {
				//cout << *(long long*)value[i] << endl;
				perror("Failed to read data");
				fclose(file);
				return;
			}
			value[i] = (void*)temp;
			cout << "读取后字符串数据为 " << (char*)value[i] << endl;
			
			
		}
	}
	//cout << "size " << size << " " << index.offt_self << endl;

	
	fclose(file);
}

void FileManager::get_BlockGraph(const char* fname, char* freeBlock)
{
	FILE* file=fopen(fname, "rb");
	if (fseek(file, 1*DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}

	if (fread(freeBlock, sizeof(char), NUM_ALL_BLOCK, file) != NUM_ALL_BLOCK) {
		perror("Failed to read data");
		fclose(file);
		return;
	}
	fclose(file);
	cout << "读取成功,前10位为：" << endl;
	for (int i = 0; i < 10; i++)cout << freeBlock[i];
	//这一步是防止出问题,已使用的块大于文件真正的大小
	size_t i=getFileSize(fname);
	i++;
	for (i; i < NUM_ALL_BLOCK; i++) {
		freeBlock[i] = BLOCK_UNAVA;
	}
	cout << endl;

}

void FileManager::flush_BlockGraph(Index index, char* freeBlock)
{
	FILE* file=fopen(index.fpath, "rb+");
	if (fseek(file, index.offt_self * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}

	fwrite(freeBlock, sizeof(char), index.max_size, file);
	fclose(file);
	cout << "读取成功,空闲块的分布为,前10位为：" << endl;
	for (int i = 0; i < 10; i++)cout << freeBlock[i];
	cout << endl;

}


void FileManager::flush_key(void* key[MAXNUM_KEY], Index index)
{
	FILE* file = fopen(index.fpath, "rb+");
	cout << "写进key的偏移量为" << index.offt_self << endl;
	if (fseek(file, index.offt_self, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	if (index.key_type == INT_KEY) {
		int temp[MAXNUM_KEY];
		for (int i = 0; i < MAXNUM_KEY; i++) {
			temp[i] = *(int*)key[i];
		}
		fwrite(&temp, sizeof(int), MAXNUM_KEY, file);
	}
	else if (index.key_type == LL_KEY) {
		long long temp[MAXNUM_KEY];
		for (int i = 0; i < MAXNUM_KEY; i++) {
			temp[i] = *(long long*)key[i];
		}
		fwrite(&temp, sizeof(long long), MAXNUM_KEY, file);
	}
	else {
		//暂时这样，后面可能有bug要修改


		for (int i = 0; i < MAXNUM_KEY; i++) {
			char* temp = new char(index.max_size);
			temp = (char*)key[i];
			cout << "写入数据前为" << temp << endl;
			int t = fwrite(temp, sizeof(char), index.max_size, file);
			cout << "写了位数为：" << t << endl;
			delete temp;
		}

	}

	fclose(file);
}

bool FileManager::table_create(const char* path, KEY_TYPE key_type, size_t max_key_size)
{
	size_t size = 0;
	//纠正最大索引所占空间
	switch (key_type)
	{
	case INT_KEY:
		size = sizeof(int);
		break;
	case LL_KEY:
		size = sizeof(long long);
		break;
	case STRING_KEY:
		size = max_key_size;
		break;
	default:
		size = max_key_size;
		break;
	}

	//对原本文件的大小进行重新初始化，如果原本的文件存在的话
	FILE* file = fopen(path, "r");
	if (file) {
		fclose(file);
		remove(path); // 删除文件
		cout << "File deleted successfully." << endl;
	}
	for(int i=0;i<6;i++)newBlock(path);

	table t;
	memcpy(t.fpath, path, sizeof(path)+1);
	t.fpath[sizeof(path)+1] = '\0';

	t.key_type = key_type;
	t.m_Depth = 1;
	t.offt_root = 1;
	t.key_use_block = 1;
	t.value_use_block = 0;
	
	t.max_key_size = size;
	//将表的信息写在文件的头部
	flushTable(t,t.fpath ,0);
	//===========表信息写入完成=================================
	
	//将根的信息写在文件头部的后面一块
	leaf_node root(2,0,NODE_TYPE_ROOT);
	void* data[MAXNUM_DATA];
	//初次创建表，根的值默认为最大的0
	for (int i = 0; i < MAXNUM_DATA; i++) {
		data[i] = (void*)new int(666);
	}
	Index index(path,2,t.max_key_size,t.key_type);
	
	flushLeafNode(root, index,data);
	//======================根信息写入成功========================
	char block_graph[NUM_ALL_BLOCK];
	block_graph[0] = BLOCK_TLABE;
	block_graph[1] = BLOCK_GRAPH;
	block_graph[2] = BLOCK_LEAF;
	for(int j=3;j<6;j++)block_graph[j] = BLOCK_FREE;
	for (int j = 6; j < NUM_ALL_BLOCK; j++)block_graph[j] = BLOCK_UNAVA;
	index.offt_self = 1;
	index.max_size = NUM_ALL_BLOCK;
	flush_BlockGraph(index, block_graph);
	//=======================空闲表写入成功======================
	return true;
}
void FileManager::newBlock(const char* filename) {

	FILE* file = fopen(filename, "ab");
	char zero[DB_BLOCK_SIZE] = { 0 };
	fwrite(zero, 1, sizeof(zero), file);
	fclose(file);
}

size_t FileManager::getFileSize(const char* fileName)
{
	struct stat statbuf;

	// 提供文件名字符串，获得文件属性结构体
	stat(fileName, &statbuf);

	// 获取文件大小
	size_t filesize = statbuf.st_size;

	return filesize / DB_BLOCK_SIZE;
	
}

