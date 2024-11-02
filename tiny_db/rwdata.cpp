#include "rwdata.h"
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
	if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
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
	if (fseek(file, index.offt_self, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	if (index.key_type == INT_KEY) {
		fwrite(&value, sizeof(int), MAXNUM_DATA, file);
	}
	else if (index.key_type == LL_KEY) {
		fwrite(&value, sizeof(long long), MAXNUM_DATA, file);
	}
	else {
		fwrite(&value, index.max_size, MAXNUM_DATA, file);
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
	if (index.key_type == INT_KEY) {
		fread(&key, sizeof(int), MAXNUM_KEY, file);
	}
	else if (index.key_type == LL_KEY) {
		fread(&key, sizeof(long long), MAXNUM_KEY, file);
	}
	else {
		fread(&key, index.max_size, MAXNUM_KEY, file);
	}
	fclose(file);
}

void FileManager::get_value(void* value[MAXNUM_DATA], Index index)
{
	FILE* file = fopen(index.fpath, "rb");
	if (fseek(file, index.offt_self, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	if (index.key_type == INT_KEY) {
		fread(&value, sizeof(int), MAXNUM_DATA, file);
	}
	else if (index.key_type == LL_KEY) {
		fread(&value, sizeof(long long), MAXNUM_DATA, file);
	}
	else {
		fread(&value, index.max_size, MAXNUM_DATA, file);
	}
	fclose(file);
}

void FileManager::flush_key(void* key[MAXNUM_KEY], Index index)
{
	FILE* file = fopen(index.fpath, "rb+");
	if (fseek(file, index.offt_self, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	if (index.key_type == INT_KEY) {
		fwrite(&key, sizeof(int), MAXNUM_DATA, file);
	}
	else if (index.key_type == LL_KEY) {
		fwrite(&key, sizeof(long long), MAXNUM_DATA, file);
	}
	else {
		fwrite(&key, index.max_size, MAXNUM_DATA, file);
	}
	fclose(file);
}

bool FileManager::table_create(const char* path, KEY_TYPE key_type, size_t max_key_size)
{
	size_t size;
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
		break;
	}
	table t;
	memcpy(t.fpath, path, sizeof(path));
	t.key_type = key_type;
	t.m_Depth = 1;
	t.offt_root = 1;
	t.key_use_block = 1;
	t.value_use_block = 0;
	//将表的信息写在文件的头部
	flushTable(t,t.fpath ,0);
	//将根的信息写在文件头部的后面一块
	leaf_node root;
	root.offt_self = 1;
	void* data[MAXNUM_DATA];
	Index index;
	memcpy(index.fpath, t.fpath, sizeof(t.fpath));
	index.max_size = t.max_key_size;
	index.key_type = t.key_type;
	flushLeafNode(root, index,data);


	return false;
}

