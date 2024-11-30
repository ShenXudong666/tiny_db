#include "BPTree.h"
#include "rwdata.h"
class DataBase {
    public:
        DataBase();
        ~DataBase();
        void run();
        bool createTable(char* sql);
        void insert(char* sql);
        void deleteRow(char* tableName, char* condition);
        void update(char* tableName, char* set, char* condition);
        void select(char* sql);

        //BPlusTree* bp;
        //void* data[ATTR_MAX_NUM];//这里代表的是一整条数据的多个属性值

};