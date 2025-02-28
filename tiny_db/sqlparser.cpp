#include "sqlparser.h"
using namespace SQL;
namespace SQL {
string extractTableName(const std::string& sql) {
    
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
string extractJoinTableName(const std::string& sql){
    regex patternJoin(R"(\bJOIN\s+(\w+))", std::regex_constants::icase);
    smatch matches;
    if (regex_search(sql, matches, patternJoin) && matches.size() > 1) {
        return matches[1];
    }
    return "";
}
vector<WhereCondition> parseSelectStatement(const std::string& sql,vector<string>&attributeNames,vector<LOGIC>&Logics){
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
    // for(int i=0;i<2;i++)
    //     getline(stream, word, ' ');

    while(getline(stream, word, ' ')){
        if(word=="Where"||word=="where"){
            break;
        }
    }

    if(word!="Where"&&word!="where"){
        return vector<WhereCondition>();
    }
    
    
    //把where读取完
    string conditionPart;
    getline(stream,conditionPart);
    vector<WhereCondition>w=parseWhereClause(conditionPart);

    istringstream conditionStream(conditionPart);
    while(getline(conditionStream, word, ' ')){
        if(word=="AND"||word=="and")Logics.push_back(AND_LOGIC);
        if(word=="OR"||word=="or")Logics.push_back(OR_LOGIC);
    }
    
    return w; 
    
}
vector<WhereCondition> parseWhereClause(const std::string& whereClause){
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
        
        vector<string> parts = splitCondition(result[i]);
        WhereCondition wc(parts[0], parts[1], parts[2]);
        //for(int i=0;i<3;i++)cout<<parts[i]<<endl;
        conditions.push_back(wc);
        //使用>,<,=,!=,>=,<=来匹配

    }
    return conditions;
}
vector<WhereCondition> parseSetStatement(const std::string& s){

    vector<WhereCondition>w;
    vector<string>result;
    string word;

    istringstream iss(s);
    while(getline(iss,word,',')){
        vector<string> parts = splitCondition(word);
        WhereCondition wc(parts[0], parts[1], parts[2]);
        w.push_back(wc);
    }

    return w;
}
vector<string> splitCondition(const std::string& condition){
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
vector<attribute> parseCreateTableStatement(const std::string& sql) {
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
                string constraintName;
                istringstream constraintStream(columeConstraint);
                getline(constraintStream, constraintName, ' ');
                if(constraintName=="ref"){
                    attribute column(columnName, key_kind, maxLength,columeConstraint);
                    attr_arry.push_back(column);
                    continue;
                }
                    
                    
            }
                
                attribute column(columnName, key_kind, maxLength);
                attr_arry.push_back(column);
            }
        }
    
    return attr_arry;
}
vector<vector<string>> parseInsertStatement(const std::string& sql){
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
vector<WhereCondition> parseDeleteStatement(const std::string& sql, vector<LOGIC>& logic){
    vector<WhereCondition>result;
    string word;
    string wherePart;
    
    istringstream stream(sql);
    for(int i=0;i<4;i++)getline(stream, word, ' ');

    getline(stream,wherePart);
    result=parseWhereClause(wherePart);
    istringstream logicStream(wherePart);
    string logicWord;
    while (std::getline(logicStream, logicWord, ' ')) {
        if(logicWord=="AND"||logicWord=="and"){
            logic.push_back(AND_LOGIC);
        }
        else if(logicWord=="OR"||logicWord=="or"){
            logic.push_back(OR_LOGIC);
        }
    }
    return result;
}
vector<WhereCondition> parseUpdateStatement(const std::string& sql,vector<WhereCondition>& set_attributes){
    vector<WhereCondition>result;
    string word;
    string wherePart;
    string SetPart;

    istringstream stream(sql);
    for(int i=0;i<3;i++)getline(stream, word, ' ');

    getline(stream,SetPart,' ');
    set_attributes=parseSetStatement(SetPart);
    

    getline(stream,word,' ');
    if(word!="WHERE"&&word!="where"){
        cout<<"语法错误"<<endl;
        return vector<WhereCondition>();
    }

    getline(stream,wherePart);
    result=parseWhereClause(wherePart);
    return result;
}
}
   

