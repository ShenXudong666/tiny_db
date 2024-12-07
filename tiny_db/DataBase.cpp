#include "DataBase.h"
#include "BPTree.h"
#include "rwdata.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ostream>
#include <regex>
#include <sstream>
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
    string tablename=this->extractTableName(sql);
    tablename+=".bin";
    //检查文件是否存在
    // if(FILE *file=fopen(tablename.c_str(),"r")){
    //     fclose(file);
    //     cout<<"表已经存在，创建表失败"<<endl;
    //     return false;
    // }
    vector<attribute> ture_attr=this->parseCreateTableStatement(sql);
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
    string fpath=this->extractTableName(sql);
    fpath+=".bin";
    //表不存在则插入失败
    FILE *file=fopen(fpath.c_str(),"r");
    if(!file){
        cout<<"表不存在，插入失败"<<endl;
        return;
    }

    
    vector<vector<string>>values=this->parseInsertStatement(sql);

    BPlusTree* bp=new BPlusTree(fpath);    // void* data[ATTR_MAX_NUM];

    bp->Insert_Data(values);
    bp->flush_file();
    delete bp;

}
void DataBase::select(const std::string& sql){
    string fpath=this->extractTableName(sql);
    fpath+=".bin";
    FILE *file=fopen(fpath.c_str(),"r");
    if(!file){
        cout<<"表不存在，查询失败"<<endl;
        return;
    }
    fclose(file);
    vector<string>attributeNames;
    vector<LOGIC>Logics;
    vector<WhereCondition>whereConditions=this->parseSelectStatement(sql,attributeNames,Logics);
    BPlusTree* bp=new BPlusTree(fpath);
    bp->Select_Data(attributeNames, Logics, whereConditions);
    

    delete bp;

}
void DataBase::Delete(const std::string& sql){
    string fpath=this->extractTableName(sql);
    fpath+=".bin";
    FILE *file=fopen(fpath.c_str(),"r");
    if(!file){
        cout<<"表不存在，删除数据失败"<<endl;
        return;
    }
    fclose(file);
    vector<WhereCondition>whereConditions=this->parseDeleteStatement(sql);
    BPlusTree* bp=new BPlusTree(fpath);
    if(bp->Delete_Data(whereConditions)){
        cout<<"删除成功"<<endl;
    }
    else{
        cout<<"未找到符合条件的数据，删除失败"<<endl;
    }
    bp->flush_file();
    delete bp;
}
void DataBase::Update(const std::string& sql){
    string fpath=this->extractTableName(sql);
    fpath+=".bin";
    FILE *file=fopen(fpath.c_str(),"r");
    if(!file){
        cout<<"表不存在，更新失败"<<endl;
        return;
    }
    fclose(file);
    vector<WhereCondition>setAttributes;
    vector<WhereCondition>whereConditions=this->parseUpdateStatement(sql,setAttributes);
    BPlusTree* bp=new BPlusTree(fpath);
    bp->Update_Data(whereConditions, setAttributes);
    bp->flush_file();
}
void DataBase::Drop(const std::string& sql){
    string fpath=this->extractTableName(sql);
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
    for(i;i<db.table_num;i++){
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
string DataBase::extractTableName(const std::string& sql) {
    
    regex patternCreate(R"(\bCREATE TABLE \b)", std::regex_constants::icase);
    regex patternSelect(R"(\bFROM\s+(\w+))", std::regex_constants::icase);
    regex patternInsert(R"(\bINSERT INTO\s+(\w+))", std::regex_constants::icase);
    regex patternUpdate(R"(\Update\s+(\w+))", std::regex_constants::icase);
    regex patternDrop(R"(\bDROP TABLE\s+(\w+))", std::regex_constants::icase);
    smatch matches;

    // 检查是否是创建表语句
    if (regex_search(sql, matches, patternCreate)) {
        size_t pos = matches.position(0) + matches.length(0);
        size_t end = sql.find('(', pos);
        if (end != string::npos) {
            return sql.substr(pos, end - pos);
        }
    }
    else if (regex_search(sql, matches, patternDrop) && matches.size() > 1) {
        return matches[1];
    }
    // 检查是否是查询语句，并提取表名
    else if (regex_search(sql, matches, patternSelect) && matches.size() > 1) {
        return matches[1];
    }
  
    else if (regex_search(sql, matches, patternInsert) && matches.size() > 1) {
        return matches[1];
    }
    else if (regex_search(sql, matches, patternUpdate) && matches.size() > 1) {
        return matches[1];
    }
  
    return "";
}
vector<WhereCondition> DataBase::parseSelectStatement(const std::string& sql,vector<string>&attributeNames,vector<LOGIC>&Logics){
    istringstream stream(sql);
    string word;
    getline(stream,word,' ');
    getline(stream,word,' ');
    string attributePart=word;
    if(attributePart[0]=='*'){
        attributeNames.push_back("*");
    }
    else{
        istringstream attributeStream(attributePart);
        string attributeName;
        while (getline(attributeStream, attributeName, ',')) {
            attributeNames.push_back(attributeName);
        }
    }
    for(int i=0;i<2;i++)
        getline(stream, word, ' ');

    if(!getline(stream,word,' ')){
        //cout<<"无条件"<<endl;
        return vector<WhereCondition>();
    }
    if(word==""){
        return vector<WhereCondition>();
    }
    if(word!="WHERE"&&word!="where"){
        //cout<<"语法错误"<<endl;
        return vector<WhereCondition>();
    }
    //把where读取完
    string conditionPart;
    getline(stream,conditionPart);
    vector<WhereCondition>w=this->parseWhereClause(conditionPart);

    istringstream conditionStream(conditionPart);
    while(getline(conditionStream, word, ' ')){
        if(word=="AND"||word=="and")Logics.push_back(AND_LOGIC);
        if(word=="OR"||word=="or")Logics.push_back(OR_LOGIC);
    }
    
    return w; 
    
}
vector<WhereCondition> DataBase::parseWhereClause(const std::string& whereClause){
    vector<WhereCondition> conditions;
    vector<string>result;
    istringstream iss(whereClause);
    string token;
    while (getline(iss, token, ' ')) {
        //去掉token末尾的分号
        if(token.back()==';')token.pop_back();
        if(token!="AND"&&token!="OR"&&token!="and"&&token!="or")result.push_back(token);
    }
    for(int i=0;i<result.size();i++){
        
        vector<string> parts = this->splitCondition(result[i]);
        WhereCondition wc(parts[0], parts[1], parts[2]);
        //for(int i=0;i<3;i++)cout<<parts[i]<<endl;
        conditions.push_back(wc);
        //使用>,<,=,!=,>=,<=来匹配

    }
    return conditions;
}
vector<WhereCondition> DataBase::parseSetStatement(const std::string& s){

    vector<WhereCondition>w;
    vector<string>result;
    string word;

    istringstream iss(s);
    while(getline(iss,word,',')){
        vector<string> parts = this->splitCondition(word);
        WhereCondition wc(parts[0], parts[1], parts[2]);
        w.push_back(wc);
    }

    return w;
}
vector<string> DataBase::splitCondition(const std::string& condition){
    vector<string> parts(3); // 初始化为3个元素，分别存储属性名、比较符号和值
    size_t pos = 0; // 用于查找比较符号的位置

    // 查找第一个非字母数字字符的位置，这通常是比较符号
    for (pos = 0; pos < condition.length(); ++pos) {
        if (!isalnum(condition[pos]) && condition[pos] != '_') {
            break;
        }
    }

    // 提取属性名
    parts[0] = condition.substr(0, pos);

    // 查找比较符号和值
    size_t start = pos;
    size_t end = condition.find_first_not_of("<>=!", pos + 1); // 找到值的开始位置
    if (end == std::string::npos) {
        // 如果没有找到值，那么整个字符串就是属性名和比较符号
        parts[1] = condition.substr(pos);
        parts[2] = ""; // 没有值
    } else {
        // 提取比较符号
        parts[1] = condition.substr(pos, end - pos);
        // 提取值
        parts[2] = condition.substr(end);
    }

    return parts;
}
vector<attribute> DataBase::parseCreateTableStatement(const std::string& sql) {
    vector<attribute> attr_arry;
    string columnsPart;
    string pattern = "create table .*?\\((.*)\\).*"; // 修改这里
    regex regexPattern(pattern, std::regex_constants::icase);
    smatch match;

    if (regex_search(sql, match, regexPattern)) {
        columnsPart = match[1];
    }

    istringstream columnStream(columnsPart);
    string columnDef;
    int keynum=0;
    while (getline(columnStream, columnDef, ',')) {
        istringstream columnDetails(columnDef);
        string columnName, columnType,columeConstraint;
        int maxLength = 0;
        if (std::getline(columnDetails, columnName, ' ')) {
            if (std::getline(columnDetails, columnType, ' ')) {
                 // 尝试解析长度
                string lengthPart = columnType;
                size_t rightPos = lengthPart.find(')');
                if (rightPos != std::string::npos) {
                    size_t leftPos=lengthPart.find('(');
                    lengthPart = lengthPart.substr(leftPos + 1, rightPos - leftPos - 1);
                    maxLength = std::stoi(lengthPart);
                    columnType = columnType.substr(0,leftPos); // 更新类型，去掉长度部分
                }
            }

                KEY_KIND key_kind;
                if(columnType=="INT"){
                    key_kind=INT_KEY;
                    maxLength=4;
                }else if(columnType=="LONG LONG"){
                    key_kind=LL_KEY;
                    maxLength=8;
                }
                else key_kind=STRING_KEY;

            if (std::getline(columnDetails, columeConstraint)) {

                // 解析约束条件
                if(_stricmp(columeConstraint.c_str(), "PRIMARY KEY") == 0){
                    if(keynum>1){
                        cout<<"主键只能有一个"<<endl;
                        return vector<attribute>();
                    }
                    attribute column(columnName, key_kind, maxLength,columeConstraint);
                    attr_arry.push_back(column);
                    keynum++;
                    continue;
                }
                    
            }
                
                attribute column(columnName, key_kind, maxLength);
                attr_arry.push_back(column);
            }
        }
    
    return attr_arry;
}
vector<vector<string>> DataBase::parseInsertStatement(const std::string& sql){
    vector<vector<string>> rows;
    vector<string> columns;
    istringstream stream(sql);
    string word;
    string valuesPart;
    // 跳过 "INSERT INTO table_name" 部分
    for(int i=0;i<4;i++)
        getline(stream, word, ' ');
    
    if (word[0] == '(') {
        std::string columnsPart = word.substr(1, word.size() - 2); // 去掉括号
        std::istringstream columnsStream(columnsPart);
        std::string columnName;
        while (std::getline(columnsStream, columnName, ',')) {
            columns.push_back(columnName);
        }
    }
    else if(word[1]=='('){
        getline(stream, word, ' ');
        std::string columnsPart = word.substr(1, word.size() - 2); // 去掉括号
        std::istringstream columnsStream(columnsPart);
        std::string columnName;
        while (std::getline(columnsStream, columnName, ',')) {
            columns.push_back(columnName);
        }
    }
    else{
        cout<<"语法格式错误"<<endl;
        return vector<vector<string>>();
    }
    getline(stream, word, ' '); // 读取 "VALUES"
    getline(stream, word, ' '); // 读取 "VALUES"
    getline(stream, word, '('); // 读取 "("
    valuesPart = word.substr(6); // 获取值部分，去掉最后的括号

    size_t pos1, pos2;
    string valueRow;
    rows.push_back(columns);
    while ((pos1 = valuesPart.find("(")) != std::string::npos) {
        pos2 = valuesPart.find(")", pos1);
        valueRow = valuesPart.substr(pos1 + 1, pos2 - pos1 - 1);
        valuesPart.erase(0, pos2 + 1);

        std::istringstream valueStream(valueRow);
        std::string value;
        std::vector<string> row;
        string column;
        
        for (int i = 0; i < columns.size(); ++i) {
            std::getline(valueStream, value, ',');
            column = value;
            size_t found = column.find('\'');
            if (found != std::string::npos) {
                column.erase(found, 1); // 去掉第一个单引号
                found = column.find('\'');
                if (found != std::string::npos) {
                    column.erase(found, 1); // 去掉第二个单引号
                }
            }
            row.push_back(column);
        }
        rows.push_back(row);
    }

    return rows;

}
vector<WhereCondition> DataBase::parseDeleteStatement(const std::string& sql){
    vector<WhereCondition>result;
    string word;
    string wherePart;
    
    istringstream stream(sql);
    for(int i=0;i<4;i++)getline(stream, word, ' ');

    getline(stream,wherePart);
    result=parseWhereClause(wherePart);
    return result;
}
vector<WhereCondition> DataBase::parseUpdateStatement(const std::string& sql,vector<WhereCondition>& set_attributes){
    vector<WhereCondition>result;
    string word;
    string wherePart;
    string SetPart;

    istringstream stream(sql);
    for(int i=0;i<3;i++)getline(stream, word, ' ');

    getline(stream,SetPart,' ');
    set_attributes=this->parseSetStatement(SetPart);
    

    getline(stream,word,' ');
    if(word!="WHERE"&&word!="where"){
        cout<<"语法错误"<<endl;
        return vector<WhereCondition>();
    }

    getline(stream,wherePart);
    result=parseWhereClause(wherePart);
    return result;
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