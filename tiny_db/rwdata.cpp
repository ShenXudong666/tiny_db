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
leaf_node FileManager::getLeafNode(const char* filename, off_t offt) {

	FILE* file = fopen(filename, "rb");
	if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	leaf_node node;
	fread(&node, 1, sizeof(leaf_node), file);
	fclose(file);

	return node;

}
bool FileManager::flushLeafNode(leaf_node node, const char* filename, off_t offt) {

	FILE* file = fopen(filename, "rb+");
	if (fseek(file, offt * DB_BLOCK_SIZE, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	fwrite(&node, 1, sizeof(leaf_node), file);
	fclose(file);
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
	flushLeafNode(root, t.fpath, 1);


	return false;
}
