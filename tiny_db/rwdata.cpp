#include "rwdata.h"

 FileManager* FileManager::getInstance() {
	static FileManager* m = new FileManager();
	
	return m;
}
inter_node FileManager::getCInternalNode(const char* filename, off_t offt) {
	FILE* file = fopen(filename, "rb");
	if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	inter_node node;
	fread(&node, 1, sizeof(inter_node), file);
	fclose(file);
	return node;

}
bool FileManager::flushInterNode(inter_node node, const char* filename, off_t offt) {

	FILE* file = fopen(filename, "rb+");
	if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	fwrite(&node, 1, sizeof(inter_node), file);
	fclose(file);
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
	off_t offset = ftell(file);
	
	index.offt_self = offset;
	fclose(file);
	get_value(data, index);

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

void FileManager::flush_key(void* key[MAXNUM_KEY], Index index)
{
	FILE* file = fopen(index.fpath, "rb+");
	cout << "写进value的偏移量为" << index.offt_self << endl;
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
	table t;
	memcpy(t.fpath, path, sizeof(path)+1);
	t.fpath[sizeof(path)+1] = '\0';
	cout << path << endl;
	cout << t.fpath << endl;
	t.key_type = key_type;
	t.m_Depth = 1;
	t.offt_root = 1;
	t.key_use_block = 1;
	t.value_use_block = 0;
	t.max_key_size = size;
	cout << size << endl;
	//将表的信息写在文件的头部
	flushTable(t,t.fpath ,0);
	//将根的信息写在文件头部的后面一块
	leaf_node root;
	root.offt_self = 1;
	
	void* data[MAXNUM_DATA];
	//初次创建表，根的值默认为最大的0
	for (int i = 0; i < MAXNUM_DATA; i++) {
		data[i] = (void*)new int(13);
	}
	Index index;
	memcpy(index.fpath, t.fpath, sizeof(t.fpath));
	index.max_size = t.max_key_size;
	index.key_type = t.key_type;

	/*for (int i = 0; i < MAXNUM_DATA; i++) {
		char* temp= new char[index.max_size];
		const char* s = "success";
		memcpy(temp, s, sizeof(s)+1);
		temp[sizeof(s) + 1] = '\0';
		cout << "创建table之后写入数据的东西为" << temp << endl;
		cout << sizeof(temp) << endl;
		data[i] = (void*)temp;
	}*/
	
	
	flushLeafNode(root, index,data);


	return false;
}
void FileManager::newBlock(const char* filename) {

	FILE* file = fopen(filename, "ab");
	char zero[DB_BLOCK_SIZE] = { 0 };
	fwrite(zero, 1, sizeof(zero), file);
	fclose(file);
}

