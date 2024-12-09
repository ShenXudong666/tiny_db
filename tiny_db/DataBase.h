#include "BPTree.h"
#include <sstream>
#include "rwdata.h"
#include <vector>
#include <regex>
#include <cstring>
#include "sqlparser.h"
using namespace SQL;
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
        void selectJoin(const std::string& sql);
        void Drop(const std::string& sql);
        void flush();
        void init();
        void printTableNames();
        database db;
    private:
        
};