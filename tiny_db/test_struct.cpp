//#include "rwdata.h"
//void xiejin(FileManager* fm,string fname) {
//	inter_node root;
//	root.offt_self = 1;
//	root.offt_pointers[0] = 2;
//	root.offt_pointers[1] = 3;
//	root.offt_pointers[2] = 4;
//	fm->flushInterNode(root, fname,root.offt_self);
//	leaf_node leaf1;
//	leaf1.offt_self = 2;
//	leaf1.value = 1000;
//	fm->flushLeafNode(leaf1, fname, leaf1.offt_self);
//	leaf_node leaf2;
//	leaf2.offt_self = 3;
//	leaf2.value = 2000;
//	fm->flushLeafNode(leaf2, fname, leaf2.offt_self);
//	leaf_node leaf3;
//	leaf3.offt_self = 4;
//	leaf3.value = 3000;
//	fm->flushLeafNode(leaf3, fname, leaf3.offt_self);
//	cout << "写入成功" << endl;
//}
//void duru(FileManager* fm, string fname) {
//	inter_node root = fm->getCInternalNode(fname, 1);
//	cout << "root的位置为:" << root.offt_self << endl;
//	leaf_node leaf1=fm->getLeafNode(fname, root.offt_pointers[0]);
//	leaf_node leaf2 = fm->getLeafNode(fname, root.offt_pointers[1]);
//	leaf_node leaf3 = fm->getLeafNode(fname, root.offt_pointers[2]);
//	cout << "leaf1:" << leaf1.value << endl;
//	cout << "leaf2:" << leaf2.value << endl;
//	cout << "leaf3:" << leaf3.value << endl;
//}
//int main4() {
//
//	FileManager* fm = FileManager().getInstance();
//	string fname = "example.bin";
//	/*fm->newBlock("example.bin");
//	fm->newBlock("example.bin");
//	fm->newBlock("example.bin");
//	fm->newBlock("example.bin");
//	fm->newBlock("example.bin");
//
//	xiejin(fm, "example.bin");*/
//
//	//xiejin(fm, "example.bin");
//	duru(fm, fname);
//
//	
//	return 0;
//	
//}