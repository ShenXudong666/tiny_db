#include "DataBase.h"
#include "BPTree.h"
#include "rwdata.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ostream>
#include <string>
#include <sys/types.h>
#include <vector>
DataBase::DataBase(){

}
void DataBase::run(){
    this->init();
    while(true){
        string sql;
        cout<<this->db.db_name<<"->";
        getline(cin,sql);
        if(sql=="exit"||sql=="quit"||sql=="\\q"){
            break;
        }
        //读取第一个空格前的词，判断是create还是insert还是select
        int pos=sql.find_first_of(" ");
        string cmd=sql.substr(0,pos);
        if(cmd=="create"||cmd=="CREATE"){
            this->createTable(sql);
        }
        else if(cmd=="insert"||cmd=="INSERT"){
            this->insert(sql);
        }
        else if(cmd=="select"||cmd=="SELECT"){
            
            this->select(sql);
        }
        else if(cmd=="delete"||cmd=="Delete"){
            this->Delete(sql);
        }
        else if(cmd=="update"||cmd=="UPDATE"){
            this->Update(sql);
        }
        else if(cmd=="drop"||cmd=="DROP"){
            this->Drop(sql);
        }
        else if(cmd=="show"||cmd=="SHOW"){
            this->printTableNames();
        }
        else{
            cout<<endl;
        }
    }

    FileManager::getInstance()->flushDatabase("database.bin",this->db);
}
bool DataBase::createTable(const std::string& sql){
    
    string tablename=SQL::extractTableName(sql);
    tablename+=".bin";
    //检查文件是否存在
    // if(FILE *file=fopen(tablename.c_str(),"r")){
    //     fclose(file);
    //     cout<<"表已经存在，创建表失败"<<endl;
    //     return false;
    // }
    vector<attribute> ture_attr=SQL::parseCreateTableStatement(sql);
    if(ture_attr.size()==0){
        cout<<"创建表失败"<<endl;
        return false;
    }

    //解析sql语句，这一步后面再做
    attribute attr[ATTR_MAX_NUM];
    int attr_num=ture_attr.size();
    for(int i=0;i<attr_num;i++){
        attr[i]=ture_attr[i];
    }
    
    //检查文件是否存在
    // if(FILE *file=fopen(fpath,"r")){
    //     fclose(file);
    //     cout<<"table already exist"<<endl;
    //     return false;
    // }
    
    FileManager::getInstance()->table_create(tablename.c_str(), attr_num, attr);
    tablename.erase(tablename.size() - 4);
    for(int i=0;i<this->db.table_num;i++){
        if(strcmp(db.tables[i],tablename.c_str())==0){
            return true;
        }
    }

    strcpy(db.tables[db.table_num], tablename.c_str());
    db.table_num++;
    return true;
}
void DataBase::insert(const std::string& sql){
    string fpath=SQL::extractTableName(sql);
    fpath+=".bin";
    //表不存在则插入失败
    FILE *file=fopen(fpath.c_str(),"r");
    if(!file){
        cout<<"表不存在，插入失败"<<endl;
        return;
    }

    
    vector<vector<string>>values=SQL::parseInsertStatement(sql);

    BPlusTree* bp=new BPlusTree(fpath);    // void* data[ATTR_MAX_NUM];

    bp->Insert_Data(values);
    bp->flush_file();
    delete bp;

}
void DataBase::select(const std::string& sql){

    string join_table=SQL::extractJoinTableName(sql);
    if(join_table!=""){
        this->selectJoin(sql);
        return;
    }

    string fpath=SQL::extractTableName(sql);
    fpath+=".bin";
    FILE *file=fopen(fpath.c_str(),"r");
    if(!file){
        cout<<"表不存在，查询失败"<<endl;
        return;
    }
    fclose(file);
    vector<string>attributeNames;
    vector<LOGIC>Logics;
    vector<WhereCondition>whereConditions=SQL::parseSelectStatement(sql,attributeNames,Logics);
    BPlusTree* bp=new BPlusTree(fpath);
    bp->Select_Data(attributeNames, Logics, whereConditions);
    

    delete bp;

}
void DataBase::Delete(const std::string& sql){
    string fpath=SQL::extractTableName(sql);
    fpath+=".bin";
    FILE *file=fopen(fpath.c_str(),"r");
    if(!file){
        cout<<"表不存在，删除数据失败"<<endl;
        return;
    }
    fclose(file);
    vector<LOGIC>Logics;
    vector<WhereCondition>whereConditions=SQL::parseDeleteStatement(sql,Logics);
    BPlusTree* bp=new BPlusTree(fpath);
    if(bp->Delete_Data(whereConditions,Logics)){
        cout<<"删除成功"<<endl;
    }
    else{
        cout<<"未找到符合条件的数据，删除失败"<<endl;
    }
    bp->flush_file();
    delete bp;
}
void DataBase::Update(const std::string& sql){
    string fpath=SQL::extractTableName(sql);
    fpath+=".bin";
    FILE *file=fopen(fpath.c_str(),"r");
    if(!file){
        cout<<"表不存在，更新失败"<<endl;
        return;
    }
    fclose(file);
    vector<WhereCondition>setAttributes;
    vector<WhereCondition>whereConditions=SQL::parseUpdateStatement(sql,setAttributes);
    BPlusTree* bp=new BPlusTree(fpath);
    bp->Update_Data(whereConditions, setAttributes);
    bp->flush_file();
}
void DataBase::selectJoin(const std::string& sql){
    string fpath1=SQL::extractTableName(sql);
    string fpath2=SQL::extractJoinTableName(sql);
    vector<string>attributeNames;
    vector<LOGIC>Logics;
    vector<WhereCondition>whereConditions=SQL::parseSelectStatement(sql,attributeNames,Logics);

    fpath1+=".bin";
    BPlusTree* bp1=new BPlusTree(fpath1,fpath2);
    bp1->Select_Data_Join(attributeNames, Logics, whereConditions);
    bp1->flush_file();
    delete bp1;

}
void DataBase::Drop(const std::string& sql){
    string fpath=SQL::extractTableName(sql);
    fpath+=".bin";
    

    //检查文件是否存在
    FILE *file=fopen(fpath.c_str(),"r");
    if(!file){
        cout<<"表不存在，删除失败"<<endl;
        return;
    }
    fclose(file);
    
    int res=remove(fpath.c_str());
    if(res!=0){
        cout<<"删除失败"<<endl;
        return;
    }
    cout<<"删除成功"<<endl;
    fpath.erase(fpath.size() - 4);
    int i=0;
    for(;i<db.table_num;i++){
        if(strcmp(this->db.tables[i],fpath.c_str())==0){
            strcpy(this->db.tables[i],"");
            break;
        }
    }
    for(int j=i;j<db.table_num-1;j++){
        strcpy(this->db.tables[j],this->db.tables[j+1]);
    }
    db.table_num--;
}


void DataBase::init(){
    size_t exist=FileManager::getInstance()->getFileSize("database.bin");
    if(exist==-1){
        
        this->db.table_num=0;
        string user_name;
        string password;
        cout<<"请输入用户名"<<endl;
        cin>>user_name;
        cout<<"请输入密码"<<endl;
        cin>>password;
        cout<<"请输入数据库名"<<endl;
        string db_name;
        cin>>db_name;
        strcpy(db.user_name,user_name.c_str());
        strcpy(db.password,password.c_str());
        strcpy(db.db_name,db_name.c_str());
        FileManager::getInstance()->flushDatabase("database.bin",db);
        return;
    }
    
    this->db=FileManager::getInstance()->getDatabase("database.bin");
    
    // while(true){
    //     string usr_name;
    //     string password;
    //     cout<<"请输入用户名"<<endl;
    //     cin>>usr_name;
    //     cout<<"请输入密码"<<endl;
    //     cin>>password;
    //     if(strcmp(this->db.user_name,usr_name.c_str())==0&&strcmp(this->db.password,password.c_str())==0){
    //         cout<<"登录成功!!!欧耶"<<endl;
    //         break;
    //     }
    //     else{
    //         cout<<"用户名或密码错误"<<endl;
    //     }
    // }
}
void DataBase::printTableNames(){
    string tail=".bin";
    vector<string>v;
    v.push_back("*");
    if(db.table_num==0){
        cout<<"当前数据库为空"<<endl;
        return;
    }
    for(int i=0;i<this->db.table_num;i++){
        
        string t(this->db.tables[i]);
        cout<<t<<":"<<endl;
        t+=".bin";
        BPlusTree* bp=new BPlusTree(t);
        bp->Print_Header(v);
        delete bp;
    }
}
void DataBase::flush(){
    FileManager::getInstance()->flushDatabase("database.bin",this->db);
}