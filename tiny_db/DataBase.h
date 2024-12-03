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
        void Delete(char* sql);
        void update(char* tableName, char* set, char* condition);
        void select(char* sql);
        string extractTableName(const std::string& sql);
        vector<attribute> parseCreateTableStatement(const std::string& sql);
        vector<vector<string>> parseInsertStatement(const std::string& sql);
        vector<vector<string>> parseSelectStatement(const std::string& sql);

};