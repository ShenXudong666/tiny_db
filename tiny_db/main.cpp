#include "BPTree.h"
#include"rwdata.h"
#include "DataBase.h"
#include<iostream>
#include <string>
#include <vector>

using namespace std;
void test_table_op() {
	//const char* fname = "table.bin";
	DataBase* db=new DataBase();
	
	// cout<<"创建表============"<<endl;
	// string sql_create="CREATE TABLE table(id INT PRIMARY KEY,name varchar(100),age INT);";
	// db->createTable(sql_create);
	
	// vector<string>t1;
	// vector<WhereCondition>t2;
	// vector<LOGIC>logic;
	// t2=db->parseSelectStatement(sql_select,t1,logic);
	// cout<<0;
	// string sql_insert1="INSERT INTO table (id,name,age) VALUES(1,'zhangsan',20);";
	// db->insert(sql_insert1);
	// string sql_insert2="INSERT INTO table (id,name,age) VALUES(2,'lisi',21);";
	// db->insert(sql_insert2);
	// string sql_insert3="INSERT INTO table (id,name,age) VALUES(3,'wangwu',22);";
	// db->insert(sql_insert3);
	// string sql_insert4="INSERT INTO table (id,name,age) VALUES(4,'zhaoliu',23);";
	// db->insert(sql_insert4);
	// string sql_insert5="INSERT INTO table (id,name,age) VALUES(5,'qianshi',24);";
	// db->insert(sql_insert5);
	// string sql_insert6="INSERT INTO table (id,name,age) VALUES(6,'sunqi',25);";
	// db->insert(sql_insert6);
	// string sql_insert7="INSERT INTO table (id,name,age) VALUES(7,'zhouba',26);";
	// db->insert(sql_insert7);
	// string sql_insert8="INSERT INTO table (id,name,age) VALUES(8,'wujiu',27);";
	// db->insert(sql_insert8);
	// string sql_insert9="INSERT INTO table (id,name,age) VALUES(9,'zhengshi',28);";
	// db->insert(sql_insert9);

	string sql_select1="SELECT * FROM table;";
	db->select(sql_select1);
	// string sql_select2="SELECT * FROM table;";
	// db->select(sql_select2);
	// string sql_delete="DELETE FROM table WHERE id=8;";
	// db->Delete(sql_delete);
	// db->select(sql_select2);
	
	

	// string sql_update="UPDATE table SET age=30 WHERE id=7;";
	// db->Update(sql_update);
	// vector<vector<string>>s=db->parseInsertStatement(sql_insert);
	// for(int i=0;i<s.size();i++){
	// 	for(int j=0;j<s[i].size();j++){
	// 		cout<<s[i][j]<<" ";
	// 	}
	// 	cout<<endl;
	// }
	

}
void test2(){
	DataBase* database=new DataBase();
	//database->init();
	database->run();
	string sql_select="SELECT * FROM employee;";
	//database->select(sql_select);
	string sql_create_emp="CREATE TABLE employee(e_id INT PRIMARY KEY,e_name varchar(100),age INT,d_name varchar(100) ref department(d_name));";
	string sql_create_dept="CREATE TABLE department(d_id INT PRIMARY KEY,d_name varchar(100),salary INT);";
	string sql_insert_emp1="INSERT INTO employee (e_id,e_name,age,d_name) VALUES(1,'zhangsan',20,'sales'),(2,'lisi',21,'sales'),(3,'wangwu',22,'sales'),(4,'zhaoliu',23,'IT'),(5,'qianshi',24,'IT'),(6,'sunqi',25,'IT'),(7,'zhouba',26,'HR'),(8,'wujiu',27,'HR'),(9,'zhengshi',28,'HR');";
	
	string sql_insert_dept1="INSERT INTO department (d_id,d_name,salary) VALUES(1,'sales',1000),(2,'IT',2000),(3,'HR',3000);";
	string sql_delete="DELETE FROM employee WHERE age>22 and d_name='HR';";
	//database->Delete(sql_delete);
	//database->select("select * from employee;");
	//database->Delete(sql_delete);
	// database->createTable(sql_create_emp);
	// database->createTable(sql_create_dept);
	// database->insert(sql_insert_emp1);
	// database->insert(sql_insert_dept1);
	// database->printTableNames();

	// string sql_select1="SELECT * FROM employee;";
	// database->select(sql_select1);
	// string sql_selct2="SELECT * FROM department;";
	// database->select(sql_selct2);
	string sql_select3="SELECT e_name,age,d_name FROM employee JOIN department where age>20 AND d_name='sales';";
	//database->select(sql_select3);

	//database->flush();
	// string sql_create="CREATE TABLE student(id INT PRIMARY KEY,name varchar(100),age INT);";
	// database->createTable(sql_create);
	// string sql_create2="CREATE TABLE teacher(t_id INT PRIMARY KEY,name varchar(100),age INT);";
	// database->createTable(sql_create2);
	// string sql_frop="DROP TABLE teacher;";
	// database->Drop(sql_frop);
	// FILE* file=fopen("new.bin","w");
	// fclose(file);
	// int a=remove("new.bin");
	// if(a==0) cout<<"删除成功"<<endl;
	// else cout<<"删除失败"<<endl;
	//database->run();
	// string sql_create="CREATE TABLE student(id INT PRIMARY KEY,name varchar(100),age INT);";
	// database->createTable(sql_create);
	// database->printTableNames();
	// string sql_create2="CREATE TABLE teacher(t_id INT PRIMARY KEY,name varchar(100),age INT);";
	// database->createTable(sql_create2);
	// string sql_drop="DROP TABLE student;";
	// database->Drop(sql_drop);
	// database->printTableNames();
	
	// database->flush();
	
	// string sql_create="CREATE TABLE student(id INT PRIMARY KEY,name varchar(100),age INT);";
	// db->createTable(sql_create);
	// string sql_insert1="INSERT INTO student (id,name,age) VALUES(1,'zhangsan',20),(2,'lisi',21),(3,'wangwu',22);";
	// db->insert(sql_insert1);
	// string sql_select1="SELECT * FROM student;";
	// db->select(sql_select1);
	// string sql_create2="CREATE TABLE teacher(id INT PRIMARY KEY,name varchar(100),age INT);";
	// db->createTable(sql_create);
	

}

char* getString(int index){
	char* s=new char[1024];
	
	s[0]=(char)('a'+index-1);
	s[1]=(char)('a'+index-1);
	s[2]='\0';
	return s;
}
void print_add_leaf(BPlusTree* bp,KEY_KIND key){
	int size=0;
	if (key==INT_KEY)size=sizeof(int);
	else if(key==LL_KEY)size=sizeof(long long);
	else if(key==STRING_KEY) size=100;//string的最大大小先写死
	for(int i=2;i<NUM_ALL_BLOCK;i++){
		if(bp->Block_GRAPH[i]==BLOCK_LEAF){
			CLeafNode* c=new CLeafNode("table.bin",key,size,i);
			//cout<<"父亲为"<<c->getPtFather()<<endl;
			c->print_data();
			cout<<endl;
		}
		else if(bp->Block_GRAPH[i]==BLOCK_INTER){
			CInternalNode* c=new CInternalNode("table.bin",key,size,i);
			c->print_data();
			cout<<endl;
		}
		
	}
}

int main(){
    
	DataBase* database=new DataBase();
	//database->init();
	database->run();
	


	return 0;
}
int test_key_ll(){
	const char* fname = "table.bin";
	// xiebiao(LL_KEY);
	// BPlusTree* bp = new BPlusTree(fname);
	// for(int i=1;i<=100;i++){
	// 	bp->Insert((void*)new long long(i));
	// }
	// bp->Delete((void*)new long long(88));
	// for(int i=1;i<=100;i++){
	// 	bool a=bp->Search((void*)new long long(i),(char*)fname);
	// 	cout<<i<<" "<<a<<endl;
	// }
	// bp->flush_file();
	// delete bp;
	BPlusTree* bp1 = new BPlusTree(fname);
	CInternalNode* root=(CInternalNode*)bp1->GetRoot();
	cout<<"root的偏移量为："<<root->getPtSelf()<<endl;
	print_add_leaf(bp1,LL_KEY);
	return 0;
}
int test_string(){
	//cout<<getString(1)<<endl;
	const char* fname = "table.bin";
	//xiebiao(STRING_KEY);
	BPlusTree* bp = new BPlusTree(fname);
	for(int i=1;i<=26;i++){
		bp->Insert(getString(i));
	}
	bp->Delete(getString(5));
	
	
	// // bp->Delete((void*)new long long(88));
	// // for(int i=1;i<=100;i++){
	// // 	bool a=bp->Search((void*)new long long(i),(char*)fname);
	// // 	cout<<i<<" "<<a<<endl;
	// // }
	bp->flush_file();
	delete bp;
	BPlusTree* bp1 = new BPlusTree(fname);
	CInternalNode* root=(CInternalNode*)bp1->GetRoot();
	cout<<"root的偏移量为："<<root->getPtSelf()<<endl;
	print_add_leaf(bp1,STRING_KEY);



	return 0;
}

