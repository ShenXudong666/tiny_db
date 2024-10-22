#include"BPlusTree.h"
#include<iostream>
#include<fstream>

using namespace std;

typedef struct {
	off_t value;
	off_t child;
	unsigned char key[0];
	
} btree_key;

void outTry() {
	FILE* file = fopen("example.bin", "wb");
	btree_key bt;
	bt.child = 10;
	bt.value = 10;
	long offset = 20000;

	if (fseek(file, offset, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}
	
	fwrite(&bt, sizeof(btree_key), 1, file);
	fclose(file);
	cout << "写入成功" << endl;
}
void inTry() {
	FILE* file = fopen("example.bin", "rb");
	btree_key bt;

	long offset = 10000;
	if (fseek(file, offset, SEEK_SET) != 0) {
		perror("Failed to seek");
		fclose(file);
	}

	fread(&bt, sizeof(btree_key), 1, file);
	fclose(file);
	cout << "读取成功" << endl;
	cout << bt.child << " " << bt.value << endl;
}
int main() {
	/*ifstream infile("example.txt", ios::in | ios::binary);
	streamoff offset = 10;
	infile.seekg(offset, ios::beg);*/

	/*ifstream infile("example.txt", ios::in | ios::binary);
	
	BPlusTree b;
	b.fpath = "I love everthing about Conor";
	infile.seekg(10L, ios::beg);
	infile();*/

	outTry();
	inTry();
	


}