#include "DataBase.h"
#include "BPTree.h"
#include "rwdata.h"
#include <cstddef>
#include <cstring>
#include <sys/types.h>
#include <vector>
DataBase::DataBase(){

}
bool DataBase::createTable(const std::string& sql){
    string tablename=this->extractTableName(sql);
    tablename+=".bin";
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
    return true;
}
void DataBase::insert(const std::string& sql){
    string fpath=this->extractTableName(sql);
    fpath+=".bin";
    //尝试解析sql语句，得到一些values
    vector<vector<string>>values=this->parseInsertStatement(sql);
    for(int i=0;i<values.size();i++){
		for(int j=0;j<values[i].size();j++){
			cout<<values[i][j]<<" ";
		}
		cout<<endl;
	}
    BPlusTree* bp=new BPlusTree(fpath);    // void* data[ATTR_MAX_NUM];

    bp->Insert_Data(values);
    bp->flush_file();
    delete bp;

}
void DataBase::select(char* sql){
    string fpath=this->extractTableName(sql);
    fpath+=".bin";
    
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
string DataBase::extractTableName(const std::string& sql) {
    
    regex patternCreate(R"(\bCREATE TABLE \b)", std::regex_constants::icase);
    regex patternSelect(R"(\bFROM\s+(\w+))", std::regex_constants::icase);
    regex patternInsert(R"(\bINSERT INTO\s+(\w+))", std::regex_constants::icase);
    smatch matches;

    // 检查是否是创建表语句
    if (regex_search(sql, matches, patternCreate)) {
        size_t pos = matches.position(0) + matches.length(0);
        size_t end = sql.find('(', pos);
        if (end != string::npos) {
            return sql.substr(pos, end - pos);
        }
    }
    // 检查是否是查询语句，并提取表名
    else if (regex_search(sql, matches, patternSelect) && matches.size() > 1) {
        return matches[1];
    }
    else if (regex_search(sql, matches, patternInsert) && matches.size() > 1) {
        return matches[1];
    }
    return "";
}
vector<vector<string>> DataBase::parseSelectStatement(const std::string& sql){
    
}
vector<attribute> DataBase::parseCreateTableStatement(const std::string& sql) {
    vector<attribute> attr_arry;
    string columnsPart;
    string pattern = "CREATE TABLE .*?\\((.*)\\).*";
    regex regexPattern(pattern);
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
    getline(stream, word, ' '); // 读取 "VALUES"
    getline(stream, word, ' '); // 读取 "VALUES"
    getline(stream, word, '('); // 读取 "("
    valuesPart = word.substr(6); // 获取值部分，去掉最后的括号

    size_t pos1, pos2;
    string valueRow;
    while ((pos1 = valuesPart.find("(")) != std::string::npos) {
        pos2 = valuesPart.find(")", pos1);
        valueRow = valuesPart.substr(pos1 + 1, pos2 - pos1 - 1);
        valuesPart.erase(0, pos2 + 1);

        std::istringstream valueStream(valueRow);
        std::string value;
        std::vector<string> row;
        string column;
        rows.push_back(columns);
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