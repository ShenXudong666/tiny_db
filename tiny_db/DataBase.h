#include "BPTree.h"
#include <sstream>
#include "rwdata.h"
#include <vector>
#include <regex>
#include <cstring>

class DataBase {
    public:
        DataBase();
        ~DataBase();
        void run();
        bool createTable(const std::string& sql);
        void insert(const std::string& sql);
        void Delete(const std::string& sql);
        void Update(const std::string& sql);
        void select(const std::string& sql);
        void Drop(const std::string& sql);
        void flush();
        void init();
        void printTableNames();
        string extractTableName(const std::string& sql);
        string extractJoinTableName(const std::string& sql);
        vector<attribute> parseCreateTableStatement(const std::string& sql);
        vector<vector<string>> parseInsertStatement(const std::string& sql);
        vector<WhereCondition> parseSelectStatement(const std::string& sql,vector<string>&attributeNames,vector<LOGIC>&Logics);
        vector<WhereCondition> parseWhereClause(const std::string& whereClause);
        vector<WhereCondition> parseDeleteStatement(const std::string& sql);
        vector<WhereCondition> parseUpdateStatement(const std::string& sql,vector<WhereCondition>& set_attributes);
        vector<WhereCondition> parseSetStatement(const std::string& s);
        vector<string> splitCondition(const std::string& condition);
        database db;
    private:
        
};