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
	FILE* file = fopen("example.bin", "ab");
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
void outTry2() {
	
	ofstream outfile("example.bin", ios::out | ios::binary);
	btree_key bt;
	outfile.seekp(100,ios::beg);
	bt.child = 10;
	bt.value = 10;
	outfile.write((char*)&bt, sizeof(bt));
	cout << "写入成功" << endl;
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
	cout << "读取成功" << endl;

	infile.close();
	
}

int main3() {
	/*ifstream infile("example.txt", ios::in | ios::binary);
	streamoff offset = 10;
	infile.seekg(offset, ios::beg);*/

	/*ifstream infile("example.txt", ios::in | ios::binary);
	
	BPlusTree b;
	b.fpath = "I love everthing about Conor";
	infile.seekg(10L, ios::beg);
	infile();*/

	//outTry();
	//newBlock();
	//outTry(BLOCK_SIZE*0);
	inTry(BLOCK_SIZE * 1);
	//newBlock();
	/*outTry(BLOCK_SIZE * 0);
	inTry(BLOCK_SIZE * 0);*/

	//cout << sizeof(btree_key) << endl;
	return 0;


}