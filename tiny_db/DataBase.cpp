#include "DataBase.h"
#include "BPTree.h"
#include "rwdata.h"
#include <cstring>
#include <sys/types.h>
DataBase::DataBase(){

}
bool DataBase::createTable(char* sql){
    //解析sql语句，这一步后面再做
    attribute attr[ATTR_MAX_NUM];
    int attr_num=2;
    const char* key_attr="id";  
    strcpy(attr[0].name,"id");
    attr[0].key_kind=INT_KEY;
    attr[0].max_size=sizeof(int);
    strcpy(attr[1].name,"name");
    attr[1].key_kind=STRING_KEY;
    attr[1].max_size=100;
    
    const char* fpath="table.bin";
    //检查文件是否存在
    // if(FILE *file=fopen(fpath,"r")){
    //     fclose(file);
    //     cout<<"table already exist"<<endl;
    //     return false;
    // }
      

    FileManager::getInstance()->table_create(fpath, attr_num, attr,key_attr);
    return true;
}
void DataBase::insert(char* sql){
    const char* fpath="table.bin";
    //尝试解析sql语句，得到一些values
    
    BPlusTree* bp=new BPlusTree(fpath);

    void* data[ATTR_MAX_NUM];
    void* key;
    char attribute_name[ATTR_MAX_NUM][20];
    KEY_KIND key_kind[ATTR_MAX_NUM];
    //插入数据
    key=new int(1);
    data[0]=new int(1);
    data[1]=new char[100];
    strcpy((char*)data[1],"zhangsan");
    
    strcpy(attribute_name[0], "id");
    strcpy(attribute_name[1], "name");
    key_kind[0]=INT_KEY;
    key_kind[1]=STRING_KEY;

    off_t offt_data=bp->Insert(key);
    bp->Insert_Data(data, attribute_name, key_kind,offt_data);
    bp->flush_file();
    delete bp;

}
void DataBase::select(char* sql){
    const char* fpath="table.bin";
    BPlusTree* bp=new BPlusTree(fpath);
    void* key;
    key=new int(1);
    off_t off_data=bp->Search(key);
    if(off_data==INVALID){
        cout<<"没有找到该数据"<<endl;
        return;
    }
    void* data[ATTR_MAX_NUM];
    bp->Get_Data(data, off_data);
    bp->Print_Data(data);

    delete bp;

}
void DataBase::Delete(char* sql){
    const char* fpath="table.bin";
    BPlusTree* bp=new BPlusTree(fpath);
    void* key;
    key=new int(1);
    bp->Delete(key);
    bp->flush_file();
    delete bp;
}