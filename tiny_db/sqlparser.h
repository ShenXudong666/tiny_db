#include "rwdata.h"
#include <sstream>
#include <vector>
#include <regex>
#include <cstring>

namespace SQL {
    string extractTableName(const std::string& sql);
    string extractJoinTableName(const std::string& sql);
    vector<attribute> parseCreateTableStatement(const std::string& sql);
    vector<vector<string>> parseInsertStatement(const std::string& sql);
    vector<WhereCondition> parseSelectStatement(const std::string& sql,vector<string>&attributeNames,vector<LOGIC>&Logics);
    vector<WhereCondition> parseWhereClause(const std::string& whereClause);
    vector<WhereCondition> parseDeleteStatement(const std::string& sql,vector<LOGIC>& logic);
    vector<WhereCondition> parseUpdateStatement(const std::string& sql,vector<WhereCondition>& set_attributes);
    vector<WhereCondition> parseSetStatement(const std::string& s);
    vector<string> splitCondition(const std::string& condition);
}