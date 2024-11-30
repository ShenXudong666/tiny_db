#include "BPTree.h"
#include "rwdata.h"
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

};