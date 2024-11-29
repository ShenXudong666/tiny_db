#include"rwdata.h"
#include "BPTree.h"
#include<iostream>
using namespace std;
void xiebiao(KEY_KIND key) {
	const char* fname = "table.bin";
	if(key==INT_KEY)FileManager::getInstance()->table_create(fname, key, sizeof(int));
	else if(key==LL_KEY)FileManager::getInstance()->table_create(fname, key, sizeof(long long));
	else FileManager::getInstance()->table_create(fname, key, 100);
}
char* getString(int index){
	char* s=new char[1024];
	
	s[0]=(char)('a'+index-1);
	s[1]=(char)('a'+index-1);
	s[2]='\0';
	return s;
}
void print_add_leaf(BPlusTree* bp,KEY_KIND key){
	int size=0;
	if (key==INT_KEY)size=sizeof(int);
	else if(key==LL_KEY)size=sizeof(long long);
	else if(key==STRING_KEY) size=100;//string的最大大小先写死
	for(int i=2;i<NUM_ALL_BLOCK;i++){
		if(bp->Block_GRAPH[i]==BLOCK_LEAF){
			CLeafNode* c=new CLeafNode("table.bin",key,size,i);
			//cout<<"父亲为"<<c->getPtFather()<<endl;
			c->print_data();
			cout<<endl;
		}
		else if(bp->Block_GRAPH[i]==BLOCK_INTER){
			CInternalNode* c=new CInternalNode("table.bin",key,size,i);
			c->print_data();
			cout<<endl;
		}
		
	}
}
struct node{
    void* a;
	int b;
};
int main(){
    const char* fname = "table.bin";
    xiebiao(INT_KEY);
    BPlusTree* bp = new BPlusTree(fname);
	for(int i=1;i<=13;i++){
		bp->Insert((void*)new int(i));
	}
	
	//bp->Delete(new int(88));

	
	// for(int i=1;i<=4;i++){
	// 	bool a=bp->Search((void*)new int(i),(char*)fname);
	// 	cout<<i<<" "<<a<<endl;
	// }

	//cout<<bp->Search(new int(3),(char *)fname)<<endl;
	bp->flush_file();
	delete bp;
	BPlusTree* bp1 = new BPlusTree(fname);
	// CNode* root=bp1->GetRoot();
	// for(int i=0;i<MAXNUM_DATA;i++){
	// 	cout<<((CLeafNode*)root)->offt_data[i]<<endl;
	// }
	
	// cout<<"root的偏移量为："<<root->getPtSelf()<<endl;
	print_add_leaf(bp1,INT_KEY);
	
	
	
	

	return 0;
}
int test_key_ll(){
	const char* fname = "table.bin";
	// xiebiao(LL_KEY);
	// BPlusTree* bp = new BPlusTree(fname);
	// for(int i=1;i<=100;i++){
	// 	bp->Insert((void*)new long long(i));
	// }
	// bp->Delete((void*)new long long(88));
	// for(int i=1;i<=100;i++){
	// 	bool a=bp->Search((void*)new long long(i),(char*)fname);
	// 	cout<<i<<" "<<a<<endl;
	// }
	// bp->flush_file();
	// delete bp;
	BPlusTree* bp1 = new BPlusTree(fname);
	CInternalNode* root=(CInternalNode*)bp1->GetRoot();
	cout<<"root的偏移量为："<<root->getPtSelf()<<endl;
	print_add_leaf(bp1,LL_KEY);
	return 0;
}
int test_string(){
	//cout<<getString(1)<<endl;
	const char* fname = "table.bin";
	xiebiao(STRING_KEY);
	BPlusTree* bp = new BPlusTree(fname);
	for(int i=1;i<=26;i++){
		bp->Insert(getString(i));
	}
	bp->Delete(getString(5));
	
	
	// // bp->Delete((void*)new long long(88));
	// // for(int i=1;i<=100;i++){
	// // 	bool a=bp->Search((void*)new long long(i),(char*)fname);
	// // 	cout<<i<<" "<<a<<endl;
	// // }
	bp->flush_file();
	delete bp;
	BPlusTree* bp1 = new BPlusTree(fname);
	CInternalNode* root=(CInternalNode*)bp1->GetRoot();
	cout<<"root的偏移量为："<<root->getPtSelf()<<endl;
	print_add_leaf(bp1,STRING_KEY);



	return 0;
}

