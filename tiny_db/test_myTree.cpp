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
int main(){
    const char* fname = "table.bin";
    xiebiao(INT_KEY);
    BPlusTree* bp = new BPlusTree(fname);
	for(int i=0;i<10;i++){
		bp->Insert((void*)(new int(i)));
	}
	bp->Insert((void*)(new int(10)));
	bp->Insert((void*)(new int(20)));
	bp->Insert((void*)(new int(30)));
	bp->Insert((void*)(new int(40)));
	bp->Insert((void*)new int(50));
	bp->flush_file();
	// bp->Insert((void*)(new int(10)));
	// bp->Insert((void*)(new int(20)));
	// bp->Insert((void*)(new int(30)));
	// bp->Insert((void*)(new int(40)));
	// bp->Insert((void*)new int(50));
	//bp->GetRoot();
	//bp->Insert((void*)(new int(30)));
	//bp->Insert((void*)(new int(40)));
	//cout<<"根节点的值有 "<<*(int*)(bp->GetRoot()->GetElement(2));
	
	// cout<<"bp的根的有效值为:"<<endl;
	// cout<<bp->GetRoot()->GetCount();
	
	//if(bp->m_Root==NULL)cout<<"NULL"<<endl;
	
    // bp->Insert((void*)new int(1));
	// cout<<bp->m_Root->GetType()<<endl;

	return 0;
}
