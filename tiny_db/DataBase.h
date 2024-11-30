#include "BPTree.h"
#include <sstream>
#include "rwdata.h"
#include <vector>
#include <regex>
class DataBase {
    public:
        DataBase();
        ~DataBase();
        void run();
        bool createTable(char* sql);
        void insert(char* sql);
        void Delete(char* sql);
        void update(char* tableName, char* set, char* condition);
        void select(char* sql);
        string extractTableName(char* sql);
        vector<attribute> parseCreateTableStatement(const std::string& sql);

};