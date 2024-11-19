#include"rwdata.h"
#include "BPTree.h"
#include <cstring>
#include<iostream>
#include<fstream>
using namespace std;
void xiebiao(KEY_KIND key) {
	const char* fname = "table.bin";
	if(key==INT_KEY)FileManager::getInstance()->table_create(fname, key, sizeof(int));
	else if(key==LL_KEY)FileManager::getInstance()->table_create(fname, key, sizeof(long long));
	else FileManager::getInstance()->table_create(fname, key, 100);
}
void print_add_leaf(BPlusTree* bp){
	for(int i=2;i<NUM_ALL_BLOCK;i++){
		if(bp->Block_GRAPH[i]==BLOCK_LEAF){
			CLeafNode* c=new CLeafNode("table.bin",INT_KEY,sizeof(int),i);
			c->print_data();
			cout<<endl;
		}
		else if(bp->Block_GRAPH[i]==BLOCK_INTER){
			CInternalNode* c=new CInternalNode("table.bin",INT_KEY,sizeof(int),i);
			c->print_data();
			cout<<endl;
		}
		
	}
}
int main(){
    const char* fname = "table.bin";
    //xiebiao(INT_KEY);
    BPlusTree* bp = new BPlusTree(fname);
	// for(int i=1;i<=40;i++){
	// 	bp->Insert((void*)new int(i));
	// }
	
	CInternalNode* root=(CInternalNode*)bp->GetRoot();
	cout<<root->getPtSelf()<<endl;
	print_add_leaf(bp);
	//cout<<bp->Search((void*)new int(13), (char*)fname)<<endl;
	//bp->PrintTree();
	// CInternalNode* root=(CInternalNode*)bp->GetRoot();
	// cout<<root->getPtSelf()<<endl;
	// cout<<root->getPointer(0)<<" ";
	// cout<<root->getPointer(1)<<" ";
	// cout<<root->getPointer(2)<<" ";
	// cout<<root->getPointer(3)<<endl;
	// CInternalNode* c = new CInternalNode(fname,INT_KEY,sizeof(int),4);
	// cout<<c->getPointer(0)<<endl;
	// cout<<c->getPointer(1)<<endl;
	// cout<<c->getPointer(2)<<endl;
	// cout<<c->getPointer(3)<<endl;
	// CInternalNode* c1 = new CInternalNode(fname,INT_KEY,sizeof(int),9);
	// cout<<c1->getPointer(0)<<endl;
	// cout<<c1->getPointer(1)<<endl;
	// cout<<c1->getPointer(2)<<endl;
	// cout<<c1->getPointer(3)<<endl;
	// cout<<bp->Search(new int(15), (char *)fname);

	// CLeafNode* c = new CLeafNode(fname,INT_KEY,sizeof(int),2);
	// c->print_data();
	// CLeafNode* c1 = new CLeafNode(fname,INT_KEY,sizeof(int),3);
	// c1->print_data();
	// CLeafNode * c2 = new CLeafNode(fname,INT_KEY,sizeof(int),5);
	// c2->print_data();


	bp->flush_file();
	
	

	return 0;
}
