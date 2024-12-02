#include "rwdata.h"
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <string.h>
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
	get_value(data, index);
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
	cout<<"读取表成功"<<endl;
	return t;

}
bool FileManager::flushTable(table t, const char* filename, off_t offt) {

	FILE* file = fopen(filename, "rb+");
	offt = offt * DB_BLOCK_SIZE;
	if (fseek(file, offt, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	fwrite(&t, 1, sizeof(table), file);
	fclose(file);
	cout<<"表更新成功"<<endl;
	return true;
}
void FileManager::flush_value(void* value[MAXNUM_DATA], Index index)
{
	FILE* file = fopen(index.fpath, "rb+");
	if (fseek(file, index.offt_self, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}

	if (index.key_kind == INT_KEY) {
		int temp[MAXNUM_DATA];
		for (int i = 0; i < MAXNUM_DATA; i++) {
			temp[i] = *(int*)value[i];
			//cout << "写入数据为" << *(int*)value[i] << endl;
		}
		fwrite(&temp, sizeof(int), MAXNUM_DATA, file);
	}
	else if (index.key_kind == LL_KEY) {
		long long temp[MAXNUM_DATA];
		for (int i = 0; i < MAXNUM_DATA; i++) {
			temp[i] = *(long long*)value[i];
		}
		fwrite(&temp, sizeof(long long), MAXNUM_DATA, file);
	}
	else {
		//暂时这样，后面可能有bug要修改
		

		for (int i = 0; i < MAXNUM_DATA; i++) {
			char* temp;
			temp = (char*)value[i];
			cout << "写入数据前为" << temp << endl;
			int t=fwrite(temp, sizeof(char), index.max_size, file);
			//delete temp;
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
	if (index.key_kind == INT_KEY) {
		
		for (int i = 0; i < MAXNUM_DATA; ++i) {
			int *temp = new int();
			if (fread(temp, sizeof(int), 1, file) != 1) {
				
				perror("Failed to read data");
				fclose(file);
				return;
			}
			key[i] = (void*)temp;
			//cout<<"读取后数据为"<<*(int*)value[i]<<endl;
		}
	}
	else if (index.key_kind == LL_KEY) {
		
		for (int i = 0; i < MAXNUM_DATA; ++i) {
			long long *temp = new long long();
			if (fread(temp, sizeof(long long), 1, file) != 1) {
				
				perror("Failed to read data");
				fclose(file);
				return;
			}
			key[i] = (void*)temp;
		}
	}
	else {
		
		
		for (int i = 0; i < MAXNUM_DATA; ++i) {
			char temp[1024] = { 0 };
			if (fread(temp, sizeof(char), index.max_size, file) != index.max_size) {
				//cout << *(long long*)value[i] << endl;
				perror("Failed to read data");
				fclose(file);
				return;
			}
			key[i] = (void*)temp;
			
			
		}
	}
	//cout << "size " << size << " " << index.offt_self << endl;
	fclose(file);
}

void FileManager::get_value(void* value[MAXNUM_DATA], Index index)
{
	FILE* file = fopen(index.fpath, "rb");
	if (fseek(file, index.offt_self, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	//void* temp;

	if (index.key_kind == INT_KEY) {
		
		for (int i = 0; i < MAXNUM_DATA; ++i) {
			int *temp = new int();
			if (fread(temp, sizeof(int), 1, file) != 1) {
				
				perror("Failed to read data");
				fclose(file);
				return;
			}
			value[i] = (void*)temp;
			//cout<<"读取后数据为"<<*(int*)value[i]<<endl;
		}
	}
	else if (index.key_kind == LL_KEY) {
		
		for (int i = 0; i < MAXNUM_DATA; ++i) {
			long long *temp = new long long();
			if (fread(temp, sizeof(long long), 1, file) != 1) {
				
				perror("Failed to read data");
				fclose(file);
				return;
			}
			value[i] = (void*)temp;
		}
	}
	else {
		
		
		for (int i = 0; i < MAXNUM_DATA; ++i) {
			char* temp=new char[1024];
			if (fread(temp, sizeof(char), index.max_size, file) != index.max_size) {
				//cout << *(long long*)value[i] << endl;
				perror("Failed to read data");
				fclose(file);
				return;
			}
			value[i] =temp;
			//cout << "读取后字符串数据为 " << (char*)value[i] << endl;
			
			
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
	
	//for (int i = 0; i < 10; i++)cout << freeBlock[i];
	//这一步是防止出问题,已使用的块大于文件真正的大小
	size_t i=getFileSize(fname);
	i++;
	for (int j=i; j < NUM_ALL_BLOCK; j++) {
		freeBlock[j] = BLOCK_UNAVA;
	}

}
char FileManager::get_BlockType(const char* fname, off_t offt){
	char* BlockGRAPH = new char[NUM_ALL_BLOCK];
	get_BlockGraph(fname, BlockGRAPH);
	return BlockGRAPH[offt];
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

}

off_t FileManager::getFreeBlock(const char* fname,char type_block)
{
	char* BlockGRAPH = new char[NUM_ALL_BLOCK];
	get_BlockGraph(fname, BlockGRAPH);
	for (int i = 0; i < NUM_ALL_BLOCK; i++) {
		if (BlockGRAPH[i] == BLOCK_FREE) {
			Index index(fname,LOC_GRAPH,NUM_ALL_BLOCK,1);
			BlockGRAPH[i] = type_block;
			flush_BlockGraph(index, BlockGRAPH);
			delete[] BlockGRAPH;
			return i;
		}
	}
	int i=this->newBlock(fname);
	BlockGRAPH[i]=type_block;
	Index index(fname, LOC_GRAPH, NUM_ALL_BLOCK, 1);
	flush_BlockGraph(index, BlockGRAPH);
	return i;
}

void FileManager::flush_key(void* key[MAXNUM_KEY], Index index)
{
	FILE* file = fopen(index.fpath, "rb+");
	if (fseek(file, index.offt_self, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	if (index.key_kind == INT_KEY) {
		int temp[MAXNUM_KEY];
		for (int i = 0; i < MAXNUM_KEY; i++) {
			temp[i] = *(int*)key[i];
		}
		fwrite(&temp, sizeof(int), MAXNUM_KEY, file);
	}
	else if (index.key_kind == LL_KEY) {
		long long temp[MAXNUM_KEY];
		for (int i = 0; i < MAXNUM_KEY; i++) {
			temp[i] = *(long long*)key[i];
		}
		fwrite(&temp, sizeof(long long), MAXNUM_KEY, file);
	}
	else {
		//暂时这样，后面可能有bug要修改


		for (int i = 0; i < MAXNUM_KEY; i++) {
			
			char* temp = (char*)key[i];
			cout << "写入数据前为" << temp << endl;
			int t = fwrite(temp, sizeof(char), index.max_size, file);
		}

	}

	fclose(file);
}

bool FileManager::table_create(const char* path,  size_t attr_num,attribute attr[ATTR_MAX_NUM])
{
	size_t size = 0;
	int max_key_size = 0;
	KEY_KIND key_type = INT_KEY;
	
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
	fclose(file);
	table t;
	memcpy(t.fpath, path, sizeof(path)+1);
	t.fpath[sizeof(path)+1] = '\0';
	for(int i=0;i<ATTR_MAX_NUM;i++)t.attr[i]=attr[i];
	t.key_kind = key_type;
	t.m_Depth = 1;
	t.offt_root = 2;
	t.key_use_block = 1;
	t.value_use_block = 0;
	for(int i=0;i<attr_num;i++)t.attr[i]=attr[i];
	t.attr_num = attr_num;
	t.max_key_size = size;

	//将表的信息写在文件的头部
	flushTable(t,t.fpath ,0);

	char block_graph[NUM_ALL_BLOCK];
	block_graph[0] = BLOCK_TLABE;
	block_graph[1] = BLOCK_GRAPH;
	for(int j=2;j<6;j++)block_graph[j] = BLOCK_FREE;
	for (int j = 6; j < NUM_ALL_BLOCK; j++)block_graph[j] = BLOCK_UNAVA;
	Index index(path,LOC_GRAPH,NUM_ALL_BLOCK,1);
	flush_BlockGraph(index, block_graph);
	//=======================空闲表写入成功======================
	cout<<"空闲表写入成功"<<endl;
	cout<<"表创建成功"<<endl;
	return true;
}
off_t FileManager::newBlock(const char* filename) {

	FILE* file = fopen(filename, "ab");
	char zero[DB_BLOCK_SIZE] = { 0 };
	fwrite(zero, 1, sizeof(zero), file);
	off_t now=ftell(file)/DB_BLOCK_SIZE;
	now--;
	fclose(file);
	char* graph=new char[NUM_ALL_BLOCK];
	//读获取位图
	get_BlockGraph(filename, graph);
	graph[now] = BLOCK_FREE;
	Index index(filename,LOC_GRAPH,NUM_ALL_BLOCK,1);
	//写位图
	flush_BlockGraph(index, graph);
	delete[] graph;
	return now;
	
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
bool FileManager::flushBlock(const char* filename, off_t offt, char type){
	char* block_graph = new char[NUM_ALL_BLOCK];
	get_BlockGraph(filename, block_graph);
	if(offt<0||offt>=NUM_ALL_BLOCK)return false;
	block_graph[offt] = type;
	Index index(filename, LOC_GRAPH, NUM_ALL_BLOCK, 1);
	flush_BlockGraph(index, block_graph);
	return true;
}

bool FileManager::flush_data(const char* filename,void* data[ATTR_MAX_NUM], attribute attr[ATTR_MAX_NUM],int attrnum,off_t offt){
	FILE* file = fopen(filename, "rb+");
	if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	for(int i=0;i<attrnum;i++){
		int data_int;
		long long data_long;
		char* data_string;
		if(attr[i].key_kind==INT_KEY){
			data_int = *(int*)data[i];
			fwrite(&data_int, 1, sizeof(int), file);
		}
		else if(attr[i].key_kind==LL_KEY){
		    data_long = *(long long*)data[i];
			fwrite(&data_long, 1, sizeof(long long), file);
		}
		else if(attr[i].key_kind==STRING_KEY){
			data_string = (char*)data[i];
			fwrite(data_string, 1, attr[i].max_size, file);
		}
		
	}
	fclose(file);
	return true;
}
void FileManager::get_data(const char* filename, void* data[ATTR_MAX_NUM], attribute attr[ATTR_MAX_NUM], int attrnum, off_t offt){

	FILE* file = fopen(filename, "rb+");
	if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	for(int i=0;i<attrnum;i++){
		if(attr[i].key_kind==INT_KEY){
			int* data_int = new int();
			fread(data_int, 1, sizeof(int), file);
			cout<<"数据读出来的值为"<<*data_int<<endl;
			data[i] = data_int;
		}
		else if(attr[i].key_kind==LL_KEY){
			long long* data_long = new long long();
			fread(&data_long, 1, sizeof(long long), file);
			data[i] = data_long;
		}
		else if(attr[i].key_kind==STRING_KEY){
			char* data_string=new char[1024];
			fread(data_string, 1, attr[i].max_size, file);
			data[i]= data_string;
		}
	}
	fclose(file);
}