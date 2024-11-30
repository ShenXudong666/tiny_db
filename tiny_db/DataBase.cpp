#include "DataBase.h"
#include "BPTree.h"
#include "rwdata.h"
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
    strcpy(attr[1].name,"age");
    attr[1].key_kind=INT_KEY;
    attr[1].max_size=sizeof(int);
    
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
    char* attribute_name[ATTR_MAX_NUM];
    KEY_KIND key_kind[ATTR_MAX_NUM];
    //插入数据

    off_t offt_data=bp->Insert(key);
    bp->Insert_Data(data, attribute_name, key_kind,offt_data);
    bp->flush_file();
    delete bp;

}
void DataBase::select(char* sql){
    const char* fpath="table.bin";
    BPlusTree* bp=new BPlusTree(fpath);
    void* key;
    off_t off_data=bp->Search(key);
    void* data[ATTR_MAX_NUM];
    bp->Get_Data(data, off_data);


    delete bp;

}