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
    xiebiao(INT_KEY);
    BPlusTree* bp = new BPlusTree(fname);
	for(int i=1;i<=15;i++){
		bp->Insert((void*)new int(i));
	}
	bp->Delete(new int(12));
	bp->Delete(new int(3));
	
	// for(int i=1;i<=15;i++){
	// 	bool a=bp->Search((void*)new int(i),(char*)fname);
	// 	cout<<i<<" "<<a<<endl;
	// }
	// CInternalNode* c=new CInternalNode("table.bin",INT_KEY,sizeof(int),9);
	// cout<<c->GetCount()<<endl;
	//bp->Insert(new int(2));
	//bp->Delete(new int(2));
	// bp->Delete(new int(12));
	//bp->Delete(new int(12));
	//cout<<bp->Search(new int(3),(char *)fname)<<endl;
	bp->flush_file();
	delete bp;
	BPlusTree* bp1 = new BPlusTree(fname);
	//bp->Delete(new int(2));
	CInternalNode* root=(CInternalNode*)bp1->GetRoot();
	// for(int i=1;i<=15;i++){
	// 	bool a=bp1->Search((void*)new int(i),(char*)fname);
	// 	cout<<i<<" "<<a<<endl;
	// }
	
	cout<<"root的偏移量为："<<root->getPtSelf()<<endl;
	print_add_leaf(bp1);
	
	bp->flush_file();
	
	

	return 0;
}
